#include "stdafx.h"

#include <algorithm>
#include <sstream>
#include <fstream>
#include <iomanip> 
#include <regex>

#include <DbgEng.h>

#include "dbgout.h"
#include "arglist.h"
#include "pyinterpret.h"
#include "pyapi.h"
#include "pyclass.h"
#include "version.h"

//////////////////////////////////////////////////////////////////////////////

static int  defaultMajorVersion = 2;
static int  defaultMinorVersion = 7;

//////////////////////////////////////////////////////////////////////////////

void handleException();
std::string getScriptFileName(const std::string &scriptName);
void getPythonVersion(int&  majorVersion, int& minorVersion);
void getDefaultPythonVersion(int& majorVersion, int& minorVersion);

//////////////////////////////////////////////////////////////////////////////

class InterruptWatch
{
public:

    InterruptWatch(PDEBUG_CLIENT client)
    {
        m_control = client;
        m_stopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        m_thread = CreateThread(NULL, 0, threadRoutine, this, 0, NULL);
    }

    ~InterruptWatch()
    {
        SetEvent(m_stopEvent);
        WaitForSingleObject(m_thread, INFINITE);
        CloseHandle(m_stopEvent);
        CloseHandle(m_thread);
    }

    static int quit(void *context)
    {
        HANDLE   quitEvent = (HANDLE)context;
        PyErr_SetString(PyExc_SystemExit(), "CTRL+BREAK");
        SetEvent(quitEvent);
        return -1;
    }

private:

    static DWORD WINAPI threadRoutine(LPVOID lpParameter) {
        return  static_cast<InterruptWatch*>(lpParameter)->interruptWatchRoutine();
    }

    DWORD InterruptWatch::interruptWatchRoutine()
    {
        while (WAIT_TIMEOUT == WaitForSingleObject(m_stopEvent, 250))
        {
            HRESULT  hres = m_control->GetInterrupt();
            if (hres == S_OK)
            {
                HANDLE  quitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
                PyGILState_STATE state = PyGILState_Ensure();
                Py_AddPendingCall(&quit, (void*)quitEvent);
                PyGILState_Release(state);
                WaitForSingleObject(quitEvent, INFINITE);
                CloseHandle(quitEvent);
            }
        }

        return 0;
    }

    HANDLE  m_thread;

    HANDLE  m_stopEvent;

    CComQIPtr<IDebugControl>  m_control;
};

//////////////////////////////////////////////////////////////////////////////

