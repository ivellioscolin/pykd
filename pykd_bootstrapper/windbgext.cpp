#include "stdafx.h"

#include <boost/python.hpp>

namespace python = boost::python;

#include "kdlib/windbg.h"

using namespace kdlib;

///////////////////////////////////////////////////////////////////////////////

class PykdBootsTrapper: public windbg::WindbgExtension
{
public:

    KDLIB_EXT_COMMAND_METHOD(py);
    KDLIB_EXT_COMMAND_METHOD(install);
    KDLIB_EXT_COMMAND_METHOD(upgrade);

    virtual void setUp();

    virtual void tearDown();

private:

    void startConsole();

    void printUsage();
    
    void printException();

    std::string getScriptFileName(const std::string &scriptName);

    std::string findScript(const std::string &fullFileName);

    PyThreadState  *m_pyState;

    bool  m_pykdInitialized;

};

///////////////////////////////////////////////////////////////////////////////

class AutoRestorePyState
{
public:

    AutoRestorePyState()
    {
        m_state = PyEval_SaveThread();
    }

    explicit AutoRestorePyState(PyThreadState **state)
    {
        *state = PyEval_SaveThread();
        m_state = *state;
    }

    ~AutoRestorePyState()
    {
        PyEval_RestoreThread(m_state);
    }

private:

    PyThreadState*    m_state;
};

///////////////////////////////////////////////////////////////////////////////

class DbgOut : public  windbg::WindbgOut
{
public:

    virtual void write(const std::wstring& str) {
        AutoRestorePyState  pystate;
        windbg::WindbgOut::write(str);
    }

    virtual void writedml(const std::wstring& str) {
        AutoRestorePyState  pystate;
        windbg::WindbgOut::writedml(str);
    }

    void flush() {
    }

    std::wstring encoding() {
        return L"ascii";
    }
};

///////////////////////////////////////////////////////////////////////////////

class DbgIn : public windbg::WindbgIn
{
public:

    std::wstring readline() {
        AutoRestorePyState  pystate;
        return kdlib::windbg::WindbgIn::readline();
    }

    std::wstring encoding() {
        return L"ascii";
    }
};

///////////////////////////////////////////////////////////////////////////////

class InterruptWatch : public windbg::InterruptWatch
{
    virtual bool onInterrupt()
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
        kdlib::eprintln(L"User Interrupt: CTRL+BREAK");
        PyErr_SetString(PyExc_SystemExit, "CTRL+BREAK");
        SetEvent(quitEvent);
        return -1;
    }
};

///////////////////////////////////////////////////////////////////////////////

KDLIB_WINDBG_EXTENSION_INIT(PykdBootsTrapper);

KDLIB_EXT_COMMAND_METHOD_IMPL(PykdBootsTrapper, py)
{
    ArgsList   args = getArgs();

    bool  global = false;
    bool  local = false;
    bool  clean = false;

    ArgsList::iterator  foundArg;

    if (!args.empty())
    {
        if (args[0] == "-h" || args[0] == "--help")
        {
            printUsage();
            return;
        }
        else
            if (args[0] == "-g" || args[0] == "--global")
            {
                global = true;
                args.erase(args.begin());
            }
            else
                if (args[0] == "-l" || args[0] == "--local")
                {
                    local = true;
                    args.erase(args.begin());
                }
    }

    PyThreadState   *localState = NULL;
    PyThreadState   *globalState = NULL;

    PyEval_RestoreThread(m_pyState);

    python::handle<>  pykdHandle(python::allow_null(PyImport_ImportModule("pykd")));
    if (!pykdHandle)
    {
        m_pyState = PyEval_SaveThread();
        kdlib::eprintln(L"Pykd package is not installed. You can install it by command \"!pykd.install\"");
        return;
    }

    std::string  scriptFileName;
    if (args.size() > 0)
    {
        scriptFileName = getScriptFileName(args[0]);

        if (scriptFileName.empty())
        {
            m_pyState = PyEval_SaveThread();
            kdlib::eprintln(L"script file not found");
            return;
        }

        global = !(global || local) ? false : global; //set local by default
    }
    else
    {
        global = !(global || local) ? true : global; //set global by default
    }

    if (!global)
    {
        globalState = PyThreadState_Swap(NULL);

        Py_NewInterpreter();

        localState = PyThreadState_Get();

        python::object       sys = python::import("sys");

        sys.attr("stdout") = python::object(::DbgOut());
        sys.attr("stderr") = python::object(::DbgOut());
        sys.attr("stdin") = python::object(::DbgIn());
    }

    if (args.size() == 0)
    {
        startConsole();
    }
    else
    {
        // устанавиливаем питоновские аргументы
        char  **pythonArgs = new char*[args.size()];

        pythonArgs[0] = const_cast<char*>(scriptFileName.c_str());

        for (size_t i = 1; i < args.size(); ++i)
            pythonArgs[i] = const_cast<char*>(args[i].c_str());

        PySys_SetArgv((int)args.size(), pythonArgs);

        delete[]  pythonArgs;

        // получаем достпу к глобальному мапу ( нужен для вызова exec_file )
        python::object  main = python::import("__main__");

        python::object  global(main.attr("__dict__"));

        try {
            InterruptWatch  interruptWatch;

            if (!m_pykdInitialized)
            {
                python::exec("__import__('pykd').initialize()", global);
                m_pykdInitialized = true;
            }

            python::exec_file(scriptFileName.c_str(), global);
        }
        catch (python::error_already_set const &)
        {
            printException();
        }
    }

    if (!global)
    {
        PyInterpreterState  *interpreter = localState->interp;

        while (interpreter->tstate_head != NULL)
        {
            PyThreadState   *threadState = (PyThreadState*)(interpreter->tstate_head);

            PyThreadState_Clear(threadState);

            PyThreadState_Swap(NULL);

            PyThreadState_Delete(threadState);
        }

        PyInterpreterState_Clear(interpreter);

        PyInterpreterState_Delete(interpreter);

        PyThreadState_Swap(globalState);
    }

    m_pyState = PyEval_SaveThread();
}

