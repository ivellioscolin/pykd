#include "stdafx.h"

#include <algorithm>
#include <sstream>
#include <iomanip> 

#include <DbgEng.h>

#include "dbgout.h"
#include "arglist.h"
#include "pyinterpret.h"
#include "pyapi.h"
#include "pyclass.h"
#include "version.h"


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
        DEBUG_OUTCTL_THIS_CLIENT,
        DEBUG_OUTPUT_NORMAL,
        sstr.str().c_str()
        );

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
    "\n"
    "\tcommand samples:\n"
    "\t\"!py\"                          : run REPL\n"
    "\t\"!py --local\"                  : run REPL in the isolated namespace\n"
    "\t\"!py -g script.py 10 \"string\"\" : run script file with argument in the commom namespace\n"
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
        DEBUG_OUTCTL_THIS_CLIENT,
        DEBUG_OUTPUT_NORMAL,
        printUsageMsg
        );

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////

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
    client->SetOutputMask(DEBUG_OUTPUT_NORMAL | DEBUG_OUTPUT_ERROR);

    try {

        Options  opts(getArgsList(args));

        if (opts.showHelp)
            throw std::exception(printUsageMsg);

        int  majorVersion = opts.pyMajorVersion;
        int  minorVersion = opts.pyMinorVersion;

        getPythonVersion(majorVersion, minorVersion);

        AutoInterpreter  autoInterpreter(opts.global, majorVersion, minorVersion);

        PyObjectRef  dbgOut = make_pyobject<DbgOut>(client);
        PySys_SetObject("stdout", dbgOut);

        PyObjectRef  dbgErr = make_pyobject<DbgOut>(client);
        PySys_SetObject("stderr", dbgErr);

        PyObjectRef dbgIn = make_pyobject<DbgIn>(client);
        PySys_SetObject("stdin", dbgIn);

        PyObjectRef  mainName = IsPy3() ? PyUnicode_FromString("__main__") : PyString_FromString("__main__"); 
        PyObjectRef  mainMod = PyImport_Import(mainName);
        PyObjectRef  globals = PyObject_GetAttrString(mainMod, "__dict__");

        checkPykd();

        InterruptWatch  interruptWatch(client);

        if (opts.args.empty())
        {
            PyObjectRef  result = PyRun_String("import pykd\nfrom pykd import *\n", Py_file_input, globals, globals);
            PyErr_Clear();
            result = PyRun_String("import code\ncode.InteractiveConsole(globals()).interact()\n", Py_file_input, globals, globals);
        }
        else
        {
            std::string  scriptFileName = getScriptFileName(opts.args[0]);
            if (scriptFileName.empty())
                throw std::invalid_argument("script not found\n");

            if (IsPy3())
            {
                // устанавиливаем питоновские аргументы
                std::vector<wchar_t*>  pythonArgs(opts.args.size());

                std::wstring  scriptFileNameW = _bstr_t(scriptFileName.c_str());

                pythonArgs[0] = const_cast<wchar_t*>(scriptFileNameW.c_str());

                for (size_t i = 1; i < opts.args.size(); ++i)
                {
                    std::wstring  argw = _bstr_t(opts.args[i].c_str());
                    pythonArgs[i] = const_cast<wchar_t*>(argw.c_str());
                }

                PySys_SetArgv_Py3((int)opts.args.size(), &pythonArgs[0]);

                FILE*  fs = _Py_fopen(scriptFileName.c_str(), "r");
                if ( !fs )
                    throw std::invalid_argument("script not found\n");

                PyObjectRef result = PyRun_File(fs, scriptFileName.c_str(), Py_file_input, globals, globals);
            }
            else
            {
                std::vector<char*>  pythonArgs(opts.args.size());

                pythonArgs[0] = const_cast<char*>(scriptFileName.c_str());

                for (size_t i = 1; i < opts.args.size(); ++i)
                    pythonArgs[i] = const_cast<char*>(opts.args[i].c_str());

                PySys_SetArgv((int)opts.args.size(), &pythonArgs[0]);

                PyObjectRef  pyfile = PyFile_FromString(pythonArgs[0], "r");
                if (!pyfile)
                    throw std::invalid_argument("script not found\n");

                FILE *fs = PyFile_AsFile(pyfile);

                PyObjectRef result = PyRun_File(fs, scriptFileName.c_str(), Py_file_input, globals, globals);
            }
        }

        handleException();
    }
    catch (std::exception &e)
    {
        CComQIPtr<IDebugControl>  control = client;

        control->ControlledOutput(
            DEBUG_OUTCTL_THIS_CLIENT,
            DEBUG_OUTPUT_ERROR,
            e.what()
            );
    }

    client->SetOutputMask(oldMask);

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

        Options  opts(getArgsList(args));

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
            DEBUG_OUTCTL_THIS_CLIENT,
            DEBUG_OUTPUT_ERROR,
            e.what()
            );
    }

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

