#include "stdafx.h"
#include "pyinterpret.h"

#include <memory>
#include <sstream>
#include <map>
#include <set>
#include <algorithm>

#include "pymodule.h"
#include "pyclass.h"
#include "dbgout.h"

class PyModule;
class PythonInterpreter;

class HKey 
{

public:

    HKey() : m_key(NULL)
    {}

    ~HKey()
    {
        if (m_key)
            RegCloseKey(m_key);
    }

    operator HKEY*()
    {
        return &m_key;
    }

    operator HKEY() const
    {
        return m_key;
    }

private:

    HKEY  m_key;
};


class PyModule
{
public:

    PyModule(int majorVesion, int minorVersion);

    ~PyModule();

    bool isPy3;

    void checkPykd();
    void deactivate();

    PyObject* PyType_Type;
    PyObject* PyProperty_Type;
    PyObject* Py_None;
    PyObject* PyExc_SystemExit;

    void( *Py_Initialize)();
    void( *Py_Finalize)();

    PyThreadState* ( *Py_NewInterpreter)();
    void( *Py_EndInterpreter)(PyThreadState *tstate);
    PyObject* ( *PyEval_GetGlobals)();
    PyObject* ( *PyImport_Import)(PyObject *name);
    PyObject* ( *PyImport_ImportModule)(const char *name);
    void( *PyEval_InitThreads)();
    PyThreadState* ( *PyEval_SaveThread)();
    void( *PyEval_RestoreThread)(PyThreadState *tstate);
    PyThreadState* ( *PyThreadState_Swap)(PyThreadState *tstate);
    PyObject* ( *PyRun_String)(const char *str, int start, PyObject *globals, PyObject *locals);
    int( *PyRun_SimpleString)(const char*  str);
    PyObject* ( *PyRun_File)(FILE *fp, const char *filename, int start, PyObject *globals, PyObject *locals);
    PyObject* ( *PyDict_New)();
    int( *PyDict_SetItemString)(PyObject *p, const char *key, PyObject *val);
    PyObject*( *PyDict_GetItemString)(PyObject *p, const char* key);
    void( *Py_IncRef)(PyObject* object);
    void( *Py_DecRef)(PyObject* object);
    PyObject* ( *PyObject_Call)(PyObject *callable_object, PyObject *args, PyObject *kw);
    PyObject* ( *PyObject_GetAttr)(PyObject *object, PyObject *attr_name);
    PyObject* ( *PyObject_GetAttrString)(PyObject *object, const char *attr_name);
    int( *PyObject_SetAttr)(PyObject *object, PyObject *attr_name, PyObject *value);
    PyObject* ( *PyObject_CallObject)(PyObject *callable_object, PyObject *args);
    PyObject* ( *PyTuple_New)(size_t len);
    int( *PyTuple_SetItem)(PyObject *p, size_t pos, PyObject *o);
    PyObject* ( *PyTuple_GetItem)(PyObject *p, size_t pos);
    size_t( *PyTuple_Size)(PyObject *p);
    PyObject* ( *PyCFunction_NewEx)(PyMethodDef *, PyObject *, PyObject *);
    PyObject* ( *PySys_GetObject)(char *name);
    int( *PySys_SetObject)(char *name, PyObject *v);
    void( *PySys_SetArgv)(int argc, char **argv);
    void( *PySys_SetArgv_Py3)(int argc, wchar_t **argv);
    PyObject* ( *PyString_FromString)(const char *v);
    char* ( *PyString_AsString)(PyObject *string);
    void( *PyErr_Fetch)(PyObject **ptype, PyObject **pvalue, PyObject **ptraceback);
    void( *PyErr_NormalizeException)(PyObject**exc, PyObject**val, PyObject**tb);
    void( *PyErr_SetString)(PyObject *type, const char *message);
    void( *PyErr_Clear)();
    PyObject* ( *PyImport_AddModule)(const char *name);
    PyObject* ( *PyClass_New)(PyObject* className, PyObject* classBases, PyObject* classDict);
    PyObject* ( *PyInstance_New)(PyObject *classobj, PyObject *arg, PyObject *kw);
    PyObject* ( *PyMethod_New)(PyObject *func, PyObject *self, PyObject *classobj);
    PyObject* ( *PyCapsule_New)(void *pointer, const char *name, PyCapsule_Destructor destructor);
    void* ( *PyCapsule_GetPointer)(PyObject *capsule, const char *name);
    int( *PyObject_SetAttrString)(PyObject *o, const char *attr_name, PyObject *v);
    PyObject* ( *PyUnicode_FromWideChar)(const wchar_t *w, size_t size);
    PyObject* ( *PyBool_FromLong)(long v);
    size_t( *PyList_Size)(PyObject* list);
    PyObject* ( *PyList_GetItem)(PyObject *list, size_t index);
    PyObject* ( *PyFile_FromString)(char *filename, char *mode);
    FILE* ( *PyFile_AsFile)(PyObject *pyfile);
    PyObject* ( *PyUnicode_FromString)(const char *u);
    PyObject* ( *PyInstanceMethod_New)(PyObject *func);
    size_t( *PyUnicode_AsWideChar)(PyObject *unicode, wchar_t *w, size_t size);
    FILE* (  *_Py_fopen)(const char* filename, const char* mode);
    int( *Py_AddPendingCall)(int(*func)(void *), void *arg);
    PyGILState_STATE( *PyGILState_Ensure)();
    void( *PyGILState_Release)(PyGILState_STATE state);
    PyObject* ( *PyDescr_NewMethod)(PyObject* type, struct PyMethodDef *meth);

