#pragma once


typedef void*  PyObject;
typedef void*  PyThreadState;
typedef PyObject *(*PyCFunction)(PyObject *, PyObject *);


typedef enum { PyGILState_LOCKED, PyGILState_UNLOCKED } PyGILState_STATE;

const int Py_single_input = 256;
const int Py_file_input = 257;
const int Py_eval_input = 258;

const int METH_VARARGS = 0x0001;

struct PyMethodDef {
    const char  *ml_name;   /* The name of the built-in function/method */
    PyCFunction ml_meth;    /* The C function that implements it */
    int         ml_flags;   /* Combination of METH_xxx flags, which mostly
                            describe the args expected by the C func */
    const char  *ml_doc;    /* The __doc__ attribute, or NULL */
};

typedef struct PyMethodDef PyMethodDef;

void __stdcall Py_IncRef(PyObject* object);
void __stdcall Py_DecRef(PyObject* object);

PyObject* __stdcall PyString_FromString(const char *v);
char* PyString_AsString(PyObject *string);
int PyString_Check(PyObject *o);

PyObject* PyImport_Import(PyObject *name);
PyObject* __stdcall PyImport_ImportModule(const char *name);
PyObject* __stdcall PyImport_AddModule(const char *name);

PyObject* __stdcall  PyDict_New();
int __stdcall PyDict_SetItemString(PyObject *p, const char *key, PyObject *val);

PyObject* __stdcall PyTuple_New(size_t len);
PyObject* __stdcall PyTuple_GetItem(PyObject *p, size_t pos);
int __stdcall PyTuple_SetItem(PyObject *p, size_t pos, PyObject *obj);
PyObject* __stdcall PyDict_GetItemString(PyObject *p, const char *key);
void __stdcall PyDict_Clear(PyObject *p);
size_t __stdcall PyTuple_Size(PyObject *p);

size_t PyList_Size(PyObject* list);
PyObject* PyList_GetItem(PyObject *list, size_t index);

PyObject* __stdcall PyCFunction_NewEx(PyMethodDef *, PyObject *, PyObject *);
PyObject* __stdcall PyClass_New(PyObject* className, PyObject* classBases, PyObject* classDict);
PyObject* __stdcall PyMethod_New(PyObject *func, PyObject *self, PyObject *classobj);
PyObject* __stdcall PyInstance_New(PyObject *classobj, PyObject *arg, PyObject *kw);

PyThreadState* __stdcall PyEval_SaveThread();
void __stdcall PyEval_RestoreThread(PyThreadState *tstate);

int __stdcall PySys_SetObject(char *name, PyObject *v);
PyObject* __stdcall PySys_GetObject(char *name);
void __stdcall PySys_SetArgv(int argc, char **argv);
void __stdcall PySys_SetArgv_Py3(int argc, wchar_t **argv);

int __stdcall PyRun_SimpleString(const char* str);
PyObject* __stdcall PyRun_String(const char *str, int start, PyObject *globals, PyObject *locals);
PyObject* PyRun_File(FILE *fp, const char *filename, int start, PyObject *globals, PyObject *locals);

typedef void(*PyCapsule_Destructor)(PyObject *);
PyObject* PyCapsule_New(void *pointer, const char *name, PyCapsule_Destructor destructor);
void* PyCapsule_GetPointer(PyObject *capsule, const char *name);

int PyObject_SetAttrString(PyObject *o, const char *attr_name, PyObject *v);
PyObject* PyObject_GetAttrString(PyObject *o, const char *attr_name);
PyObject* PyObject_CallObject(PyObject *callable_object, PyObject *args);
PyObject* PyObject_Call(PyObject *callable_object, PyObject *args, PyObject *kw);

PyObject* PyUnicode_FromWideChar(const wchar_t *w, size_t size);
int PyUnicode_Check(PyObject *o);

PyObject* PyBool_FromLong(long v);

PyObject* Py_None();
PyObject* PyExc_SystemExit();
PyObject* PyExc_TypeError();
PyObject* PyType_Type();
PyObject* PyProperty_Type();

void PyErr_Fetch(PyObject **ptype, PyObject **pvalue, PyObject **ptraceback);
void PyErr_NormalizeException(PyObject**exc, PyObject**val, PyObject**tb);
void PyErr_SetString(PyObject *type, const char *message);
void PyErr_Clear();

PyObject* PyFile_FromString(char *filename, char *mode);
FILE* PyFile_AsFile(PyObject *pyfile);
FILE* _Py_fopen(const char* filename, const char* mode);

PyObject* __stdcall PyUnicode_FromString(const char*  str);
PyObject* __stdcall PyInstanceMethod_New(PyObject *func);
size_t __stdcall PyUnicode_AsWideChar(PyObject *unicode, wchar_t *w, size_t size);
int __stdcall Py_AddPendingCall(int(*func)(void *), void *arg);

PyGILState_STATE __stdcall PyGILState_Ensure();
void __stdcall PyGILState_Release(PyGILState_STATE);

PyObject* __stdcall PyDescr_NewMethod(PyObject* type, struct PyMethodDef *meth);

size_t __stdcall PyGC_Collect(void);

bool IsPy3();

class  PyObjectRef;


class PyObjectBorrowedRef
{
    friend PyObjectRef;

public:

    PyObjectBorrowedRef(PyObject* obj)
    {
        m_obj = obj;
        if (m_obj)
            Py_IncRef(m_obj);
    }

    ~PyObjectBorrowedRef()
    {
        if (m_obj)
            Py_DecRef(m_obj);
    }

    operator PyObject*()
    {
        return m_obj;
    }

private:


    PyObjectBorrowedRef(const PyObjectRef& obj) = delete;

    PyObject*  m_obj;
};


class PyObjectRef
{
public:

    PyObjectRef() : m_obj(0)
    {}

    PyObjectRef(PyObject* obj)
    {
        m_obj = obj;
    }

    ~PyObjectRef()
    {
        if (m_obj)
            Py_DecRef(m_obj);
    }

    operator PyObject*()
    {
        return m_obj;
    }

    PyObjectRef& operator= (const PyObjectRef& ref)
    {
        if (m_obj)
            Py_DecRef(m_obj);

        m_obj = ref.m_obj;
        if (m_obj)
            Py_IncRef(m_obj);

        return *this;
    }

    PyObjectRef& operator= (const PyObjectBorrowedRef& ref)
    {
        if (m_obj)
            Py_DecRef(m_obj);

        m_obj = ref.m_obj;
        if (m_obj)
            Py_IncRef(m_obj);

        return *this;
    }

    PyObjectRef& operator= (PyObject* obj)
    {
        if (m_obj)
            Py_DecRef(m_obj);

        m_obj = obj;

        return *this;
    }

private:

    PyObjectRef(const PyObjectRef& obj) = delete;

    PyObject*  m_obj;
};