///////////////////////////////////////////////////////////////////////////////

KDLIB_EXT_COMMAND_METHOD_IMPL(PykdBootsTrapper, install)
{
    PyEval_RestoreThread(m_pyState);

    // получаем доступ к глобальному мапу ( нужен для вызова exec_file )
    python::object       main = python::import("__main__");

    python::object       global(main.attr("__dict__"));

    try {
        InterruptWatch  interruptWatch;

        python::exec("import pip\n", global);
        python::exec("pip.logger.consumers = []\n", global);
        python::exec("pip.main(['install', 'pykd'])\n", global);

    }
    catch (python::error_already_set const &)
    {
        printException();
    }

    m_pyState = PyEval_SaveThread();
}

///////////////////////////////////////////////////////////////////////////////

KDLIB_EXT_COMMAND_METHOD_IMPL(PykdBootsTrapper, upgrade)
{
    PyEval_RestoreThread(m_pyState);

    // получаем доступ к глобальному мапу ( нужен для вызова exec_file )
    python::object       main = python::import("__main__");

    python::object       global(main.attr("__dict__"));

    try {
        InterruptWatch  interruptWatch;

        python::exec("import pip\n", global);
        python::exec("pip.logger.consumers = []\n", global);
        python::exec("pip.main(['install', '--upgrade', 'pykd'])\n", global);
    }
    catch (python::error_already_set const &)
    {
        printException();
    }

    m_pyState = PyEval_SaveThread();
}

///////////////////////////////////////////////////////////////////////////////

void PykdBootsTrapper::setUp()
{
    WindbgExtension::setUp();

    PyEval_InitThreads();

    Py_Initialize();

    python::object  main = boost::python::import("__main__");

    python::object  main_namespace = main.attr("__dict__");

    // Python debug output console helper classes
    python::class_<::DbgOut>("dout", "dout", python::no_init)
        .def("write", &::DbgOut::write)
        .def("writedml", &::DbgOut::writedml)
        .def("flush", &::DbgOut::flush)
        .add_property("encoding", &::DbgOut::encoding);

    python::class_<::DbgIn>("din", "din", python::no_init)
        .def("readline", &::DbgIn::readline)
        .add_property("encoding", &::DbgIn::encoding);

    python::object       sys = python::import("sys");

    sys.attr("stdout") = python::object(::DbgOut());
    sys.attr("stderr") = python::object(::DbgOut());
    sys.attr("stdin") = python::object(::DbgIn());

    m_pyState = PyEval_SaveThread();
}

///////////////////////////////////////////////////////////////////////////////

void PykdBootsTrapper::tearDown()
{
    PyEval_RestoreThread(m_pyState);

    Py_Finalize();

    WindbgExtension::tearDown();
}

///////////////////////////////////////////////////////////////////////////////

void PykdBootsTrapper::startConsole()
{

    // получаем доступ к глобальному мапу ( нужен для вызова exec_file )
    python::object       main = python::import("__main__");

    python::object       global(main.attr("__dict__"));

    try {
        InterruptWatch  interruptWatch;

        python::exec("import pykd", global);
        python::exec("from pykd import *", global);
        if (!m_pykdInitialized)
        {
            python::exec("pykd.initialize()", global);
            m_pykdInitialized = true;
        }
        python::exec("__import__('code').InteractiveConsole(__import__('__main__').__dict__).interact()\n", global);
    }
    catch (python::error_already_set const &)
    {
        printException();
    }
}

///////////////////////////////////////////////////////////////////////////////

void PykdBootsTrapper::printUsage()
{
    kdlib::dprintln(L"usage:");
    kdlib::dprintln(L"!py [options] [file]");
    kdlib::dprintln(L"\tOptions:");
    kdlib::dprintln(L"\t-g --global  : run code in the common namespace");
    kdlib::dprintln(L"\t-l --local   : run code in the isolate namespace");
    kdlib::dprintln(L"!install");
    kdlib::dprintln(L"!upgrade");
}

//////////////////////////////////////////////////////////////////////////////

std::string PykdBootsTrapper::getScriptFileName(const std::string &scriptName)
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

std::string PykdBootsTrapper::findScript(const std::string &fullFileName)
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

            if (bufSize > 0)
            {
                return std::string(&fullFileNameCStr[0]);
            }
        }
    }

    return "";
}

///////////////////////////////////////////////////////////////////////////////


void PykdBootsTrapper::printException()
{
    // ошибка в скрипте
    PyObject  *errtype = NULL, *errvalue = NULL, *traceback = NULL;

    PyErr_Fetch(&errtype, &errvalue, &traceback);

    PyErr_NormalizeException(&errtype, &errvalue, &traceback);

    if (errtype == PyExc_SystemExit)
        return;

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

    kdlib::eprintln(sstr.str());
}

///////////////////////////////////////////////////////////////////////////////