    HMODULE  m_handlePython;
    PyThreadState*  m_globalState;
    PythonInterpreter*  m_globalInterpreter;
    bool m_pykdInit;
};



class PythonInterpreter
{
public:

    PythonInterpreter(PyModule* mod) :
        m_module(mod)
    {
        PyThreadState*  state = mod->Py_NewInterpreter();

        m_module->PyThreadState_Swap(state);

        m_state = m_module->PyEval_SaveThread();
    }

    ~PythonInterpreter()
    {
        m_module->PyEval_RestoreThread(m_state);

        m_module->Py_EndInterpreter(m_state);
    }

    PyModule*  m_module;

    PyThreadState*  m_state;
};


class PythonSingleton
{
public:


    static  PythonSingleton* get()
    {
        if (m_singleton.get() == 0)
            m_singleton.reset(new PythonSingleton());
        return m_singleton.get();
    }


    PythonInterpreter* currentInterpreter()
    {
        return m_currentInterpter;
    }

    PythonInterpreter* getInterpreter(int majorVersion, int minorVersion, bool global)
    {
        PyModule*  module = 0;

        if (m_modules.find(std::make_pair(majorVersion, minorVersion)) == m_modules.end())
        {
            module = new PyModule(majorVersion, minorVersion);
            m_modules.insert(std::make_pair(std::make_pair(majorVersion, minorVersion), module));
        }
        else
        {
            module = m_modules[std::make_pair(majorVersion, minorVersion)];
        }

        if (global)
        {
            if (module->m_globalInterpreter == 0)
            {
                module->PyEval_RestoreThread(module->m_globalState);
                module->m_globalInterpreter = new PythonInterpreter(module);
            }

            m_currentInterpter = module->m_globalInterpreter;
            m_currentIsGlobal = true;
        }
        else
        {
            module->PyEval_RestoreThread(module->m_globalState);
            m_currentInterpter = new PythonInterpreter(module);
            m_currentIsGlobal = false;
        }

        m_currentInterpter->m_module->PyEval_RestoreThread(m_currentInterpter->m_state);

        return m_currentInterpter;
    }

    void releaseInterpretor(PythonInterpreter* interpret)
    {
        PyModule*  module = m_currentInterpter->m_module;

        m_currentInterpter->m_state = module->PyEval_SaveThread();

        if (!m_currentIsGlobal)
        {
            delete m_currentInterpter;

            module->PyThreadState_Swap(module->m_globalState);
            module->m_globalState = module->PyEval_SaveThread();
        }

        m_currentInterpter = 0;
    }

