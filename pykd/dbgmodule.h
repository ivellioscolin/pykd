#pragma once

#include <string>
#include <map>

/////////////////////////////////////////////////////////////////////////////////

// global unique module data
// WARNING: add only numeric field or change operator <
struct ModuleInfo
{
    ULONG64 m_base;
    ULONG m_timeDataStamp;
    ULONG m_checkSumm;

    ModuleInfo()
      : m_base(0)
      , m_timeDataStamp(0)
      , m_checkSumm(0)
    {
    }
    ModuleInfo(
        const ModuleInfo &rhs
    ) : m_base(rhs.m_base)
      , m_timeDataStamp(rhs.m_timeDataStamp)
      , m_checkSumm(rhs.m_checkSumm)
    {
    }
    ModuleInfo(
        const IMAGEHLP_MODULEW64 &dbgImageHelperInfo
    ) : m_base(addr64(dbgImageHelperInfo.BaseOfImage))
      , m_timeDataStamp(dbgImageHelperInfo.TimeDateStamp)
      , m_checkSumm(dbgImageHelperInfo.CheckSum)
    {
    }
    ModuleInfo(
        const DEBUG_MODULE_PARAMETERS &dbgModuleParameters
    ) : m_base(addr64(dbgModuleParameters.Base))
      , m_timeDataStamp(dbgModuleParameters.TimeDateStamp)
      , m_checkSumm(dbgModuleParameters.Checksum)
    {
    }

    bool operator ==(const ModuleInfo &rhs) const
    {
        return m_base == rhs.m_base
            && m_timeDataStamp == rhs.m_timeDataStamp
            && m_checkSumm == rhs.m_checkSumm;
    }
    bool operator < (const ModuleInfo &rhs) const
    {
        return memcmp(this, &rhs, sizeof(ModuleInfo)) < 0;
    }
};

/////////////////////////////////////////////////////////////////////////////////

class dbgModuleClass {

public:

    dbgModuleClass() :
        m_base( 0 ),
        m_end( 0 )     
    {}

    dbgModuleClass( const std::string &name, ULONG64 base, ULONG size );
    
    ULONG64
    getBegin() const {
        return m_base;
    }

    ULONG64
    getEnd() const {
        return m_end;
    }

    bool
    contain( ULONG64  addr ) const {
        if ( *( (ULONG*)&addr + 1 ) == 0 )
            *( (ULONG*)&addr + 1 ) = 0xFFFFFFFF;
                          
        return m_base <= addr && addr <= m_end;
    }

    std::string
    getName() const {
        return m_name;    
    }

    void
    reloadSymbols();

    ULONG64
    getOffset( const std::string  &symName );

    std::wstring
    getImageSymbolName() const {
        return m_imageFullName;
    }

    std::wstring
    getPdbName() const {
        return std::wstring( m_debugInfo.LoadedPdbName );
    }

    bool 
    addSyntheticSymbol( ULONG64 offset, ULONG size, const std::string &symName );

    void
    delAllSyntheticSymbols();

    ULONG
    delSyntheticSymbol( ULONG64 offset );

    ULONG
    delSyntheticSymbolsMask( const std::string &symName );

    std::string
    print() const;

private:

    ULONG64         m_base;

    ULONG64         m_end;

    std::string     m_name;

    std::wstring    m_imageFullName;

    IMAGEHLP_MODULEW64        m_debugInfo;

    typedef std::map<std::string, ULONG64>  OffsetMap;
    OffsetMap       m_offsets;

    void
    getImagePath();
};

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
loadModule( const std::string &moduleName );


boost::python::object
findModule( ULONG64 addr );

/////////////////////////////////////////////////////////////////////////////////