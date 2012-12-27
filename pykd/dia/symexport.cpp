#include "stdafx.h"
#include "symengine.h"
#include "dbgexcept.h"
#include "dbgmem.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/tag.hpp>

namespace bmi = boost::multi_index;

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

class ExportSymbolBase : public Symbol
{

    virtual SymbolPtrList findChildren( ULONG symTag, const std::string &name = "", bool caseSensitive = FALSE )
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual ULONG getBaseType()
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual ULONG getBitPosition()
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual SymbolPtr getChildByIndex(ULONG _index )
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual SymbolPtr getChildByName(const std::string &_name )
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual ULONG getChildCount()
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual ULONG getChildCount(ULONG symTag )
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual ULONG getCount()
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual ULONG getDataKind()
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual SymbolPtr getIndexType()
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual ULONG getLocType()
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }
    
    virtual ULONG getMachineType()
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual std::string getName()
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual LONG getOffset()
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual ULONG getRva()
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual ULONGLONG getSize()
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual ULONG getSymTag()
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual SymbolPtr getType()
    {        
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual ULONG getUdtKind()
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual ULONGLONG getVa()
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual void getValue( BaseTypeVariant &vtValue)
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual ULONG getVirtualBaseDispIndex()
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual int getVirtualBasePointerOffset()
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual ULONG getVirtualBaseDispSize()
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual bool isBasicType()
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual bool isConstant()
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual bool isIndirectVirtualBaseClass()
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual bool isVirtualBaseClass()
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }

    virtual ULONG getRegRealativeId()
    {
        throw ImplementException( __FILE__, __LINE__, "TODO" );
    }
};

///////////////////////////////////////////////////////////////////////////////

class ExportSymbol : public ExportSymbolBase
{
public:

    ExportSymbol( const std::string &name, ULONG rva ) :
      m_name( name ),
      m_rva( rva )
      {}


private:

    virtual std::string getName()
    {
        return m_name;
    }

    virtual ULONG getRva()
    {
        return m_rva;
    }

    std::string  m_name;
    ULONG  m_rva;

};

///////////////////////////////////////////////////////////////////////////////

class FunctionMap
{
public:
    typedef ULONG           FunctionAddress;
    typedef LONG            FunctionOffset;
    typedef std::string     FunctionName;

    bool add(const FunctionName& name, const FunctionAddress address)
    {
        typedef std::pair<NameIndex::iterator, bool> InsertionResult;

        const Entry entry(name, address);
        InsertionResult result = GetNameIndex().insert(entry);

        //NB: true if item inserted, false - if already exists
       return result.second;
    }

    void clear()
    {
        GetNameIndex().clear();
    }

    bool findByName(const FunctionName& name, FunctionAddress& address) const
    {
        NameIndex::const_iterator it = GetNameIndex().find(name);
        if (GetNameIndex().end() == it)
            return false;
        
        address = (*it).Address;
        return true;        
    }

    bool findByAddress(const FunctionAddress address, FunctionName& name, FunctionAddress& funcAddress, FunctionOffset& offset ) const
    {
        if (GetAddressIndex().empty())
            return false;

        AddressIndex::const_iterator it = GetAddressIndex().lower_bound(address);
        if (GetAddressIndex().end() == it)
        {
            name = (*GetAddressIndex().rbegin()).Name;
            offset = static_cast<FunctionOffset>(address - (*GetAddressIndex().rbegin()).Address);
            funcAddress = (*GetAddressIndex().rbegin()).Address;
        }
        else
        {
           if ((address < (*it).Address) && GetAddressIndex().begin() != it)
                --it;

            name = (*it).Name;
            offset = static_cast<FunctionOffset>(address - (*it).Address);
            funcAddress = (*it).Address;
        }
        return true;        
    }

private:
    struct Entry
    {
        Entry(const FunctionName& name, const FunctionAddress address)
            : Name(name)
            , Address(address)
        {}

        FunctionName    Name;
        FunctionAddress Address;
    };

    struct ByAddress {};
    struct ByName {};

    typedef bmi::multi_index_container<
        Entry,
        bmi::indexed_by<
            bmi::ordered_unique<
                bmi::tag<ByAddress>,
                bmi::member<Entry, FunctionAddress, &Entry::Address>
            >,
            bmi::hashed_unique<
                bmi::tag<ByName>,
                bmi::member<Entry, FunctionName, &Entry::Name>
            >
        >
    > Container;

    typedef Container::index<ByAddress>::type AddressIndex;
    typedef Container::index<ByName>::type NameIndex;

private:
    const AddressIndex& GetAddressIndex() const
    {
        return m_storage.get<ByAddress>();
    }

    AddressIndex& GetAddressIndex()
    {
        return m_storage.get<ByAddress>();
    }

    const NameIndex& GetNameIndex() const
    {
        return m_storage.get<ByName>();
    }