    bool isInterpreterLoaded(int majorVersion, int minorVersion)
    {
        return m_modules.find(std::make_pair(majorVersion, minorVersion)) != m_modules.end();
    }

    void checkPykd()
    {
        m_currentInterpter->m_module->checkPykd();
    }

    void stopAllInterpreter()
    {
        for (auto m : m_modules)
        {
            m_currentInterpter = m.second->m_globalInterpreter;
            m.second->deactivate();
        }
        m_currentInterpter = 0;
    }

private:

    static std::auto_ptr<PythonSingleton>  m_singleton;

    std::map<std::pair<int,int>, PyModule*>  m_modules;
   
    PythonInterpreter*  m_currentInterpter;
    bool  m_currentIsGlobal;
};

std::auto_ptr<PythonSingleton>  PythonSingleton::m_singleton; 

HMODULE LoadPythonForKey(HKEY installPathKey, int majorVersion, int minorVersion)
{

    HMODULE  hmodule = NULL;

    char  installPath[1000];
    DWORD  installPathSize = sizeof(installPath);

    if (ERROR_SUCCESS == RegQueryValueExA(installPathKey, NULL, NULL, NULL, (LPBYTE)installPath, &installPathSize))
    {
        std::stringstream  dllName;
        dllName << "python" << majorVersion << minorVersion << ".dll";

        std::stringstream  imagePath;
        imagePath << installPath << dllName.str();

        hmodule = LoadLibraryExA(imagePath.str().c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
        if (hmodule)
            return hmodule;

        hmodule = LoadLibraryA(dllName.str().c_str());
        if (hmodule)
            return hmodule;

    }

    return NULL;
}


HMODULE LoadPythonLibrary(int majorVersion, int minorVersion)
{

    HKey  pythonCoreKey;

    for (auto rootKey : std::list<HKEY>({ HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER }))
    {
        if (ERROR_SUCCESS == RegOpenKeyA(rootKey, "SOFTWARE\\Python\\PythonCore", pythonCoreKey))
        {
            HKey  installPathKey;

            std::stringstream   installPathStr;

            if (majorVersion == 2 || (majorVersion == 3 && minorVersion <= 4))
            {
                installPathStr << majorVersion << '.' << minorVersion << "\\InstallPath";
            }
            else
            if (majorVersion == 3 && minorVersion >= 5)
            {
#ifdef _M_X64
                installPathStr << majorVersion << '.' << minorVersion << "\\InstallPath";
#else
                installPathStr << majorVersion << '.' << minorVersion << "-32" << "\\InstallPath";
#endif
            }

            if (ERROR_SUCCESS == RegOpenKeyA(pythonCoreKey, installPathStr.str().c_str(), installPathKey))
            {
                HMODULE  hmodule = LoadPythonForKey(installPathKey, majorVersion, minorVersion);
                if (hmodule)
                    return hmodule;
            }
        }
    }

    return NULL;
}




std::list<InterpreterDesc>  getInstalledInterpreter()
{
    std::set<InterpreterDesc>  interpretSet;

    for (auto rootKey : const std::list<HKEY>({ HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER }))
    {
        HKey  pythonCoreKey;
        
        if (ERROR_SUCCESS != RegOpenKeyA(rootKey, "SOFTWARE\\Python\\PythonCore", pythonCoreKey))
            continue;

        for (DWORD i = 0;; ++i)
        {
            char  versionStr[20];
            if (ERROR_SUCCESS != RegEnumKeyA(pythonCoreKey, i, versionStr, sizeof(versionStr)))
                break;

            int  majorVersion = -1, minorVersion = -1;
            sscanf_s(versionStr, "%d.%d", &majorVersion, &minorVersion);

            HKey  installPathKey;
            std::string   installPathStr(versionStr);
            installPathStr += "\\InstallPath";

            if (ERROR_SUCCESS != RegOpenKeyA(pythonCoreKey, installPathStr.c_str(), installPathKey))
                continue;

            HMODULE  hmodule = LoadPythonForKey(installPathKey, majorVersion, minorVersion);
            
            if (hmodule)
            {
                char  fullPath[1000];
            
                if (GetModuleFileNameA(hmodule, fullPath, sizeof(fullPath)))
                {
                    interpretSet.insert({ majorVersion, minorVersion, fullPath });
                }
            
                FreeLibrary(hmodule);
            }
        }
    }

    std::list<InterpreterDesc>  interpretLst;

    std::copy(interpretSet.begin(), interpretSet.end(), std::inserter(interpretLst, interpretLst.begin()));

    interpretLst.sort();

    return interpretLst;
}

PyModule::PyModule(int majorVesion, int minorVersion)
{
    m_handlePython = LoadPythonLibrary(majorVesion, minorVersion);

    if (!m_handlePython)
        throw std::exception("failed to load python module");

    isPy3 = majorVesion == 3;

    *reinterpret_cast<FARPROC*>(&PyType_Type) = GetProcAddress(m_handlePython, "PyType_Type");
    *reinterpret_cast<FARPROC*>(&PyProperty_Type) = GetProcAddress(m_handlePython, "PyProperty_Type");
    *reinterpret_cast<FARPROC*>(&Py_None) = GetProcAddress(m_handlePython, "_Py_NoneStruct");
    PyExc_SystemExit = *reinterpret_cast<PyObject**>(GetProcAddress(m_handlePython, "PyExc_SystemExit"));

    *reinterpret_cast<FARPROC*>(&Py_Initialize) = GetProcAddress(m_handlePython, "Py_Initialize");
    *reinterpret_cast<FARPROC*>(&Py_Finalize) = GetProcAddress(m_handlePython, "Py_Finalize");
    *reinterpret_cast<FARPROC*>(&Py_NewInterpreter) = GetProcAddress(m_handlePython, "Py_NewInterpreter");
    *reinterpret_cast<FARPROC*>(&Py_EndInterpreter) = GetProcAddress(m_handlePython, "Py_EndInterpreter");
    *reinterpret_cast<FARPROC*>(&Py_DecRef) = GetProcAddress(m_handlePython, "Py_DecRef");
    *reinterpret_cast<FARPROC*>(&Py_IncRef) = GetProcAddress(m_handlePython, "Py_IncRef");
    *reinterpret_cast<FARPROC*>(&PyEval_GetGlobals) = GetProcAddress(m_handlePython, "PyEval_GetGlobals");
    *reinterpret_cast<FARPROC*>(&PyEval_InitThreads) = GetProcAddress(m_handlePython, "PyEval_InitThreads");
    *reinterpret_cast<FARPROC*>(&PyEval_SaveThread) = GetProcAddress(m_handlePython, "PyEval_SaveThread");
    *reinterpret_cast<FARPROC*>(&PyEval_RestoreThread) = GetProcAddress(m_handlePython, "PyEval_RestoreThread");
    *reinterpret_cast<FARPROC*>(&PyThreadState_Swap) = GetProcAddress(m_handlePython, "PyThreadState_Swap");
    *reinterpret_cast<FARPROC*>(&PyRun_String) = GetProcAddress(m_handlePython, "PyRun_String");
    *reinterpret_cast<FARPROC*>(&PyRun_SimpleString) = GetProcAddress(m_handlePython, "PyRun_SimpleString");
    *reinterpret_cast<FARPROC*>(&PyRun_File) = GetProcAddress(m_handlePython, "PyRun_File");
    *reinterpret_cast<FARPROC*>(&PyDict_New) = GetProcAddress(m_handlePython, "PyDict_New");
    *reinterpret_cast<FARPROC*>(&PyDict_SetItemString) = GetProcAddress(m_handlePython, "PyDict_SetItemString");
    *reinterpret_cast<FARPROC*>(&PyDict_GetItemString) = GetProcAddress(m_handlePython, "PyDict_GetItemString");
    *reinterpret_cast<FARPROC*>(&PyObject_Call) = GetProcAddress(m_handlePython, "PyObject_Call");
    *reinterpret_cast<FARPROC*>(&PyObject_GetAttr) = GetProcAddress(m_handlePython, "PyObject_GetAttr");
    *reinterpret_cast<FARPROC*>(&PyObject_GetAttrString) = GetProcAddress(m_handlePython, "PyObject_GetAttrString");
    *reinterpret_cast<FARPROC*>(&PyObject_SetAttr) = GetProcAddress(m_handlePython, "PyObject_SetAttr");
    *reinterpret_cast<FARPROC*>(&PyObject_CallObject) = GetProcAddress(m_handlePython, "PyObject_CallObject");
    *reinterpret_cast<FARPROC*>(&PyTuple_New) = GetProcAddress(m_handlePython, "PyTuple_New");
    *reinterpret_cast<FARPROC*>(&PyTuple_SetItem) = GetProcAddress(m_handlePython, "PyTuple_SetItem");
    *reinterpret_cast<FARPROC*>(&PyTuple_GetItem) = GetProcAddress(m_handlePython, "PyTuple_GetItem");
    *reinterpret_cast<FARPROC*>(&PyTuple_Size) = GetProcAddress(m_handlePython, "PyTuple_Size");
    *reinterpret_cast<FARPROC*>(&PyString_FromString) = GetProcAddress(m_handlePython, "PyString_FromString");
    *reinterpret_cast<FARPROC*>(&PyCFunction_NewEx) = GetProcAddress(m_handlePython, "PyCFunction_NewEx");
    *reinterpret_cast<FARPROC*>(&PySys_GetObject) = GetProcAddress(m_handlePython, "PySys_GetObject");
    *reinterpret_cast<FARPROC*>(&PySys_SetObject) = GetProcAddress(m_handlePython, "PySys_SetObject");
    *reinterpret_cast<FARPROC*>(&PySys_SetArgv) = !isPy3 ? GetProcAddress(m_handlePython, "PySys_SetArgv") : 0 ;
    *reinterpret_cast<FARPROC*>(&PySys_SetArgv_Py3) = isPy3 ? GetProcAddress(m_handlePython, "PySys_SetArgv") : 0;
    *reinterpret_cast<FARPROC*>(&PyString_FromString) = GetProcAddress(m_handlePython, "PyString_FromString");
    *reinterpret_cast<FARPROC*>(&PyString_AsString) = GetProcAddress(m_handlePython, "PyString_AsString");
    *reinterpret_cast<FARPROC*>(&PyErr_Fetch) = GetProcAddress(m_handlePython, "PyErr_Fetch");
    *reinterpret_cast<FARPROC*>(&PyErr_NormalizeException) = GetProcAddress(m_handlePython, "PyErr_NormalizeException");
    *reinterpret_cast<FARPROC*>(&PyErr_SetString) = GetProcAddress(m_handlePython, "PyErr_SetString");
    *reinterpret_cast<FARPROC*>(&PyErr_Clear) = GetProcAddress(m_handlePython, "PyErr_Clear");
    *reinterpret_cast<FARPROC*>(&PyImport_AddModule) = GetProcAddress(m_handlePython, "PyImport_AddModule");
    *reinterpret_cast<FARPROC*>(&PyImport_ImportModule) = GetProcAddress(m_handlePython, "PyImport_ImportModule");    
    *reinterpret_cast<FARPROC*>(&PyClass_New) = GetProcAddress(m_handlePython, "PyClass_New");
    *reinterpret_cast<FARPROC*>(&PyInstance_New) = GetProcAddress(m_handlePython, "PyInstance_New");
    *reinterpret_cast<FARPROC*>(&PyMethod_New) = GetProcAddress(m_handlePython, "PyMethod_New");
    *reinterpret_cast<FARPROC*>(&PyCapsule_New) = GetProcAddress(m_handlePython, "PyCapsule_New");
    *reinterpret_cast<FARPROC*>(&PyCapsule_GetPointer) = GetProcAddress(m_handlePython, "PyCapsule_GetPointer");
    *reinterpret_cast<FARPROC*>(&PyObject_SetAttrString) = GetProcAddress(m_handlePython, "PyObject_SetAttrString");
    *reinterpret_cast<FARPROC*>(&PyUnicode_FromWideChar) = isPy3 ? GetProcAddress(m_handlePython, "PyUnicode_FromWideChar") :
         GetProcAddress(m_handlePython, "PyUnicodeUCS2_FromWideChar");
    *reinterpret_cast<FARPROC*>(&PyImport_Import) = GetProcAddress(m_handlePython, "PyImport_Import");
    *reinterpret_cast<FARPROC*>(&PyBool_FromLong) = GetProcAddress(m_handlePython, "PyBool_FromLong");
    *reinterpret_cast<FARPROC*>(&PyList_Size) = GetProcAddress(m_handlePython, "PyList_Size");
    *reinterpret_cast<FARPROC*>(&PyList_GetItem) = GetProcAddress(m_handlePython, "PyList_GetItem");
    *reinterpret_cast<FARPROC*>(&PyFile_FromString) = GetProcAddress(m_handlePython, "PyFile_FromString");
    *reinterpret_cast<FARPROC*>(&PyFile_AsFile) = GetProcAddress(m_handlePython, "PyFile_AsFile");
    *reinterpret_cast<FARPROC*>(&PyUnicode_FromString) = GetProcAddress(m_handlePython, "PyUnicode_FromString");
    *reinterpret_cast<FARPROC*>(&PyInstanceMethod_New) = GetProcAddress(m_handlePython, "PyInstanceMethod_New");
    *reinterpret_cast<FARPROC*>(&PyUnicode_AsWideChar) = GetProcAddress(m_handlePython, "PyUnicode_AsWideChar");
    *reinterpret_cast<FARPROC*>(&_Py_fopen) = GetProcAddress(m_handlePython, "_Py_fopen");
    *reinterpret_cast<FARPROC*>(&Py_AddPendingCall) = GetProcAddress(m_handlePython, "Py_AddPendingCall");
    *reinterpret_cast<FARPROC*>(&PyGILState_Ensure) = GetProcAddress(m_handlePython, "PyGILState_Ensure");
    *reinterpret_cast<FARPROC*>(&PyGILState_Release) = GetProcAddress(m_handlePython, "PyGILState_Release");
    *reinterpret_cast<FARPROC*>(&PyDescr_NewMethod) = GetProcAddress(m_handlePython, "PyDescr_NewMethod");
    
    Py_Initialize();
    PyEval_InitThreads();

    m_globalState = PyEval_SaveThread();
}


PyModule::~PyModule()
{
    assert(0);

    //if (m_globalInterpreter)
    //{
    //    delete m_globalInterpreter;
    //    m_globalInterpreter = 0;
    //}

    //PyThreadState_Swap(m_globalState);

    //Py_Finalize();

    //FreeLibrary(m_handlePython);
}


void PyModule::deactivate()
{
    if (m_globalInterpreter)
    {
        delete m_globalInterpreter;
        m_globalInterpreter = 0;
    }

    PyThreadState_Swap(m_globalState);

    if (m_pykdInit)
    {
        PyObject* mainName = isPy3 ? PyUnicode_FromString("__main__") : PyString_FromString("__main__");
        PyObject*  mainMod = PyImport_Import(mainName);
        PyObject*  globals = PyObject_GetAttrString(mainMod, "__dict__");
        PyObject*  result = PyRun_String("__import__('pykd').deinitialize()\n", Py_file_input, globals, globals);

        if (mainName) Py_DecRef(mainName);
        if (mainMod) Py_DecRef(mainMod);
        if (globals) Py_DecRef(globals);
        if (result) Py_DecRef(result);
    }

    m_globalState = PyEval_SaveThread();
}

void PyModule::checkPykd()
{
    if (m_pykdInit)
        return;

    PyObject*  pykdMod = PyImport_ImportModule("pykd");

    if (!pykdMod)
    {
        PyObject*  mainName = isPy3 ? PyUnicode_FromString("__main__") : PyString_FromString("__main__");
        PyObject*  mainMod = PyImport_Import(mainName);
        PyObject*  globals = PyObject_GetAttrString(mainMod, "__dict__");
        PyObject*  result = PyRun_String("__import__('pykd').initialize()\n", Py_file_input, globals, globals);
        if (mainName) Py_DecRef(mainName);
        if (mainMod) Py_DecRef(mainMod);
        if (globals) Py_DecRef(globals);
        if (result) Py_DecRef(result);
    }

    if (pykdMod) Py_DecRef(pykdMod);

    m_pykdInit = true;
}



PythonInterpreter* activateInterpreter(bool global, int majorVersion, int minorVersion)
{
    return PythonSingleton::get()->getInterpreter(majorVersion, minorVersion, global);
}

void releaseInterpretor(PythonInterpreter* interpret)
{
    PythonSingleton::get()->releaseInterpretor(interpret);
}

bool isInterpreterLoaded(int majorVersion, int minorVersion)
{
    return PythonSingleton::get()->isInterpreterLoaded(majorVersion, minorVersion);
}

void stopAllInterpreter()
{
    PythonSingleton::get()->stopAllInterpreter();
}

void checkPykd()
{
    PythonSingleton::get()->checkPykd();
}

void __stdcall Py_IncRef(PyObject* object)
{
    PythonSingleton::get()->currentInterpreter()->m_module->Py_IncRef(object);
}

void __stdcall Py_DecRef(PyObject* object)
{
    PythonSingleton::get()->currentInterpreter()->m_module->Py_DecRef(object);
}

PyObject* __stdcall PyString_FromString(const char *v)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyString_FromString(v);
}

