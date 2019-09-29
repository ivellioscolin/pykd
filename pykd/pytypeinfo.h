#pragma  once

#include <comutil.h>

#include "kdlib/typeinfo.h"

#include "pythreadstate.h"
#include "dbgexcept.h"

namespace pykd {

inline kdlib::MEMOFFSET_64 getSymbolOffset( const std::wstring &name )
{
    AutoRestorePyState  pystate;
    return kdlib::getSymbolOffset(name);
}

std::wstring findSymbol(  kdlib::MEMOFFSET_64 offset, bool showDisplacement = true );

python::tuple findSymbolAndDisp( ULONG64 offset );

inline size_t getSymbolSize( const std::wstring &name )
{
    AutoRestorePyState  pystate;
    return kdlib::getSymbolSize(name);
}

inline kdlib::TypeInfoPtr defineStruct( const std::wstring &structName, size_t align = 0 ) 
{
    AutoRestorePyState  pystate;
    return kdlib::defineStruct(structName, align);
}

inline kdlib::TypeInfoPtr defineUnion( const std::wstring& unionName, size_t align = 0 )
{
    AutoRestorePyState  pystate;
    return kdlib::defineUnion(unionName, align);
}


inline kdlib::TypeInfoPtr defineFunction( const kdlib::TypeInfoPtr& returnType, kdlib::CallingConventionType callconv = kdlib::CallConv_NearC)
{
    AutoRestorePyState  pystate;
    return kdlib::defineFunction(returnType, callconv);
}

python::object callFunctionByVar( python::tuple& args, python::dict& kwargs );

python::object callFunctionByOffset( python::tuple& args, python::dict& kwargs);

python::object callFunctionRaw( python::tuple& args, python::dict& kwargs);


inline kdlib::TypeInfoPtr getTypeInfoByName( const std::wstring &name )
{
    AutoRestorePyState  pystate;
    return kdlib::loadType( name );
}

inline kdlib::TypeInfoPtr getTypeFromSource( const std::wstring& sourceCode, const std::wstring& typeName, const std::wstring& compileOptions=L"")
{
    AutoRestorePyState  pystate;
    return kdlib::compileType( sourceCode, typeName, compileOptions);
}

inline kdlib::TypeInfoProviderPtr getTypeInfoProviderFromSource(const std::wstring& sourceCode, const std::wstring& compileOptions=L"")
{
    AutoRestorePyState  pystate;
    return kdlib::getTypeInfoProviderFromSource(sourceCode, compileOptions);
}

inline kdlib::SymbolProviderPtr getSymbolProviderFromSource(const std::wstring& sourceCode, const std::wstring& compileOptions = L"")
{
    AutoRestorePyState  pystate;
    return kdlib::getSymbolProviderFromSource(sourceCode, compileOptions);
}

inline kdlib::TypeInfoProviderPtr getTypeInfoProviderFromPdb(const std::wstring&  fileName, kdlib::MEMOFFSET_64 offset = 0UL)
{
    AutoRestorePyState  pystate;
    return kdlib::getTypeInfoProviderFromPdb(fileName, offset);
}

struct TypeInfoAdapter : public kdlib::TypeInfo {

    static std::wstring getName( kdlib::TypeInfo &typeInfo )
    {
        AutoRestorePyState  pystate;
        return typeInfo.getName();
    }

    static std::wstring getScopeName( kdlib::TypeInfo &typeInfo )
    {
        AutoRestorePyState  pystate;
        return typeInfo.getScopeName();
    }

    static size_t getSize( kdlib::TypeInfo &typeInfo )
    {
        AutoRestorePyState  pystate;
        return typeInfo.getSize();
    }

    static kdlib::BITOFFSET getBitOffset( kdlib::TypeInfo &typeInfo )
    {
        AutoRestorePyState  pystate;
        return typeInfo.getBitOffset();
    }

    static kdlib::BITOFFSET getBitWidth( kdlib::TypeInfo &typeInfo ) 
    {
        AutoRestorePyState  pystate;
        return typeInfo.getBitWidth();
    }

    static kdlib::TypeInfoPtr getBitType( kdlib::TypeInfo &typeInfo ) 
    {
        AutoRestorePyState  pystate;
        return typeInfo.getBitType();
    }

    static size_t getElementCount( kdlib::TypeInfo &typeInfo )
    {
        AutoRestorePyState  pystate;
        return typeInfo.getElementCount();
    }