std::string findScript(const std::string &fullFileName)
{
    if (GetFileAttributesA(fullFileName.c_str()) != INVALID_FILE_ATTRIBUTES)
        return fullFileName;

    PyObjectBorrowedRef  pathLst = PySys_GetObject("path");

    size_t  pathLstSize = PyList_Size(pathLst);

    for (size_t i = 0; i < pathLstSize; i++)
    {
        char  *path = PyString_AsString(PyList_GetItem(pathLst, i));

        DWORD bufSize = SearchPathA(
            path,
            fullFileName.c_str(),
            NULL,
            0,
            NULL,
            NULL);

        if (bufSize > 0)
        {
            bufSize += 1;
            std::vector<char> fullFileNameCStr(bufSize);
            char *partFileNameCStr = NULL;

            bufSize = SearchPathA(
                path,
                fullFileName.c_str(),
                NULL,
                bufSize,
                &fullFileNameCStr[0],
                &partFileNameCStr);

            DWORD   fileAttr = GetFileAttributesA(&fullFileNameCStr[0]);

            if ((fileAttr & FILE_ATTRIBUTE_DIRECTORY) == 0)
                return std::string(&fullFileNameCStr[0]);
        }

    }

    return "";
}

///////////////////////////////////////////////////////////////////////////////

std::string getScriptFileName(const std::string &scriptName)
{
    std::string scriptFileName = findScript(scriptName);

    if (scriptFileName.empty())
    {
        std::string scriptNameLow;
        scriptNameLow.resize(scriptName.size());
        std::transform(
            scriptName.begin(),
            scriptName.end(),
            scriptNameLow.begin(),
            ::tolower);
        if (scriptNameLow.rfind(".py") != (scriptNameLow.length() - 3))
            scriptFileName = findScript(scriptName + ".py");
    }

    return scriptFileName;
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
        if (2 == interpret.majorVersion && 7 == interpret.minorVersion)
        {
            majorVersion = 2;
            minorVersion = 7;
            return;
        }
    }

    for (auto interpret : interpreterList)
    {
        if (3 == interpret.majorVersion && 5 == interpret.minorVersion)
        {
            majorVersion = 3;
            minorVersion = 5;
            return;
        }
    }

    for (auto interpret : interpreterList)
    {
        if (2 == interpret.majorVersion &&
            minorVersion <= interpret.minorVersion )
        {
            found = true;
            majorVersion = 2;
            minorVersion = interpret.minorVersion;
        }
    }

    if (found)
        return;

    for (auto interpret : interpreterList)
    {
        if (3 == interpret.majorVersion &&
            minorVersion <= interpret.minorVersion )
        {
            found = true;
            majorVersion = 3;
            minorVersion = interpret.minorVersion;
        }
    }

    if (found)
        return;

    if (!found)
        throw std::exception("failed to find python interpreter\n");
}

///////////////////////////////////////////////////////////////////////////////