PyObject* __stdcall  PyDict_New()
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyDict_New();
}

PyObject* __stdcall PyDict_GetItemString(PyObject *p, const char *key)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyDict_GetItemString(p, key);
}

int __stdcall PyDict_SetItemString(PyObject *p, const char *key, PyObject *val)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyDict_SetItemString(p, key, val);
}

PyObject* __stdcall PyCFunction_NewEx(PyMethodDef* pydef, PyObject *p1, PyObject *p2)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyCFunction_NewEx(pydef, p1, p2);
}

PyObject* __stdcall PyClass_New(PyObject* className, PyObject* classBases, PyObject* classDict)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyClass_New(className, classBases, classDict);
}

PyObject* __stdcall PyMethod_New(PyObject *func, PyObject *self, PyObject *classobj)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyMethod_New(func, self, classobj);
}

int __stdcall PySys_SetObject(char *name, PyObject *v)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PySys_SetObject(name, v);
}

void __stdcall PySys_SetArgv(int argc, char **argv)
{
    PythonSingleton::get()->currentInterpreter()->m_module->PySys_SetArgv(argc, argv);
}

void __stdcall PySys_SetArgv_Py3(int argc, wchar_t **argv)
{
    PythonSingleton::get()->currentInterpreter()->m_module->PySys_SetArgv_Py3(argc, argv);
}

