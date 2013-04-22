
#include "stdafx.h"
#include "dia\diadecls.h"
#include "win\utils.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

const std::string DiaException::descPrefix("pyDia: ");

std::string DiaException::makeFullDesc(const std::string &desc, HRESULT hres, IDiaSymbol *symbol /*= NULL*/)
{
    std::stringstream sstream;
    sstream << descPrefix << desc << " failed" << std::endl;
    if (symbol)
    {
        BSTR bstrName = NULL;
        HRESULT locRes = symbol->get_undecoratedName(&bstrName);
        if (S_OK == locRes && bstrName)
        {
            autoBstr freeBstr(bstrName);
            sstream << "Symbol name: \"" << autoBstr::asStr(bstrName) << "\"";
        }
        else
        {
            locRes = symbol->get_name(&bstrName);
            if (S_OK == locRes && bstrName)
            {
                autoBstr freeBstr(bstrName);
                sstream << "Symbol name: " << autoBstr::asStr(bstrName);
            }
            else
            {
                sstream << "Symbol: ";
            }
        }

        DWORD dwValue;
        locRes = symbol->get_relativeVirtualAddress(&dwValue);
        if (S_OK == locRes)
        {
            sstream << ", RVA= 0x" << std::hex << dwValue;
        }

        locRes = symbol->get_symTag(&dwValue);
        if (S_OK == locRes)
        {
            sstream << ", tag= " << std::dec << dwValue;
        }

        locRes = symbol->get_locationType(&dwValue);
        if (S_OK == locRes)
        {
            sstream << ", location: " << std::dec << dwValue;
        }

        sstream << std::endl;
    }
    sstream << "Return value is 0x" << std::hex << hres;

    switch (hres)
    {
#define _CASE_DIA_ERROR(x)  case E_PDB_##x: sstream << ": E_PDB_" #x << std::endl; break

    _CASE_DIA_ERROR(USAGE);
    _CASE_DIA_ERROR(OUT_OF_MEMORY);
    _CASE_DIA_ERROR(FILE_SYSTEM);
    _CASE_DIA_ERROR(NOT_FOUND);
    _CASE_DIA_ERROR(INVALID_SIG);
    _CASE_DIA_ERROR(INVALID_AGE);
    _CASE_DIA_ERROR(PRECOMP_REQUIRED);
    _CASE_DIA_ERROR(OUT_OF_TI);
    _CASE_DIA_ERROR(NOT_IMPLEMENTED);
    _CASE_DIA_ERROR(V1_PDB);
    _CASE_DIA_ERROR(FORMAT);
    _CASE_DIA_ERROR(LIMIT);
    _CASE_DIA_ERROR(CORRUPT);
    _CASE_DIA_ERROR(TI16);
    _CASE_DIA_ERROR(ACCESS_DENIED);
    _CASE_DIA_ERROR(ILLEGAL_TYPE_EDIT);
    _CASE_DIA_ERROR(INVALID_EXECUTABLE);
    _CASE_DIA_ERROR(DBG_NOT_FOUND);
    _CASE_DIA_ERROR(NO_DEBUG_INFO);
    _CASE_DIA_ERROR(INVALID_EXE_TIMESTAMP);
    _CASE_DIA_ERROR(RESERVED);
    _CASE_DIA_ERROR(DEBUG_INFO_NOT_IN_PDB);
    _CASE_DIA_ERROR(SYMSRV_BAD_CACHE_PATH);
    _CASE_DIA_ERROR(SYMSRV_CACHE_FULL);

#undef _CASE_DIA_ERROR
    default:
        {
            PCHAR errMessage = NULL;
            FormatMessageA(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                NULL,
                hres,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (PCHAR)&errMessage,
                0,
                NULL);
            if (errMessage)
            {
                sstream << ": " << std::endl;
                sstream << errMessage;
                LocalFree(errMessage);
            }
            else
            {
                sstream << std::endl;
            }
        }
    }

    return sstream.str();
}

///////////////////////////////////////////////////////////////////////////////

}; // pykd namespace end
