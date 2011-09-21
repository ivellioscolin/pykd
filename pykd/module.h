#pragma once

#include <string>

#include "dbgobj.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

class Module : private DbgObject {

public:
    
    Module( IDebugClient5 *client, const std::string& moduleName );

    Module( IDebugClient5 *client, ULONG64 offset );

    std::string  getName() {
        return m_name;
    }

    ULONG64  getBase() {
        return m_base;
    }

    ULONG64 getEnd() {
        return m_base + m_size;
    }

    ULONG  getSize() {
        return m_size;
    }

    std::wstring
    getPdbName();

    void
    reloadSymbols();

private:

    std::string     m_name;
    ULONG64         m_base;
    ULONG           m_size;

};

///////////////////////////////////////////////////////////////////////////////////

};






//#include <string>
//#include <map>
//
//#include "dbgext.h"
//#include "dbgmem.h"
//
///////////////////////////////////////////////////////////////////////////////////
//
//// global unique module data
//// WARNING: add only numeric field or change operator <
//struct ModuleInfo
//{
//    ULONG64 m_base;
//    ULONG m_timeDataStamp;
//    ULONG m_checkSumm;
//
//    ModuleInfo()
//      : m_base(0)
//      , m_timeDataStamp(0)
//      , m_checkSumm(0)
//    {
//    }
//    ModuleInfo(
//        const ModuleInfo &rhs
//    ) : m_base(rhs.m_base)
//      , m_timeDataStamp(rhs.m_timeDataStamp)
//      , m_checkSumm(rhs.m_checkSumm)
//    {
//    }
//    ModuleInfo(
//        const IMAGEHLP_MODULEW64 &dbgImageHelperInfo
//    ) : m_base(addr64(dbgImageHelperInfo.BaseOfImage))
//      , m_timeDataStamp(dbgImageHelperInfo.TimeDateStamp)
//      , m_checkSumm(dbgImageHelperInfo.CheckSum)
//    {
//    }
//    ModuleInfo(
//        const DEBUG_MODULE_PARAMETERS &dbgModuleParameters
//    ) : m_base(addr64(dbgModuleParameters.Base))
//      , m_timeDataStamp(dbgModuleParameters.TimeDateStamp)
//      , m_checkSumm(dbgModuleParameters.Checksum)
//    {
//    }
//
//    bool operator ==(const ModuleInfo &rhs) const
//    {
//        return m_base == rhs.m_base
//            && m_timeDataStamp == rhs.m_timeDataStamp
//            && m_checkSumm == rhs.m_checkSumm;
//    }
//    bool operator < (const ModuleInfo &rhs) const
//    {
//        return memcmp(this, &rhs, sizeof(ModuleInfo)) < 0;
//    }
//};
//
///////////////////////////////////////////////////////////////////////////////////
//
//class dbgModuleClass {
//
//public:
//
//    dbgModuleClass() :
//        m_base( 0 ),
//        m_end( 0 )
//    {}
//
//    dbgModuleClass( const std::string &name, ULONG64 base, ULONG size );
//
//    ULONG64
//    getBegin() const {
//        return m_base;
//    }
//
//    ULONG64
//    getEnd() const {
//        return m_end;
//    }
//    
//    ULONG
//    getSize() const {
//        return (ULONG)( m_end - m_base );
//    }        
//
//    bool
//    contain( ULONG64  addr ) const {
//        if ( *( (ULONG*)&addr + 1 ) == 0 )
//            *( (ULONG*)&addr + 1 ) = 0xFFFFFFFF;
//                          
//        return m_base <= addr && addr <= m_end;
//    }
//
//    std::string
//    getName() const {
//        return m_name;    
//    }
//
//    void
//    reloadSymbols();
//
//    ULONG64
//    getOffset( const std::string  &symName );
//
//    std::wstring
//    getImageSymbolName() const {
//        return m_imageFullName;
//    }
//
//    std::wstring
//    getPdbName() const {
//        return std::wstring( m_debugInfo.LoadedPdbName );
//    }
//    
//    ULONG
//    getCheckSum() const {
//        return m_debugInfo.CheckSum;
//    }
//    
//    ULONG
//    getTimeStamp() const {
//        return m_debugInfo.TimeDateStamp;
//    }        
//
//    bool 
//    addSyntheticSymbol( ULONG64 offset, ULONG size, const std::string &symName );
//
//    void
//    delAllSyntheticSymbols();
//
//    ULONG
//    delSyntheticSymbol( ULONG64 offset );
//
//    ULONG
//    delSyntheticSymbolsMask( const std::string &symName );
//
//    std::string
//    print() const;
//
//private:
//
//    ULONG64         m_base;
//
//    ULONG64         m_end;
//    
//    std::string     m_name;
//
//    std::wstring    m_imageFullName;
//
//    IMAGEHLP_MODULEW64        m_debugInfo;
//
//    typedef std::map<std::string, ULONG64>  OffsetMap;
//    OffsetMap       m_offsets;
//
//    void
//    getImagePath();
//};
//
///////////////////////////////////////////////////////////////////////////////////
//
//boost::python::object
//loadModule( const std::string &moduleName );
//
//// query module parameters (for construct dbgModuleClass) by virtual address
//// error : DbgException exception
//void queryModuleParams(
//    __in ULONG64 addr,
//    __out std::string &name,
//    __out ULONG64 &base,
//    __out ULONG &size
//);
//
//boost::python::object
//findModule( ULONG64 addr );
//
///////////////////////////////////////////////////////////////////////////////////