PyObject* __stdcall PySys_GetObject(char *name)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PySys_GetObject(name);
}

PyObject* __stdcall PyInstance_New(PyObject *classobj, PyObject *arg, PyObject *kw)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyInstance_New(classobj, arg, kw);
}

int __stdcall PyRun_SimpleString(const char* str)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyRun_SimpleString(str);
}

PyObject* __stdcall PyRun_String(const char *str, int start, PyObject *globals, PyObject *locals)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyRun_String(str, start, globals, locals);
}

PyObject* PyCapsule_New(void *pointer, const char *name, PyCapsule_Destructor destructor)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyCapsule_New(pointer, name, destructor);
}

void* PyCapsule_GetPointer(PyObject *capsule, const char *name)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyCapsule_GetPointer(capsule, name);
}

int PyObject_SetAttrString(PyObject *o, const char *attr_name, PyObject *v)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyObject_SetAttrString(o, attr_name, v);
}

PyObject* PyObject_GetAttrString(PyObject *o, const char *attr_name)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyObject_GetAttrString(o, attr_name);
}

PyObject* PyObject_CallObject(PyObject *callable_object, PyObject *args)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyObject_CallObject(callable_object, args);
}

PyObject* PyObject_Call(PyObject *callable_object, PyObject *args, PyObject *kw)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyObject_Call(callable_object, args, kw);
}