    NameIndex& GetNameIndex()
    {
        return m_storage.get<ByName>();
    }

private:
    Container m_storage;
};

///////////////////////////////////////////////////////////////////////////////

class ExportSymbolDir;
typedef boost::shared_ptr<ExportSymbolDir> ExportSymbolDirPtr;

class ExportSymbolDir : public ExportSymbolBase
{
public:

    static ExportSymbolDirPtr getExportSymbolDir( ULONGLONG moduleBase )
    {
        return ExportSymbolDirPtr( new ExportSymbolDir(moduleBase) );
    }

    SymbolPtr findByRva( ULONG rva, ULONG symTag, LONG* displacement )
    {
        std::string name;
        ULONG address;
        LONG offset;

        if ( !m_exportMap.findByAddress( rva, name, address, offset ) )
            throw SymbolException( "dymbol can not be found by rva");

        if ( displacement )
            *displacement = offset;

        return SymbolPtr( new ExportSymbol( name, address ) );
    }

private:

    ExportSymbolDir( ULONGLONG moduleBase )
    {
        ULONG64  ntHeaderOffset = moduleBase + ptrDWord( moduleBase + 0x3c );

        IMAGE_NT_HEADERS  ntHeaders;
        readMemory( ntHeaderOffset, &ntHeaders, sizeof(ntHeaders) );

        m_machineType = ntHeaders.FileHeader.Machine;

        if ( m_machineType == IMAGE_FILE_MACHINE_I386 )
        {
            GetExport<IMAGE_NT_HEADERS32>( moduleBase );
            return;
        }
        
        if ( m_machineType == IMAGE_FILE_MACHINE_AMD64 )
        {
            GetExport<IMAGE_NT_HEADERS64>( moduleBase );
            return;
        }

        throw SymbolException( "Unkonw machine type");
    }

    template<typename NTHEADERT>
    void  GetExport( ULONG64 moduleBase )
    {
        ULONG64  ntHeaderOffset = moduleBase + ptrDWord( moduleBase + 0x3c );

        NTHEADERT  ntHeader;
        readMemory( ntHeaderOffset, &ntHeader, sizeof(ntHeader) );
    
        if ( ntHeader.OptionalHeader.DataDirectory[0].Size == 0 )
            return;

        ULONG64  exportDirOffset = moduleBase + ntHeader.OptionalHeader.DataDirectory[0].VirtualAddress;

        ULONG  namesCount = (ULONG) ptrDWord( exportDirOffset + 0x18 );
        ULONG64  funcRvaOffset = moduleBase + ptrDWord( exportDirOffset + 0x1C );
        ULONG64  namesOffset = moduleBase + ptrDWord( exportDirOffset + 0x20 ); 
        ULONG64  ordinalsOffset = moduleBase + ptrDWord( exportDirOffset + 0x24 );

        for ( ULONG i = 0; i < namesCount; ++i )
        {
            std::string  exportName = loadCStr( moduleBase + ptrDWord( namesOffset + 4 * i ) );
            USHORT  exportOrdinal = (USHORT)ptrWord( ordinalsOffset + 2 * i );
            ULONG rva = (ULONG)ptrDWord( funcRvaOffset + 4 * exportOrdinal );

            m_exportMap.add( exportName, rva );
        }
    }

    virtual SymbolPtr getChildByName(const std::string &name )
    {
        FunctionMap::FunctionAddress addr;            

         if( !m_exportMap.findByName( name, addr ) )
             throw SymbolException(name + " is not found");

        return SymbolPtr( new ExportSymbol( name, addr ) );

    }

    FunctionMap m_exportMap;

    ULONG  m_machineType;
};

///////////////////////////////////////////////////////////////////////////////

class ExportSession : public SymbolSession
{
public:

    ExportSession( ULONGLONG moduleBase ) :
      m_moduleBase( moduleBase )
      {
          m_exportDir = ExportSymbolDir::getExportSymbolDir(moduleBase);
      }

    SymbolPtr getSymbolScope() {
        return SymbolPtr( m_exportDir );
    }

    SymbolPtr findByRva( ULONG rva, ULONG symTag = SymTagNull, LONG* displacement = NULL ) {
        return m_exportDir->findByRva( rva, symTag, displacement );
    }

    virtual void getSourceLine( ULONG64 offset, std::string &fileName, ULONG &lineNo, LONG &displacement ) 
    {
        throw SymbolException( "there is no source file" );
    }

private:

    ULONGLONG           m_moduleBase;

    ExportSymbolDirPtr   m_exportDir;
};

///////////////////////////////////////////////////////////////////////////////

SymbolSessionPtr loadSymbolFromExports(ULONGLONG loadBase)
{
    return SymbolSessionPtr( new ExportSession( loadBase ) );
}

///////////////////////////////////////////////////////////////////////////////

}; // end pykd namespace
