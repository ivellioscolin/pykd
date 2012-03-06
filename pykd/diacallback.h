// 
// DIA load callback
// 

#pragma once

namespace pyDia {

class LoadCallback : public IDiaLoadCallback2 {
    int m_nRefCount;
public:
    LoadCallback() : m_nRefCount(1) {}

    ULONG STDMETHODCALLTYPE AddRef() {
        m_nRefCount++;
        return m_nRefCount;
    }
    ULONG STDMETHODCALLTYPE Release() {
        if ( (--m_nRefCount) == 0 )
            delete this;
        return m_nRefCount;
    }
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID rid, void **ppUnk ) {
        if ( ppUnk == NULL )
            return E_INVALIDARG;

        if (rid == __uuidof( IDiaLoadCallback2 ) )
            *ppUnk = (IDiaLoadCallback2 *)this;
        else if (rid == __uuidof( IDiaLoadCallback ) )
            *ppUnk = (IDiaLoadCallback *)this;
        else if (rid == __uuidof( IUnknown ) )
            *ppUnk = (IUnknown *)this;
        else
            *ppUnk = NULL;

        if ( *ppUnk != NULL ) {
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    HRESULT STDMETHODCALLTYPE NotifyDebugDir(
        BOOL fExecutable, 
        DWORD cbData,
        BYTE data[]
    ) 
    {
        DBG_UNREFERENCED_PARAMETER(fExecutable);
        DBG_UNREFERENCED_PARAMETER(cbData);
        DBG_UNREFERENCED_PARAMETER(data);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE NotifyOpenDBG(
        LPCOLESTR dbgPath, 
        HRESULT resultCode
    )
    {
        DBG_UNREFERENCED_PARAMETER(dbgPath);
        DBG_UNREFERENCED_PARAMETER(resultCode);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE NotifyOpenPDB(
        LPCOLESTR pdbPath, 
        HRESULT resultCode
    )
    {
        DBG_UNREFERENCED_PARAMETER(pdbPath);
        DBG_UNREFERENCED_PARAMETER(resultCode);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE RestrictRegistryAccess() {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE RestrictSymbolServerAccess() {
      return S_OK;
    }
    HRESULT STDMETHODCALLTYPE RestrictOriginalPathAccess() {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE RestrictReferencePathAccess() {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE RestrictDBGAccess() {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE RestrictSystemRootAccess() {
        return S_OK;
    }
};

}