PyObject* __stdcall PyTuple_New(size_t len)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyTuple_New(len);
}

PyObject* __stdcall PyTuple_GetItem(PyObject *p, size_t pos)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyTuple_GetItem(p, pos);
}

int __stdcall PyTuple_SetItem(PyObject *p, size_t pos, PyObject *obj)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyTuple_SetItem(p, pos, obj);
}

size_t __stdcall PyTuple_Size(PyObject *p)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyTuple_Size(p);
}

char* PyString_AsString(PyObject *string)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyString_AsString(string);
}

PyObject* PyUnicode_FromWideChar(const wchar_t *w, size_t size)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyUnicode_FromWideChar(w, size);
}

PyObject* PyImport_Import(PyObject *name)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyImport_Import(name);
}

PyObject* PyBool_FromLong(long v)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyBool_FromLong(v);
}

PyObject* Py_None()
{
    return PythonSingleton::get()->currentInterpreter()->m_module->Py_None;
}

PyObject* PyExc_SystemExit()
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyExc_SystemExit;
}

PyObject* PyType_Type()
{
    return  PythonSingleton::get()->currentInterpreter()->m_module->PyType_Type;
}

PyObject* PyProperty_Type()
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyProperty_Type;
}

void PyErr_Fetch(PyObject **ptype, PyObject **pvalue, PyObject **ptraceback)
{
    PythonSingleton::get()->currentInterpreter()->m_module->PyErr_Fetch(ptype, pvalue, ptraceback);
}