    static kdlib::MEMOFFSET_32 getElementOffset( kdlib::TypeInfo &typeInfo, const std::wstring &name ) 
    {
        AutoRestorePyState  pystate;
        return typeInfo.getElementOffset( name );
    }

    static kdlib::MEMOFFSET_32 getElementOffsetByIndex(kdlib::TypeInfo &typeInfo, size_t index)
    {
        AutoRestorePyState  pystate;
        return typeInfo.getElementOffset(index);
    }

    static std::wstring getElementName( kdlib::TypeInfo &typeInfo, size_t index ) 
    {
        AutoRestorePyState  pystate;
        return typeInfo.getElementName(index);
    }

    static kdlib::MEMOFFSET_64 getStaticOffset( kdlib::TypeInfo &typeInfo, const std::wstring &name )
    {
        AutoRestorePyState  pystate;
        return typeInfo.getElementVa( name );
    }

    static bool isStaticField(const kdlib::TypeInfoPtr &typeInfo, const std::wstring &name)
    {
        AutoRestorePyState  pystate;
        return typeInfo->isStaticMember(name);
    }

    static bool isStaticFieldByIndex(const kdlib::TypeInfoPtr &typeInfo, size_t index)
    {
        AutoRestorePyState  pystate;
        return typeInfo->isStaticMember(index);
    }


    static bool isConstField(const kdlib::TypeInfoPtr &typeInfo, const std::wstring &name)
    {
        AutoRestorePyState  pystate;
        return typeInfo->isConstMember(name);
    }

    static bool isConstFieldByIndex(const kdlib::TypeInfoPtr &typeInfo, size_t index)
    {
        AutoRestorePyState  pystate;
        return typeInfo->isConstMember(index);
    }

    static kdlib::TypeInfoPtr getElementByName( kdlib::TypeInfo &typeInfo, const std::wstring &name ) 
    {
        AutoRestorePyState  pystate;
        return typeInfo.getElement(name);
    }

    static bool hasField(kdlib::TypeInfoPtr &typeInfo, const std::wstring &name);

    static bool hasMethod(kdlib::TypeInfoPtr &typeInfo, const std::wstring &fieldName);

    static kdlib::TypeInfoPtr getElementAttr(kdlib::TypeInfo &typeInfo, const std::wstring &name);

    static kdlib::TypeInfoPtr getElementByKey(kdlib::TypeInfo &typeInfo, const std::wstring &name);

    static kdlib::TypeInfoPtr getElementByIndex( kdlib::TypeInfo &typeInfo, size_t index )
    {
        AutoRestorePyState  pystate;
        return typeInfo.getElement(index);
    }

    static size_t getMethodsCount( kdlib::TypeInfo &typeInfo )
    {
        AutoRestorePyState  pystate;
        return typeInfo.getMethodsCount();
    }

    static kdlib::TypeInfoPtr getMethodByName( kdlib::TypeInfo &typeInfo, const std::wstring& methodName)
    {
        AutoRestorePyState  pystate;
        return typeInfo.getMethod(methodName);
    }

    static kdlib::TypeInfoPtr getMethodByIndex( kdlib::TypeInfo &typeInfo, size_t index )
    {
        AutoRestorePyState  pystate;
        return typeInfo.getMethod(index);
    }

    static std::wstring getMethodName(kdlib::TypeInfo &typeInfo, size_t index)
    {
        AutoRestorePyState  pystate;
        return typeInfo.getMethodName(index);
    }

    static size_t getBaseClassesCount(kdlib::TypeInfo &typeInfo )
    {
        AutoRestorePyState  pystate;
        return typeInfo.getBaseClassesCount();
    }

    static kdlib::TypeInfoPtr getBaseClassByName(kdlib::TypeInfo &typeInfo, const std::wstring&  className)
    {
        AutoRestorePyState  pystate;
        return typeInfo.getBaseClass(className);
    }
       
    static kdlib::TypeInfoPtr getBaseClassByIndex(kdlib::TypeInfo &typeInfo, size_t index)
    {
        AutoRestorePyState  pystate;
        return typeInfo.getBaseClass(index);
    }

    static kdlib::MEMOFFSET_32 getBaseClassOffsetByName(kdlib::TypeInfo &typeInfo, const std::wstring&  className)
    {
        AutoRestorePyState  pystate;
        return typeInfo.getBaseClassOffset(className);
    }

    static kdlib::MEMOFFSET_32 getBaseClassOffsetByIndex(kdlib::TypeInfo &typeInfo, size_t index)
    {
        AutoRestorePyState  pystate;
        return typeInfo.getBaseClassOffset(index);
    }

