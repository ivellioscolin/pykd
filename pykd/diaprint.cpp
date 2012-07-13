#include "stdafx.h"

#include "diawrapper.h"
#include "utils.h"

namespace pyDia {

static void printVariant(
    __in const VARIANT &vtValue, 
    __out std::stringstream &sstream
)
{
    switch (vtValue.vt)
    {
    case VT_I1:
    case VT_UI1:
        sstream << ", Value: ";
        sstream << "0x" << std::hex << vtValue.bVal;
        break;

    case VT_BOOL:
        sstream << ", Value: ";
        sstream << vtValue.iVal ? "True" : "False";
        break;

    case VT_I2:
    case VT_UI2:
        sstream << ", Value: ";
        sstream << "0x" << std::hex << vtValue.iVal;
        break;

    case VT_I4:
    case VT_UI4:
    case VT_INT:
    case VT_UINT:
    case VT_ERROR:
    case VT_HRESULT:
        sstream << ", Value: ";
        sstream << "0x" << std::hex << vtValue.lVal;
        break;

    case VT_I8:
    case VT_UI8:
        sstream << ", Value: ";
        sstream << "0x" << std::hex << vtValue.llVal;
        break;

    case VT_R4:
        sstream << ", Value: ";
        sstream << vtValue.fltVal;
        break;

    case VT_R8:
        sstream << ", Value: ";
        sstream << vtValue.dblVal;
        break;

    case VT_BSTR:
        sstream << ", Value: ";
        sstream << "\"" << autoBstr::asStr(vtValue.bstrVal).c_str() << "\"";
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////

static void printSignedOffset(
    std::stringstream &sstream,
    LONG lValue
)
{
    const bool bNegOffset = lValue < 0;
    lValue = bNegOffset ? -1 * lValue : lValue;
    sstream << (bNegOffset ? "-" : "+") << "0x" << std::hex << lValue;
}

////////////////////////////////////////////////////////////////////////////////

std::string Symbol::printImpl(
    IDiaSymbol *_symbol,
    DWORD machineType,
    ULONG indent /*= 0*/,
    Symbol::checkSymLoop *checkLoopPrev /*= NULL*/,
    const char *prefix /*= NULL*/
)
{
    assert(_symbol);

    std::stringstream sstream;
    for (ULONG i =0; i < indent; ++i)
        sstream << " ";
    if (prefix)
        sstream << prefix;

    DWORD dwValue;
    autoBstr bstrValue;
    VARIANT vtValue = { VT_EMPTY };
    BOOL bValue;
    LONG lValue;
    ULONGLONG ullValue;
    HRESULT hres;

    hres = _symbol->get_symIndexId(&dwValue);
    if (hres == S_OK)
        sstream << "ID " << std::hex << dwValue << " ";

    checkSymLoop _loop(checkLoopPrev);
    if (_loop.check(_symbol))
    {
        sstream << "<...already printed...>";
        return sstream.str();
    }

    DWORD locType = LocIsNull;
    hres = _symbol->get_locationType(&locType);
    bool bLocation = (S_OK == hres);
    if (bLocation)
    {
        hres = _symbol->get_offset(&lValue);

        switch (locType)
        {
        case LocIsBitField:
        case LocIsThisRel:
            assert(S_OK == hres);
            printSignedOffset(sstream, lValue);
            if (LocIsBitField == locType)
            {
                hres = _symbol->get_bitPosition(&dwValue);
                if (S_OK == hres)
                    sstream << ", Bit position: " << dwValue;
            }
            break;

        case LocIsEnregistered:
        case LocIsRegRel:
            hres = _symbol->get_registerId(&dwValue);
            if (S_OK == hres)
            {
                const char *regName = NULL;
                if (IMAGE_FILE_MACHINE_I386 == machineType)
                {
                    for (ULONG i = 0; i < cntI386RegName; ++i)
                    {
                        if (dwValue == i386RegName[i].first)
                        {
                            regName = i386RegName[i].second;
                            break;
                        }
                    }
                }
                else if (IMAGE_FILE_MACHINE_AMD64 == machineType)
                {
                    for (ULONG i = 0; i < cntI386RegName; ++i)
                    {
                        if (dwValue == i386RegName[i].first)
                        {
                            regName = i386RegName[i].second;
                            break;
                        }
                    }
                }
                if (!regName)
                {
                    sstream << locTypeName[locType].second;
                }
                else
                {
                    if (LocIsEnregistered == locType)
                    {
                        sstream << regName;
                    }
                    else
                    {
                        sstream << "[" << regName;
                        printSignedOffset(sstream, lValue);
                        sstream << "]";
                    }
                }
            }
            else
            {
                sstream << locTypeName[locType].second;
            }
            break;

        default:
            if (S_OK == _symbol->get_relativeVirtualAddress(&dwValue))
                sstream << "RVA:0x" << std::hex << dwValue;
            else if (locType < _countof(locTypeName))
                sstream << "Location: " << locTypeName[locType].second;
            if (S_OK == hres)
            {
                sstream << ", Offset: ";
                printSignedOffset(sstream, lValue);
            }
            break;
        }
    }

    bool bFuncDebugRange = false;

    hres = _symbol->get_symTag(&dwValue);
    if ((S_OK == hres) && dwValue < _countof(symTagName))
    {
        if (bLocation)
            sstream << ", ";

        sstream << symTagName[dwValue].second;
        if ((S_OK == _symbol->get_udtKind(&dwValue)) && (dwValue < cntUdtKindName))
            sstream << ": " << udtKindName[dwValue].second;

        bFuncDebugRange = 
            (SymTagFuncDebugStart == symTagName[dwValue].first) ||
            (SymTagFuncDebugEnd == symTagName[dwValue].first);

        if (S_OK == _symbol->get_count(&dwValue))
            sstream << ", Count: " << std::dec << dwValue;

        hres = _symbol->get_dataKind(&dwValue);
        if ((S_OK == hres) && (DataIsUnknown != dwValue))
        {
            if (dwValue < _countof(dataKindName))
                sstream << ", " << dataKindName[dwValue].second;
        }
    }
    else
    {
        sstream << "!invalid symTag!";
    }
    sstream << ", ";

    hres = _symbol->get_name(&bstrValue);
    if (S_OK == hres)
        sstream << "\"" << bstrValue.asStr().c_str() << "\"";
    else
        sstream << "<no-name>";
    bstrValue.free();

    hres = _symbol->get_length(&ullValue);
    if (S_OK == hres)
        sstream << ", Length: 0x" << std::hex << ullValue;

    hres = _symbol->get_targetSection(&dwValue);
    if (S_OK == hres)
        sstream << ", Section: " << std::dec << ullValue;

    bValue = false;
    try
    {
        getValueImpl(_symbol, vtValue);
        bValue = true;
    }
    catch (const Exception &except)
    {
        DBG_UNREFERENCED_PARAMETER(except);
    }
    if (bValue)
        printVariant(vtValue, sstream);

    hres = _symbol->get_baseType(&dwValue);
    if (SUCCEEDED(hres) && btNoType != dwValue)
    {
        for (ULONG i = 0; i < cntBasicTypeName; ++i)
        {
            if (basicTypeName[i].first == dwValue)
            {
                sstream << ", Basic type: " << basicTypeName[i].second;
                break;
            }
        }
    }

    DiaSymbolPtr pType;
    hres = _symbol->get_type(&pType);
    if (S_OK == hres)
    {
        sstream << std::endl;
        sstream << printImpl(pType, machineType, indent+1, &_loop, "Type: ").c_str();
    }

    if (bFuncDebugRange)
        return sstream.str();

    DiaEnumSymbolsPtr symbols;
    hres = 
        _symbol->findChildren(
            SymTagNull,
            NULL,
            nsCaseSensitive,
            &symbols);
    if (S_OK == hres)
    {
        if (indent <= 5)
        {
            DiaSymbolPtr child;
            ULONG celt;
            while ( SUCCEEDED(symbols->Next(1, &child, &celt)) && (celt == 1) )
            {
                sstream << std::endl;
                sstream << printImpl(child, machineType, indent + 1, &_loop).c_str();
                child.Release();
            }
        }
        else
        {
            lValue = 0;
            symbols->get_Count(&lValue);
            if (lValue)
            {
                sstream << std::endl;
                for (ULONG i =0; i < indent+1; ++i)
                    sstream << " ";
                sstream << "<...>";
            }
        }
    }
    return sstream.str();
}

////////////////////////////////////////////////////////////////////////////////

}