void PyErr_NormalizeException(PyObject**exc, PyObject**val, PyObject**tb)
{
    PythonSingleton::get()->currentInterpreter()->m_module->PyErr_NormalizeException(exc, val, tb);
}

void PyErr_Clear()
{
    PythonSingleton::get()->currentInterpreter()->m_module->PyErr_Clear();
}

void PyErr_SetString(PyObject *type, const char *message)
{
    PythonSingleton::get()->currentInterpreter()->m_module->PyErr_SetString(type, message);
}

size_t PyList_Size(PyObject* list)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyList_Size(list);
}

PyObject* PyList_GetItem(PyObject *list, size_t index)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyList_GetItem(list, index);
}

PyObject* PyFile_FromString(char *filename, char *mode)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyFile_FromString(filename, mode);
}

FILE* PyFile_AsFile(PyObject *pyfile)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyFile_AsFile(pyfile);
}

PyObject* PyRun_File(FILE *fp, const char *filename, int start, PyObject *globals, PyObject *locals)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyRun_File(fp, filename, start, globals, locals);
}

PyObject* __stdcall PyUnicode_FromString(const char*  str)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyUnicode_FromString(str);
}

PyObject* __stdcall PyInstanceMethod_New(PyObject *func)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyInstanceMethod_New(func);
}

