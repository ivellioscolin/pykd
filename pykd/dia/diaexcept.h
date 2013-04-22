
#pragma once

#include "dbgexcept.h"

namespace pykd {

//////////////////////////////////////////////////////////////////////////////

class DiaException : public SymbolException {
public:
    DiaException(const std::string &desc, HRESULT hres, IDiaSymbol *symbol = NULL)
        : SymbolException( makeFullDesc(desc, hres, symbol) )
        , m_hres(hres)
    {
    }

    DiaException(const std::string &desc)
        : SymbolException(descPrefix + desc)
        , m_hres(S_FALSE)
    {
    }

    HRESULT getRes() const {
        return m_hres;
    }
private:

    static const std::string descPrefix;

    static std::string makeFullDesc(const std::string &desc, HRESULT hres, IDiaSymbol *symbol = NULL);

    HRESULT m_hres;
};

////////////////////////////////////////////////////////////////////////////

} // end pykd namespace

