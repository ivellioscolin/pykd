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


//////////////////////////////////////////////////////////////////////////////

void handleException();
std::string getScriptFileName(const std::string &scriptName);
void  getPythonVersion(int&  majorVersion, int& minorVersion);

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
}

//////////////////////////////////////////////////////////////////////////////

std::string make_version(int major, int minor)
{
    std::stringstream sstr;
    sstr << std::dec << major << '.' << minor;
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
    std::list<InterpreterDesc>   interpreterList = getInstalledInterpreter();
    std::stringstream   sstr;

    sstr << std::endl << "Installed python" << std::endl << std::endl;
    sstr << std::setw(12) << std::left << "Version:" <<  std::left << "Image:" <<  std::endl;
    sstr << "------------------------------------------------------------------------------" << std::endl;
    if (interpreterList.size() > 0)
    {
        for (const InterpreterDesc& desc : interpreterList)
        {
            sstr << std::setw(12) << std::left << make_version(desc.majorVersion, desc.minorVersion);
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
    "usage:\n"
    "!py [options] [file]\n"
    "\tOptions:\n"
    "\t-g --global  : run code in the common namespace\n"
    "\t-l --local   : run code in the isolate namespace\n"
    "!install\n"
    "!upgrade\n";

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

        InterruptWatch  interruptWatch(client);

        if (opts.args.empty())
        {
            PyObjectRef  result = PyRun_String("__import__('code').InteractiveConsole(__import__('__main__').__dict__).interact()\n", Py_file_input, globals, globals);
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

    return S_OK;

}


//////////////////////////////////////////////////////////////////////////////


void handleException()
{
    // ошибка в скрипте
    PyObject  *errtype = NULL, *errvalue = NULL, *traceback = NULL;

    PyErr_Fetch(&errtype, &errvalue, &traceback);

    PyErr_NormalizeException(&errtype, &errvalue, &traceback);

    if (errtype != PyExc_SystemExit() )
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

    Py_DecRef(errtype);
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

void  getPythonVersion(int&  majorVersion, int& minorVersion)
{
    std::list<InterpreterDesc>   interpreterList = getInstalledInterpreter();

    bool  found = false;
    bool  anyMinorVersion = minorVersion == -1;
    for (auto interpret : interpreterList)
    {
        if (majorVersion == interpret.majorVersion && 
            anyMinorVersion ? (minorVersion <= interpret.minorVersion) : (minorVersion == interpret.minorVersion))
        {
            found = true;
            minorVersion = interpret.minorVersion;
        }
    }

    if (!found)
        throw std::exception("failed to find python interpreter\n");
}

///////////////////////////////////////////////////////////////////////////////




















/*
#include <vector>

#include <DbgEng.h>
#include <atlbase.h>
#include <comutil.h>

#include <boost/python.hpp>
namespace python = boost::python;

#include <boost/tokenizer.hpp>

///////////////////////////////////////////////////////////////////////////////

class AutoRestorePyState
{
public:

    AutoRestorePyState()
    {
        m_state = PyEval_SaveThread();
    }

    ~AutoRestorePyState()
    {
        PyEval_RestoreThread(m_state);
    }

private:

    PyThreadState*    m_state;
};

//////////////////////////////////////////////////////////////////////////////

class DbgOut
{
public:

    DbgOut(PDEBUG_CLIENT client)
        : m_control(client)
    {}

    void write(const std::wstring& str)
    {
        AutoRestorePyState  pystate;

        m_control->ControlledOutputWide(
            DEBUG_OUTCTL_THIS_CLIENT,
            DEBUG_OUTPUT_NORMAL,
            L"%ws",
            str.c_str()
            );

    }

    void writedml(const std::wstring& str) 
    {
        AutoRestorePyState  pystate;

        m_control->ControlledOutputWide(
            DEBUG_OUTCTL_THIS_CLIENT | DEBUG_OUTCTL_DML,
            DEBUG_OUTPUT_NORMAL,
            L"%ws",
            str.c_str()
            );
    }

    void flush() {
    }

    std::wstring encoding() {
        return L"ascii";
    }

    bool closed() {
        return false;
    }

private:

    CComQIPtr<IDebugControl4>  m_control;
};

///////////////////////////////////////////////////////////////////////////////

class DbgIn
{
public:

    DbgIn(PDEBUG_CLIENT client)
        : m_control(client)
        {}

    std::wstring readline()
    {
        AutoRestorePyState  pystate;

        std::vector<wchar_t>  inputBuffer(0x10000);

        ULONG  read = 0;
        m_control->InputWide(&inputBuffer[0], static_cast<ULONG>(inputBuffer.size()), &read);

        std::wstring  inputstr = std::wstring(&inputBuffer[0]);

        return inputstr.empty() ? L"\n" : inputstr;
    }

    bool closed() {
        return false;
    }


private:

    CComQIPtr<IDebugControl4>  m_control;
};

///////////////////////////////////////////////////////////////////////////////

class PythonInterpreter
{
public:

    PythonInterpreter()
    {
        PyThreadState*  state = Py_NewInterpreter();

        PyThreadState_Swap(state);

        m_state = PyEval_SaveThread();
    }

    ~PythonInterpreter()
    {
        PyEval_RestoreThread(m_state);

        PyInterpreterState  *interpreter = m_state->interp;

        while (interpreter->tstate_head != NULL)
        {
            PyThreadState   *threadState = (PyThreadState*)(interpreter->tstate_head);

            PyThreadState_Clear(threadState);

            PyThreadState_Swap(NULL);

            PyThreadState_Delete(threadState);
        }

        PyInterpreterState_Clear(interpreter);

        PyInterpreterState_Delete(interpreter);
    }

    void acivate()
    {
        PyEval_RestoreThread(m_state);
    }

    void deactivate()
    {
        m_state = PyEval_SaveThread();
    }

private:

    PyThreadState*  m_state;

};

///////////////////////////////////////////////////////////////////////////////

class PythonSingleton
{

public:

    static PythonSingleton* get()
    {
        if (!m_instance)
            m_instance = new PythonSingleton();

        return m_instance;
    }

    void stop()
    {
        delete m_globalInterpreter;
        m_globalInterpreter = 0;
        PyThreadState_Swap(m_globalState);
        m_globalState = PyEval_SaveThread();
    }

    void start()
    {
        PyEval_RestoreThread(m_globalState);

        m_globalInterpreter = new PythonInterpreter();

        //m_globalInterpreter->acivate();

        //python::object  main = boost::python::import("__main__");

        //python::object  main_namespace = main.attr("__dict__");

        //m_globalInterpreter->deactivate();
    }

    void acivateGlobal() {
        m_globalInterpreter->acivate();
    }

    void deactivateGlobal() {
        m_globalInterpreter->deactivate();
    }

    void acivateLocal() {
        PyEval_RestoreThread(m_globalState);
        m_locallInterpreter = new PythonInterpreter();
        m_locallInterpreter->acivate();
    }

    void deactivateLocal() {
        m_locallInterpreter->deactivate();
        delete m_locallInterpreter;
        m_locallInterpreter = 0;
        PyThreadState_Swap(m_globalState);
        m_globalState = PyEval_SaveThread();
    }

    void checkPykd()
    {
        if (m_pykdInit)
            return;

        python::handle<>  pykdHandle(python::allow_null(PyImport_ImportModule("pykd")));
        if (!pykdHandle)
            throw std::exception("Pykd package is not installed.You can install it by command \"!pykd.install\"");

        python::object       main = python::import("__main__");
        python::object       globalScope(main.attr("__dict__"));
        python::exec("__import__('pykd').initialize()", globalScope);

        m_pykdInit = true;
    }

private:

    static PythonSingleton*  m_instance;

    PythonSingleton()
    {
        Py_Initialize();
        PyEval_InitThreads();

        // Python debug output console helper classes
        python::class_<DbgOut>("dout", "dout", python::no_init)
            .def("write", &DbgOut::write)
            .def("writedml", &DbgOut::writedml)
            .def("flush", &DbgOut::flush)
            .add_property("encoding", &DbgOut::encoding)
            .add_property("closed", &DbgOut::closed);

        python::class_<DbgIn>("din", "din", python::no_init)
            .def("readline", &DbgIn::readline)
            .add_property("closed", &DbgIn::closed);


        m_globalState = PyEval_SaveThread();
    }

    PythonInterpreter*  m_globalInterpreter;
    PythonInterpreter*  m_locallInterpreter;
    PyThreadState*  m_globalState;

    bool  m_pykdInit;
};

PythonSingleton*   PythonSingleton::m_instance = 0;

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

    bool onInterrupt()
    {
        HANDLE  quitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        PyGILState_STATE state = PyGILState_Ensure();
        Py_AddPendingCall(&quit, (void*)quitEvent);
        PyGILState_Release(state);
        WaitForSingleObject(quitEvent, INFINITE);
        CloseHandle(quitEvent);
        return true;
    }

    static int quit(void *context)
    {
        HANDLE   quitEvent = (HANDLE)context;
        PyErr_SetString(PyExc_SystemExit, "CTRL+BREAK");
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

typedef  std::vector< std::string >  ArgsList;
typedef  boost::escaped_list_separator<char>    char_separator_t;
typedef  boost::tokenizer< char_separator_t >   char_tokenizer_t;

ArgsList  getArgsList(
    PCSTR args
    )
{
    std::string  argsStr(args);

    char_tokenizer_t  token(argsStr, char_separator_t("", " \t", "\""));
    ArgsList  argsList;

    for (char_tokenizer_t::iterator it = token.begin(); it != token.end(); ++it)
    {
        if (*it != "")
            argsList.push_back(*it);
    }

    return argsList;
}

//////////////////////////////////////////////////////////////////////////////

static const char  printUsageMsg[] =
    "usage:\n"
    "!py [options] [file]\n"
    "\tOptions:\n"
    "\t-g --global  : run code in the common namespace\n"
    "\t-l --local   : run code in the isolate namespace\n"
    "!install\n"
    "!upgrade\n";

void printUsage(PDEBUG_CLIENT client)
{
    CComQIPtr<IDebugControl>(client)->
        ControlledOutput(
            DEBUG_OUTCTL_THIS_CLIENT,
            DEBUG_OUTPUT_NORMAL,
            "%s",
            printUsageMsg
            );
}

//////////////////////////////////////////////////////////////////////////////

extern "C"
HRESULT
CALLBACK
DebugExtensionInitialize(
    PULONG  Version,
    PULONG  Flags
    )
{
    PythonSingleton::get()->start();
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////

extern "C"
VOID
CALLBACK
DebugExtensionUninitialize()
{
    PythonSingleton::get()->stop();
}

//////////////////////////////////////////////////////////////////////////////

std::string getScriptFileName(const std::string &scriptName);

void printException(DbgOut &dbgOut);

//////////////////////////////////////////////////////////////////////////////

extern "C"
HRESULT
CALLBACK
py(
    PDEBUG_CLIENT client,
    PCSTR args
    )
{
    ArgsList  argsList = getArgsList(args);

    bool  global = false;
    bool  local = false;
    bool  clean = false;

    if (!argsList.empty())
    {
        if (argsList[0] == "-h" || argsList[0] == "--help")
        {
            printUsage(client);
            return S_OK;
        }
        else
        if (argsList[0] == "-g" || argsList[0] == "--global")
        {
            global = true;
            argsList.erase(argsList.begin());
        }
        else
        if (argsList[0] == "-l" || argsList[0] == "--local")
        {
            local = true;
            argsList.erase(argsList.begin());
        }
    }

    if (argsList.size() > 0)
    {
        global = !(global || local) ? false : global; //set local by default
    }
    else
    {
        global = !(global || local) ? true : global; //set global by default
    }

    DbgOut   dbgOut(client);
    DbgOut   dbgErr(client);
    DbgIn    dbgIn(client);

    ULONG   oldMask;
    client->GetOutputMask(&oldMask);
    client->SetOutputMask(DEBUG_OUTPUT_NORMAL | DEBUG_OUTPUT_ERROR);


    if (global)
        PythonSingleton::get()->acivateGlobal();
    else
        PythonSingleton::get()->acivateLocal();

    try {

        InterruptWatch  interruptWatch(client);

        python::object  sys = python::import("sys");

        sys.attr("stdout") = python::object(dbgOut);
        sys.attr("stderr") = python::object(dbgErr);
        sys.attr("stdin") = python::object(dbgIn);

        python::object  main = python::import("__main__");
        python::object  globalScope(main.attr("__dict__"));

        PythonSingleton::get()->checkPykd();

        if (argsList.size() == 0)
        {

            python::exec("import pykd", globalScope);
            python::exec("from pykd import *", globalScope);
            python::exec("__import__('code').InteractiveConsole(__import__('__main__').__dict__).interact()", globalScope);
        }
        else
        {

            std::string  scriptFileName = getScriptFileName(argsList[0]);
            if (scriptFileName.empty())
                throw std::invalid_argument("script not found");

            // устанавиливаем питоновские аргументы
            char  **pythonArgs = new char*[argsList.size()];
        
            pythonArgs[0] = const_cast<char*>(scriptFileName.c_str());
        
            for (size_t i = 1; i < argsList.size(); ++i)
                pythonArgs[i] = const_cast<char*>(argsList[i].c_str());
        
            PySys_SetArgv((int)argsList.size(), pythonArgs);
        
            delete[]  pythonArgs;

            python::exec_file(scriptFileName.c_str(), globalScope);
        }
    }
    catch (const python::error_already_set&)
    {
        printException(dbgOut);
    }
    catch (const std::exception& invalidArg)
    {
        _bstr_t    bstrInavalidArg(invalidArg.what());
        dbgOut.write(std::wstring(bstrInavalidArg));
    }

    if (global)
        PythonSingleton::get()->deactivateGlobal();
    else
        PythonSingleton::get()->deactivateLocal();

    client->SetOutputMask(oldMask);

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////

extern "C"
HRESULT
CALLBACK
install(
    PDEBUG_CLIENT client,
    PCSTR args
    )
{
    DbgOut   dbgOut(client);
    DbgOut   dbgErr(client);
    DbgIn    dbgIn(client);

    PythonSingleton::get()->acivateGlobal();
    
    try {
    
        python::object  sys = python::import("sys");

        sys.attr("stdout") = python::object(dbgOut);
        sys.attr("stderr") = python::object(dbgErr);
        sys.attr("stdin") = python::object(dbgIn);

        // получаем доступ к глобальному мапу ( нужен для вызова exec_file )
        python::object       main = python::import("__main__");
        python::object       global(main.attr("__dict__"));

        python::exec("import pip\n", global);
        python::exec("pip.logger.consumers = []\n", global);
        python::exec("pip.main(['install', 'pykd'])\n", global);
    
    }
    catch (const python::error_already_set&)
    {
        printException(dbgOut);
    }
    catch (const std::exception& invalidArg)
    {
        _bstr_t    bstrInavalidArg(invalidArg.what());
        dbgOut.write(std::wstring(bstrInavalidArg));
    }
    
    PythonSingleton::get()->deactivateGlobal();

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////

extern "C"
HRESULT
CALLBACK
upgrade(
    PDEBUG_CLIENT client,
    PCSTR args
    )
{
    DbgOut   dbgOut(client);
    DbgOut   dbgErr(client);
    DbgIn    dbgIn(client);

    PythonSingleton::get()->acivateGlobal();

    try {

        python::object  sys = python::import("sys");

        sys.attr("stdout") = python::object(dbgOut);
        sys.attr("stderr") = python::object(dbgErr);
        sys.attr("stdin") = python::object(dbgIn);

        // получаем доступ к глобальному мапу ( нужен для вызова exec_file )
        python::object       main = python::import("__main__");
        python::object       global(main.attr("__dict__"));

        python::exec("import pip\n", global);
        python::exec("pip.logger.consumers = []\n", global);
        python::exec("pip.main(['install', '--upgrade', 'pykd'])\n", global);

    }
    catch (const python::error_already_set&)
    {
        printException(dbgOut);
    }
    catch (const std::exception& invalidArg)
    {
        _bstr_t    bstrInavalidArg(invalidArg.what());
        dbgOut.write(std::wstring(bstrInavalidArg));
    }

    PythonSingleton::get()->deactivateGlobal();

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////

std::string findScript(const std::string &fullFileName)
{
    if (GetFileAttributesA(fullFileName.c_str()) != INVALID_FILE_ATTRIBUTES)
        return fullFileName;

    python::object  sys = python::import("sys");

    python::list  pathList(sys.attr("path"));

    python::ssize_t  n = python::len(pathList);

    for (python::ssize_t i = 0; i < n; i++)
    {
        std::string  path = boost::python::extract<std::string>(pathList[i]);

        DWORD bufSize = SearchPathA(
            path.c_str(),
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
                path.c_str(),
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

void printException(DbgOut &dbgOut)
{
    // ошибка в скрипте
    PyObject  *errtype = NULL, *errvalue = NULL, *traceback = NULL;

    PyErr_Fetch(&errtype, &errvalue, &traceback);

    PyErr_NormalizeException(&errtype, &errvalue, &traceback);

    if (errtype != PyExc_SystemExit)
    {
        python::object  tracebackModule = python::import("traceback");

        std::wstringstream  sstr;

        python::object   lst =
            python::object(tracebackModule.attr("format_exception"))(
            python::handle<>(errtype),
            python::handle<>(python::allow_null(errvalue)),
            python::handle<>(python::allow_null(traceback)));

        sstr << std::endl << std::endl;

        for (long i = 0; i < python::len(lst); ++i)
            sstr << std::wstring(python::extract<std::wstring>(lst[i])) << std::endl;

        dbgOut.write(sstr.str());
    }
}

///////////////////////////////////////////////////////////////////////////////

*/