size_t __stdcall PyUnicode_AsWideChar(PyObject *unicode, wchar_t *w, size_t size)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyUnicode_AsWideChar(unicode, w, size);
}

PyObject* __stdcall PyImport_ImportModule(const char *name)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyImport_ImportModule(name);
}

PyThreadState* __stdcall PyEval_SaveThread()
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyEval_SaveThread();
}

void __stdcall PyEval_RestoreThread(PyThreadState *tstate)
{
    PythonSingleton::get()->currentInterpreter()->m_module->PyEval_RestoreThread(tstate);
}

FILE* _Py_fopen(const char* filename, const char* mode)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->_Py_fopen(filename, mode);
}

int __stdcall Py_AddPendingCall(int(*func)(void *), void *arg)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->Py_AddPendingCall(func, arg);
}

PyGILState_STATE __stdcall PyGILState_Ensure()
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyGILState_Ensure();
}

void __stdcall PyGILState_Release(PyGILState_STATE state)
{
    PythonSingleton::get()->currentInterpreter()->m_module->PyGILState_Release(state);
}

PyObject* __stdcall PyDescr_NewMethod(PyObject* type, struct PyMethodDef *meth)
{
    return PythonSingleton::get()->currentInterpreter()->m_module->PyDescr_NewMethod(type, meth);
}



bool IsPy3()
{
    return PythonSingleton::get()->currentInterpreter()->m_module->isPy3;
}