extern "C"
HRESULT
CALLBACK
DebugExtensionInitialize(
    PULONG  Version,
    PULONG  Flags
)
{
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////

extern "C"
VOID
CALLBACK
DebugExtensionUninitialize()
{
   stopAllInterpreter();
}

//////////////////////////////////////////////////////////////////////////////

std::string make_version(int major, int minor)
{
    std::stringstream sstr;
    sstr << std::dec << major << '.' << minor; 

#ifdef _WIN64

    sstr << " x86-64";

#else

    sstr << " x86-32";

#endif

    return sstr.str();
}

extern "C"
HRESULT
CALLBACK
info(
    PDEBUG_CLIENT client,
    PCSTR args
)
{
    try 
    {
        std::stringstream   sstr;

        sstr <<std::endl << "pykd bootstrapper version: " << PYKDEXT_VERSION_MAJOR << '.' << PYKDEXT_VERSION_MINOR << '.' 
            << PYKDEXT_VERSION_SUBVERSION << '.' << PYKDEXT_VERSION_BUILDNO << std::endl;

        std::list<InterpreterDesc>   interpreterList = getInstalledInterpreter();

        int defaultMajor;
        int defaultMinor;

        getDefaultPythonVersion(defaultMajor, defaultMinor);

        sstr << std::endl << "Installed python:" << std::endl << std::endl;
        sstr << std::setw(16) << std::left << "Version:" << std::setw(12) << std::left << "Status: " << std::left << "Image:" <<  std::endl;
        sstr << "------------------------------------------------------------------------------" << std::endl;
        if (interpreterList.size() > 0)
        {
            for (const InterpreterDesc& desc : interpreterList)
            {
                if ( defaultMajor == desc.majorVersion && defaultMinor == desc.minorVersion)
                    sstr << "* ";
                else
                    sstr << "  ";

                sstr << std::setw(14) << std::left << make_version(desc.majorVersion, desc.minorVersion);
            
                sstr << std::setw(12) << std::left << (isInterpreterLoaded(desc.majorVersion, desc.minorVersion) ? "Loaded" : "Unloaded");

                sstr << desc.imagePath << std::endl;
            }
        }
        else
        {
            sstr << "No python interpreter found" << std::endl; 
        }

        sstr << std::endl;

        CComQIPtr<IDebugControl>  control = client;

        control->ControlledOutput(
            DEBUG_OUTCTL_AMBIENT_TEXT,
            DEBUG_OUTPUT_NORMAL,
            "%s",
            sstr.str().c_str()
            );
    } 
    catch(std::exception &e)
    {
        CComQIPtr<IDebugControl>  control = client;

        control->ControlledOutput(
            DEBUG_OUTCTL_AMBIENT_TEXT,
            DEBUG_OUTPUT_ERROR,
            "%s",
            e.what()
            );
    }

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////

extern "C"
HRESULT
CALLBACK
selectVersion(
    PDEBUG_CLIENT client,
    PCSTR args
)
{
    Options  opts(args);

    int  majorVersion = opts.pyMajorVersion;
    int  minorVersion = opts.pyMinorVersion;

    getPythonVersion(majorVersion, minorVersion);

    if ( opts.pyMajorVersion == majorVersion && opts.pyMinorVersion == minorVersion )
    {
        defaultMajorVersion = majorVersion;
        defaultMinorVersion = minorVersion;
    }

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////

static const char  printUsageMsg[] =
    "\n"
    "usage:\n"
    "\n"
    "!help\n"
    "\tprint this text\n"
    "\n"
    "!info\n"
    "\tlist installed python interpreters\n"
    "\n"
    "!select version\n"
    "\tchange default version of a python interpreter\n"
    "\n"
    "!py [version] [options] [file]\n"
    "\trun python script or REPL\n"
    "\n"
    "\tVersion:\n"
    "\t-2           : use Python2\n"
    "\t-2.x         : use Python2.x\n"
    "\t-3           : use Python3\n"
    "\t-3.x         : use Python3.x\n"
    "\n"
    "\tOptions:\n"
    "\t-g --global  : run code in the common namespace\n"
    "\t-l --local   : run code in the isolated namespace\n"
    "\t-m --module  : run module as the __main__ module ( see the python command line option -m )\n"
    "\n"
    "\tcommand samples:\n"
    "\t\"!py\"                          : run REPL\n"
    "\t\"!py --local\"                  : run REPL in the isolated namespace\n"
    "\t\"!py -g script.py 10 \"string\"\" : run a script file with an argument in the commom namespace\n"
    "\t\"!py -m module_name\" : run a named module as the __main__\n"
    "\n"
    "!pip [version] [args]\n"
    "\trun pip package manager\n"
    "\n"
    "\tVersion:\n"
    "\t-2           : use Python2\n"
    "\t-2.x         : use Python2.x\n"
    "\t-3           : use Python3\n"
    "\t-3.x         : use Python3.x\n"
    "\n"
    "\tpip command samples:\n"
    "\t\"pip list\"                   : show all installed packagies\n"
    "\t\"pip install pykd\"           : install pykd\n"
    "\t\"pip install --upgrade pykd\" : upgrade pykd to the latest version\n"
    "\t\"pip show pykd\"              : show info about pykd package\n"
    ;


//////////////////////////////////////////////////////////////////////////////

extern "C"
HRESULT
CALLBACK
help(
    PDEBUG_CLIENT client,
    PCSTR args
    )
{
    CComQIPtr<IDebugControl>  control = client;

    control->ControlledOutput(
        DEBUG_OUTCTL_AMBIENT_TEXT,
        DEBUG_OUTPUT_NORMAL,
        "%s",
        printUsageMsg
        );

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////


static const std::regex  shebangRe("^#!\\s*python([2,3])(?:\\.(\\d))?$");

static volatile long recursiveGuard = 0L;

extern "C"
HRESULT
CALLBACK
py(
    PDEBUG_CLIENT client,
    PCSTR args
)
{

    ULONG   oldMask;
    client->GetOutputMask(&oldMask);
    client->SetOutputMask(DEBUG_OUTPUT_NORMAL|DEBUG_OUTPUT_ERROR|DEBUG_OUTPUT_WARNING|DEBUG_OUTPUT_DEBUGGEE );

    try {

        if ( 1 < ++recursiveGuard  )
            throw std::exception( "can not run !py command recursive\n");

        Options  opts(args);

        if (opts.showHelp)
            throw std::exception(printUsageMsg);

        int  majorVersion = opts.pyMajorVersion;
        int  minorVersion = opts.pyMinorVersion;

        std::string  scriptFileName;

        if ( opts.args.size() > 0 && !opts.runModule )
        {
            scriptFileName = getScriptFileName(opts.args[0]);
            if ( scriptFileName.empty() )
                throw std::invalid_argument("script not found\n");
        }

        if ( !opts.runModule && majorVersion == -1 && minorVersion == -1 )
        {
            std::ifstream  scriptFile(scriptFileName);

            if ( scriptFile.is_open() )
            {
                std::string  firstline;
                std::getline(scriptFile, firstline);

                std::smatch  mres;
                if (std::regex_match(firstline, mres, shebangRe))
                {
                    majorVersion = atol(std::string(mres[1].first, mres[1].second).c_str());

                    if (mres[2].matched)
                    {
                        minorVersion = atol(std::string(mres[2].first, mres[2].second).c_str());
                    }
                }
            }
        }

        getPythonVersion(majorVersion, minorVersion);

        AutoInterpreter  autoInterpreter(opts.global, majorVersion, minorVersion);

        PyObjectRef  mainMod = PyImport_ImportModule("__main__");
        PyObjectRef  globals = PyObject_GetAttrString(mainMod, "__dict__");

        PyObjectRef  dbgOut = make_pyobject<DbgOut>(client);
        PySys_SetObject("stdout", dbgOut);

        PyObjectRef  dbgErr = make_pyobject<DbgOut>(client);
        PySys_SetObject("stderr", dbgErr);

        PyObjectRef dbgIn = make_pyobject<DbgIn>(client);
        PySys_SetObject("stdin", dbgIn);

        InterruptWatch  interruptWatch(client);

        if (opts.args.empty())
        {
            PyObjectRef  result = PyRun_String("import pykd\nfrom pykd import *\n", Py_file_input, globals, globals);
            PyErr_Clear();
            result = PyRun_String("import code\ncode.InteractiveConsole(globals()).interact()\n", Py_file_input, globals, globals);
        }
        else 
        {
            if (IsPy3())
            {
                std::wstring  scriptFileNameW = _bstr_t(scriptFileName.c_str());

                // устанавиливаем питоновские аргументы
                std::vector<std::wstring>   argws(opts.args.size());

                if ( !scriptFileNameW.empty() )
                    argws[0] = scriptFileNameW;
                else
                    argws[0] = L"";

                for (size_t i = 1; i < opts.args.size(); ++i)
                    argws[i] = _bstr_t(opts.args[i].c_str());

                std::vector<wchar_t*>  pythonArgs(opts.args.size());
                for (size_t i = 0; i < opts.args.size(); ++i)
                    pythonArgs[i] = const_cast<wchar_t*>(argws[i].c_str());

                PySys_SetArgv_Py3((int)opts.args.size(), &pythonArgs[0]);

                if ( opts.runModule )
                {
                   std::stringstream sstr;
                   sstr << "runpy.run_module(\"" << opts.args[0] << "\", run_name='__main__', alter_sys=True)" << std::endl;

                    PyObjectRef result;
                    result = PyRun_String("import runpy\n", Py_file_input, globals, globals);
                    result = PyRun_String(sstr.str().c_str(), Py_file_input, globals, globals);
                }
                else
                {
                    FILE*  fs = _Py_fopen(scriptFileName.c_str(), "r");
                    if ( !fs )
                        throw std::invalid_argument("script not found\n");

                    PyObjectRef result = PyRun_File(fs, scriptFileName.c_str(), Py_file_input, globals, globals);
                }
            }
            else
            {
                std::vector<char*>  pythonArgs(opts.args.size());

                if ( !scriptFileName.empty() )
                    pythonArgs[0] = const_cast<char*>(scriptFileName.c_str());
                else
                    pythonArgs[0] = "";

                for (size_t i = 1; i < opts.args.size(); ++i)
                    pythonArgs[i] = const_cast<char*>(opts.args[i].c_str());

                PySys_SetArgv((int)opts.args.size(), &pythonArgs[0]);

                if ( opts.runModule )
                {
                   std::stringstream sstr;
                   sstr << "runpy.run_module('" << opts.args[0] << "', run_name='__main__', alter_sys=True)" << std::endl;

                    PyObjectRef result;
                    result = PyRun_String("import runpy\n", Py_file_input, globals, globals);
                    result = PyRun_String(sstr.str().c_str(), Py_file_input, globals, globals);
                }
                else
                {
                    PyObjectRef  pyfile = PyFile_FromString(const_cast<char*>(scriptFileName.c_str()), "r");
                    if (!pyfile)
                        throw std::invalid_argument("script not found\n");

                    FILE *fs = PyFile_AsFile(pyfile);

                    PyObjectRef result = PyRun_File(fs, scriptFileName.c_str(), Py_file_input, globals, globals);
                }
            }
        }

        handleException();

        if ( !opts.global )
            PyDict_Clear(globals);
    }
    catch (std::exception &e)
    {
        CComQIPtr<IDebugControl>  control = client;

        control->ControlledOutput(
            DEBUG_OUTCTL_AMBIENT_TEXT,
            DEBUG_OUTPUT_ERROR,
            "%s",
            e.what()
            );
    }

    client->SetOutputMask(oldMask);

    --recursiveGuard;

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////

extern "C"
HRESULT
CALLBACK
pip(
    PDEBUG_CLIENT client,
    PCSTR args
)
{

    try {

        if ( 1 < ++recursiveGuard  )
            throw std::exception( "can not run !pip command recursive\n");

        Options  opts(args);

        int  majorVersion = opts.pyMajorVersion;
        int  minorVersion = opts.pyMinorVersion;

        getPythonVersion(majorVersion, minorVersion);

        AutoInterpreter  autoInterpreter(true, majorVersion, minorVersion);

        PyObjectRef  dbgOut = make_pyobject<DbgOut>(client);
        PySys_SetObject("stdout", dbgOut);

        PyObjectRef  dbgErr = make_pyobject<DbgOut>(client);
        PySys_SetObject("stderr", dbgErr);

        PyObjectRef dbgIn = make_pyobject<DbgIn>(client);
        PySys_SetObject("stdin", dbgIn);

        PyObjectRef  mainName = IsPy3() ? PyUnicode_FromString("__main__") : PyString_FromString("__main__");
        PyObjectRef  mainMod = PyImport_Import(mainName);
        PyObjectRef  globals = PyObject_GetAttrString(mainMod, "__dict__");


        std::stringstream  sstr;
        sstr << "pip.main([";
        for (auto arg : opts.args)
            sstr << '\'' << arg << '\'' << ',';
        sstr << "])\n";

        PyObjectRef result;
        result = PyRun_String("import pip\n", Py_file_input, globals, globals);
        result = PyRun_String("pip.logger.consumers = []\n", Py_file_input, globals, globals);
        result = PyRun_String(sstr.str().c_str(), Py_file_input, globals, globals);

        handleException();
    }
    catch (std::exception &e)
    {
        CComQIPtr<IDebugControl>  control = client;

        control->ControlledOutput(
            DEBUG_OUTCTL_AMBIENT_TEXT, 
            DEBUG_OUTPUT_ERROR,
            "%s",
            e.what()
            );
    }

    --recursiveGuard;

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////

void handleException()
{
    // ошибка в скрипте
    PyObject  *errtype = NULL, *errvalue = NULL, *traceback = NULL;

    PyErr_Fetch(&errtype, &errvalue, &traceback);

    PyErr_NormalizeException(&errtype, &errvalue, &traceback);

    if (errtype && errtype != PyExc_SystemExit())
    {
        PyObjectRef  traceback_module = PyImport_ImportModule("traceback");

        std::stringstream  sstr;

        PyObjectRef  format_exception = PyObject_GetAttrString(traceback_module, "format_exception");

        PyObjectRef  args = PyTuple_New(3);
        PyTuple_SetItem(args, 0, errtype ? errtype : Py_None());
        PyTuple_SetItem(args, 1, errvalue ? errvalue : Py_None());
        PyTuple_SetItem(args, 2, traceback ? traceback : Py_None());

        PyObjectRef  lst = PyObject_Call(format_exception, args, NULL);

        sstr << std::endl << std::endl;

        for (size_t i = 0; i < PyList_Size(lst); ++i)
        {
            PyObjectBorrowedRef  item = PyList_GetItem(lst, i);
            sstr << std::string(convert_from_python(item)) << std::endl;
        }

        throw std::exception(sstr.str().c_str());
    }

    if (errtype) Py_DecRef(errtype);
    if (errvalue) Py_DecRef(errvalue);
    if (traceback) Py_DecRef(traceback);
}

///////////////////////////////////////////////////////////////////////////////

void getPathList( std::list<std::string>  &pathStringLst)
{
    PyObjectBorrowedRef  pathLst = PySys_GetObject("path");

    size_t  pathLstSize = PyList_Size(pathLst);

    for (size_t i = 0; i < pathLstSize; i++)
    {
        PyObjectBorrowedRef  pathLstItem = PyList_GetItem(pathLst, i);

        if ( IsPy3() )
        {
            std::vector<wchar_t>  buf(0x10000);
            size_t  len = buf.size();
            PyUnicode_AsWideChar(pathLstItem, &buf[0], len);

            DWORD  attr =  GetFileAttributesW(&buf[0]);
            if ( attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
                continue;

            pathStringLst.push_back( std::string(_bstr_t(&buf[0])));
        }
        else
        {
            char*  path = PyString_AsString(pathLstItem);

            DWORD  attr =  GetFileAttributesA(path);
            if ( attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
                continue;

            pathStringLst.push_back(path);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

std::string getScriptFileName(const std::string &scriptName)
{
    char*  ext = NULL;

    DWORD searchResult = 
        SearchPathA(
            NULL,
            scriptName.c_str(),
            NULL,
            0,
            NULL,
            NULL );

    if ( searchResult == 0 )
    {
        ext = ".py";

        searchResult = 
            SearchPathA(
                NULL,
                scriptName.c_str(),
                ext,
                0,
                NULL,
                NULL );

        if ( searchResult == 0 )
        {
            return "";
        }
    }

    std::vector<char>  pathBuffer(searchResult);

    searchResult = 
        SearchPathA(
            NULL,
            scriptName.c_str(),
            ext,
            pathBuffer.size(),
            &pathBuffer.front(),
            NULL );

    return std::string(&pathBuffer.front(), searchResult);
}

///////////////////////////////////////////////////////////////////////////////

void getPythonVersion(int&  majorVersion, int& minorVersion)
{
    if (majorVersion == -1 && minorVersion == -1 )
        return getDefaultPythonVersion(majorVersion, minorVersion);

    std::list<InterpreterDesc>   interpreterList = getInstalledInterpreter();

    bool  found = false;
    bool  anyMinorVersion = minorVersion == -1;
    bool  anyMajorVersion = majorVersion == -1;
    
    if (anyMajorVersion)
    {
        for (auto interpret : interpreterList)
        {

            if (2 == interpret.majorVersion &&
                anyMinorVersion ? (minorVersion <= interpret.minorVersion) : (minorVersion == interpret.minorVersion))
            {
                found = true;
                minorVersion = interpret.minorVersion;
            }
        }

        if (found)
        {
            majorVersion = 2;
            return;
        }
    }

    for (auto interpret : interpreterList)
    {
        if (anyMajorVersion ? (majorVersion <= interpret.majorVersion) : (majorVersion==interpret.majorVersion) &&
            anyMinorVersion ? (minorVersion <= interpret.minorVersion) : (minorVersion == interpret.minorVersion))
        {
            found = true;
            minorVersion = interpret.minorVersion;
            majorVersion = interpret.majorVersion;
        }
    }

    if (!found)
        throw std::exception("failed to find python interpreter\n");
}

///////////////////////////////////////////////////////////////////////////////

void getDefaultPythonVersion(int& majorVersion, int& minorVersion)
{
    std::list<InterpreterDesc>   interpreterList = getInstalledInterpreter();

    bool  found = false;
    majorVersion = -1;
    minorVersion = -1;
    
    for (auto interpret : interpreterList)
    {
        if (defaultMajorVersion == interpret.majorVersion && defaultMinorVersion == interpret.minorVersion)
        {
            majorVersion = defaultMajorVersion;
            minorVersion = defaultMinorVersion;
            return;
        }
    }

    for (auto interpret : interpreterList)
    {
        if (2 == interpret.majorVersion &&  minorVersion <= interpret.minorVersion )
        {
            found = true;
            majorVersion = interpret.majorVersion;
            minorVersion = interpret.minorVersion;
        }
    }

    if (found)
        return;

    for (auto interpret : interpreterList)
    {
        if (3 == interpret.majorVersion && minorVersion <= interpret.minorVersion )
        {
            found = true;
            majorVersion = interpret.majorVersion;
            minorVersion = interpret.minorVersion;
        }
    }

    if (found)
        return;

    if (!found)
        throw std::exception("failed to find python interpreter\n");
}

///////////////////////////////////////////////////////////////////////////////
