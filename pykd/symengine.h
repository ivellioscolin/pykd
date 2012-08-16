#pragma once

#include "variant.h"

namespace pykd {

////////////////////////////////////////////////////////////////////////////////

class Symbol;
typedef boost::shared_ptr< Symbol > SymbolPtr;
typedef std::list< SymbolPtr > SymbolPtrList;

class SymbolSession;
typedef boost::shared_ptr<SymbolSession> SymbolSessionPtr;

////////////////////////////////////////////////////////////////////////////////

enum SymTagEnum
{
    SymTagNull,
    SymTagExe,
    SymTagCompiland,
    SymTagCompilandDetails,
    SymTagCompilandEnv,
    SymTagFunction,
    SymTagBlock,
    SymTagData,
    SymTagAnnotation,
    SymTagLabel,
    SymTagPublicSymbol,
    SymTagUDT,
    SymTagEnum,
    SymTagFunctionType,
    SymTagPointerType,
    SymTagArrayType,
    SymTagBaseType,
    SymTagTypedef,
    SymTagBaseClass,
    SymTagFriend,
    SymTagFunctionArgType,
    SymTagFuncDebugStart,
    SymTagFuncDebugEnd,
    SymTagUsingNamespace,
    SymTagVTableShape,
    SymTagVTable,
    SymTagCustom,
    SymTagThunk,
    SymTagCustomType,
    SymTagManagedType,
    SymTagDimension,
    SymTagMax
};

////////////////////////////////////////////////////////////////////////////////

enum LocationType
{
    LocIsNull,
    LocIsStatic,
    LocIsTLS,
    LocIsRegRel,
    LocIsThisRel,
    LocIsEnregistered,
    LocIsBitField,
    LocIsSlot,
    LocIsIlRel,
    LocInMetaData,
    LocIsConstant,
    LocTypeMax
};

////////////////////////////////////////////////////////////////////////////////

enum DataKind
{
    DataIsUnknown,
    DataIsLocal,
    DataIsStaticLocal,
    DataIsParam,
    DataIsObjectPtr,
    DataIsFileStatic,
    DataIsGlobal,
    DataIsMember,
    DataIsStaticMember,
    DataIsConstant
};

////////////////////////////////////////////////////////////////////////////////

enum BasicType
{
    btNoType = 0,
    btVoid = 1,
    btChar = 2,
    btWChar = 3,
    btInt = 6,
    btUInt = 7,
    btFloat = 8,
    btBCD = 9,
    btBool = 10,
    btLong = 13,
    btULong = 14,
    btCurrency = 25,
    btDate = 26,
    btVariant = 27,
    btComplex = 28,
    btBit = 29,
    btBSTR = 30,
    btHresult = 31
};

////////////////////////////////////////////////////////////////////////////////

class Symbol {

public:

    virtual SymbolPtrList findChildren( ULONG symTag, const std::string &name = "", bool caseSensitive = FALSE ) = 0;
    virtual ULONG getBaseType() = 0;
    virtual ULONG getBitPosition() = 0;
    virtual SymbolPtr getChildByIndex(ULONG _index ) = 0;
    virtual SymbolPtr getChildByName(const std::string &_name ) = 0;
    virtual ULONG getChildCount() = 0;
    virtual ULONG getChildCount(ULONG symTag ) = 0;
    virtual ULONG getCount() = 0;
    virtual ULONG getDataKind() = 0;
    virtual ULONG getLocType() = 0;
    virtual ULONG getMachineType() = 0;
    virtual std::string getName() = 0;
    virtual LONG getOffset() = 0;
    virtual ULONG getRva() = 0;
    virtual ULONGLONG getSize() = 0;
    virtual ULONG getSymTag() = 0;
    virtual SymbolPtr getType() = 0;
    virtual std::string getUndecoratedName() = 0;
    virtual ULONGLONG getVa() = 0;
    virtual void getValue( BaseTypeVariant &vtValue) = 0;
    virtual ULONG getVirtualBaseDispIndex() = 0;
    virtual int getVirtualBasePointerOffset() = 0;
    virtual bool isVirtualBaseClass() = 0;
    virtual ULONG getVirtualBaseDispSize() = 0;
};

///////////////////////////////////////////////////////////////////////////////

class SymbolSession {

public:

    virtual SymbolPtr& getSymbolScope() = 0;

    virtual SymbolPtr findByRva( ULONG rva, ULONG symTag = SymTagNull, LONG* displacement = NULL ) = 0;

};

///////////////////////////////////////////////////////////////////////////////

std::string getBasicTypeName( ULONG basicType );

SymbolSessionPtr  loadSymbolFile(const std::string &filePath, ULONGLONG loadBase = 0);

////////////////////////////////////////////////////////////////////////////////

}; // end pykd endpoint