    static kdlib::TypeInfoPtr ptrTo( kdlib::TypeInfo &typeInfo, size_t ptrSize = 0 ) 
    {
        AutoRestorePyState  pystate;
        return typeInfo.ptrTo(ptrSize);
    }

    static kdlib::TypeInfoPtr deref( kdlib::TypeInfo &typeInfo )
    {
        AutoRestorePyState  pystate;
        return typeInfo.deref();
    }

    static kdlib::TypeInfoPtr arrayOf( kdlib::TypeInfo &typeInfo, size_t size ) 
    {
        AutoRestorePyState  pystate;
        return typeInfo.arrayOf(size);
    }

    static bool isArray( kdlib::TypeInfo &typeInfo )
    {
        AutoRestorePyState  pystate;
        return typeInfo.isArray();
    }

    static bool isPointer( kdlib::TypeInfo &typeInfo ) 
    {
        AutoRestorePyState  pystate;
        return typeInfo.isPointer();
    }

    static bool isVoid( kdlib::TypeInfo &typeInfo ) 
    {
        AutoRestorePyState  pystate;
        return typeInfo.isVoid();
    }

    static bool isBase( kdlib::TypeInfo &typeInfo ) 
    {
        AutoRestorePyState  pystate;
        return typeInfo.isBase();
    }

    static bool isUserDefined( kdlib::TypeInfo &typeInfo ) 
    {
        AutoRestorePyState  pystate;
        return typeInfo.isUserDefined();
    }

    static bool isConstant( kdlib::TypeInfo &typeInfo ) 
    {
        AutoRestorePyState  pystate;
        return typeInfo.isConstant();
    }

    static bool isEnum( kdlib::TypeInfo &typeInfo )
    {
        AutoRestorePyState  pystate;
        return typeInfo.isEnum();
    }

    static bool isBitField( kdlib::TypeInfo &typeInfo ) 
    {
        AutoRestorePyState  pystate;
        return typeInfo.isBitField();
    }

    static bool isFunction( kdlib::TypeInfo &typeInfo ) 
    {
        AutoRestorePyState  pystate;
        return typeInfo.isFunction();
    }

    static bool isVtbl(kdlib::TypeInfo &typeInfo)
    {
        AutoRestorePyState  pystate;
        return typeInfo.isVtbl();
    }

    static bool isNoType(kdlib::TypeInfo &typeInfo)
    {
        AutoRestorePyState  pystate;
        return typeInfo.isNoType();
    }

    static bool isTemplate(const kdlib::TypeInfoPtr &typeInfo)
    {
        AutoRestorePyState  pystate;
        return typeInfo->isTemplate();
    }

    static void appendField( kdlib::TypeInfo &typeInfo, const std::wstring &fieldName, kdlib::TypeInfoPtr &fieldType )
    {
        AutoRestorePyState  pystate;
        typeInfo.appendField( fieldName, fieldType );
    }

    static kdlib::CallingConventionType getCallingConvention( kdlib::TypeInfo &typeInfo )
    {
        AutoRestorePyState  pystate;
        return typeInfo.getCallingConvention();
    }

    static kdlib::TypeInfoPtr getClassParent( kdlib::TypeInfo &typeInfo )
    {
        AutoRestorePyState  pystate;
        return typeInfo.getClassParent();
    }

    static std::wstring str( kdlib::TypeInfo &typeInfo ) 
    {
        AutoRestorePyState  pystate;
        return typeInfo.str();
    }

    static python::list getFields( const kdlib::TypeInfoPtr &typeInfo );

    static python::list getMembers(const kdlib::TypeInfoPtr &typeInfo);

    static python::list getMethods(kdlib::TypeInfo &typeInfo);

    static python::list getElementDir(kdlib::TypeInfo &typeInfo);

    static python::list getBaseClasses(kdlib::TypeInfo &typeInfo);

    static python::list getTemplateArgs(const kdlib::TypeInfoPtr &typeInfo);

    static bool isZero(kdlib::TypeInfo &typeInfo) {
        return false;
    }

    static bool hasFieldOrMethod(kdlib::TypeInfoPtr& typedVar, const std::wstring& name);

};


class TypeInfoProviderIterator {

public:

    TypeInfoProviderIterator(kdlib::TypeInfoEnumeratorPtr typeInfoEnum) :
        m_typeInfoEnum(typeInfoEnum)
    {}

    static python::object self(const python::object& obj)
    {
        return obj;
    }

    kdlib::TypeInfoPtr next()
    {
        AutoRestorePyState  pystate;

        kdlib::TypeInfoPtr  typeInfo = m_typeInfoEnum->Next();

        if (!typeInfo)
            throw StopIteration("No more data.");

        return typeInfo;
    }

    
private:

    kdlib::TypeInfoEnumeratorPtr   m_typeInfoEnum;

};

struct TypeInfoProviderAdapter : public kdlib::TypeInfoProvider
{

    static kdlib::TypeInfoPtr getTypeByName( kdlib::TypeInfoProvider &typeInfoProvider, const std::wstring& name)
    {
        AutoRestorePyState  pystate;
        return typeInfoProvider.getTypeByName(name);
    }

    static TypeInfoProviderIterator* getTypeIter( kdlib::TypeInfoProvider &typeInfoProvider )
    {
        return new TypeInfoProviderIterator( typeInfoProvider.getTypeEnumerator() );
    };

    static TypeInfoProviderIterator* getTypeIterWithMask( kdlib::TypeInfoProvider &typeInfoProvider, const std::wstring& mask)
    {
        return new TypeInfoProviderIterator( typeInfoProvider.getTypeEnumerator(mask) );
    }

    static kdlib::TypeInfoPtr getTypeAsAttr(kdlib::TypeInfoProvider &typeInfoProvider, const std::wstring& name);

};

struct SymbolEnumeratorAdapter
{
    SymbolEnumeratorAdapter(const kdlib::SymbolEnumeratorPtr&  symEnum)
        : m_symEnum(symEnum)
    {}


    static python::object getIter(const python::object& obj)
    {
        return obj;
    }

    std::wstring next()
    {
        auto symName = m_symEnum->Next();

        if (symName.empty())
            throw StopIteration("No more data.");

        return symName;
    }

private:

    kdlib::SymbolEnumeratorPtr  m_symEnum;
};

struct SymbolProviderAdapter
{
    static SymbolEnumeratorAdapter* getIter(const kdlib::SymbolProviderPtr &symProvider)
    {
        return new SymbolEnumeratorAdapter(symProvider->getSymbolEnumerator());
    };

    static SymbolEnumeratorAdapter* getIterWithMask(const kdlib::SymbolProviderPtr &symProvider, const std::wstring& mask)
    {
        return new SymbolEnumeratorAdapter(symProvider->getSymbolEnumerator(mask));
    }
};

struct BaseTypesEnum {
    static kdlib::TypeInfoPtr getUInt1B() { return pykd::getTypeInfoByName(L"UInt1B"); }
    static kdlib::TypeInfoPtr getUInt2B() { return pykd::getTypeInfoByName(L"UInt2B");  }
    static kdlib::TypeInfoPtr getUInt4B() { return pykd::getTypeInfoByName(L"UInt4B");  }
    static kdlib::TypeInfoPtr getUInt8B() { return pykd::getTypeInfoByName(L"UInt8B");  }
    static kdlib::TypeInfoPtr getInt1B() { return pykd::getTypeInfoByName(L"Int1B"); }
    static kdlib::TypeInfoPtr getInt2B() { return pykd::getTypeInfoByName(L"Int2B"); }
    static kdlib::TypeInfoPtr getInt4B() { return pykd::getTypeInfoByName(L"Int4B"); }
    static kdlib::TypeInfoPtr getInt8B() { return pykd::getTypeInfoByName(L"Int8B"); }
    static kdlib::TypeInfoPtr getLong() { return pykd::getTypeInfoByName(L"Long"); }
    static kdlib::TypeInfoPtr getULong() { return pykd::getTypeInfoByName(L"ULong"); }
    static kdlib::TypeInfoPtr getBool() { return pykd::getTypeInfoByName(L"Bool"); }
    static kdlib::TypeInfoPtr getChar() { return pykd::getTypeInfoByName(L"Char"); }
    static kdlib::TypeInfoPtr getWChar() { return pykd::getTypeInfoByName(L"WChar"); }
    static kdlib::TypeInfoPtr getVoidPtr() { return pykd::getTypeInfoByName(L"Void*"); }
    static kdlib::TypeInfoPtr getFloat() { return pykd::getTypeInfoByName(L"Float"); }
    static kdlib::TypeInfoPtr getDouble() { return pykd::getTypeInfoByName(L"Double"); }
};

} // end namespace pykd
