#include "stdafx.h"

#include "pytypedvar.h"
#include "kdlib/exceptions.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

kdlib::TypedVarPtr getTypedVarByTypeName(const std::wstring &name, python::object& dataStorage)
{
    python::extract<kdlib::MEMOFFSET_64>  get_addr(dataStorage);
    if ( get_addr.check() )
    {
        kdlib::MEMOFFSET_64   offset = get_addr();
        AutoRestorePyState  pystate;
        return kdlib::loadTypedVar( name, offset );
    }

    std::vector<char>   byteArray;

    for (int i = 0; i < python::len(dataStorage); ++i)
    {
        byteArray.push_back( python::extract<unsigned char>(dataStorage[i]) );
    }
    
    AutoRestorePyState  pystate;
    return kdlib::loadTypedVar( name, kdlib::getCacheAccessor(byteArray) );
}

///////////////////////////////////////////////////////////////////////////////

kdlib::TypedVarPtr getTypedVarByTypeInfo( const kdlib::TypeInfoPtr &typeInfo, python::object& dataStorage)
{
    python::extract<kdlib::MEMOFFSET_64>  get_addr(dataStorage);
    if ( get_addr.check() )
    {
        kdlib::MEMOFFSET_64   offset = get_addr();
        AutoRestorePyState  pystate;
        return kdlib::loadTypedVar(typeInfo, offset );
    }

    std::vector<char>   byteArray;

    for (int i = 0; i < python::len(dataStorage); ++i)
    {
        byteArray.push_back( python::extract<unsigned char>(dataStorage[i]) );
    }
    
    AutoRestorePyState  pystate;
    return kdlib::loadTypedVar(typeInfo, kdlib::getCacheAccessor(byteArray) );
}

///////////////////////////////////////////////////////////////////////////////

python::list getTypedVarListByTypeName( kdlib::MEMOFFSET_64 offset, const std::wstring &typeName, const std::wstring &fieldName )
{
    kdlib::TypedVarList  lst;

    do {
        AutoRestorePyState  pystate;
        lst = kdlib::loadTypedVarList( offset, typeName, fieldName );
    } while(false);

    return vectorToList( lst );
}

///////////////////////////////////////////////////////////////////////////////

python::list getTypedVarListByType( kdlib::MEMOFFSET_64 offset, kdlib::TypeInfoPtr &typeInfo, const std::wstring &fieldName )
{
    kdlib::TypedVarList  lst;

    do {
        AutoRestorePyState  pystate;
        lst = kdlib::loadTypedVarList( offset, typeInfo, fieldName );
    } while(false);

    return vectorToList( lst );
}

///////////////////////////////////////////////////////////////////////////////

python::list getTypedVarArrayByTypeName( kdlib::MEMOFFSET_64 offset, const std::wstring &typeName, size_t number )
{
    kdlib::TypedVarList  lst;
    
    do {
        AutoRestorePyState  pystate;
        lst = kdlib::loadTypedVarArray( offset, typeName, number );
    } while(false);

    return vectorToList( lst );
}

///////////////////////////////////////////////////////////////////////////////

python::list getTypedVarArrayByType( kdlib::MEMOFFSET_64 offset, kdlib::TypeInfoPtr &typeInfo, size_t number )
{
    kdlib::TypedVarList  lst;
    
    do {
        AutoRestorePyState  pystate;
        lst = kdlib::loadTypedVarArray( offset, typeInfo, number );
    } while(false);

    return vectorToList( lst );
}

///////////////////////////////////////////////////////////////////////////////

python::list TypedVarAdapter::getFields( kdlib::TypedVar& typedVar )
{
    typedef boost::tuple<std::wstring,kdlib::MEMOFFSET_32,kdlib::TypedVarPtr> FieldTuple;

    std::list<FieldTuple>  lst;

    do {

        AutoRestorePyState  pystate;

        for ( size_t i = 0; i < typedVar.getElementCount(); ++i )
        {
            std::wstring  name = typedVar.getElementName(i);
            kdlib::MEMOFFSET_32  offset = 0;

            if (!typedVar.getType()->isStaticMember(i) )
                offset = typedVar.getElementOffset(i);

            kdlib::TypedVarPtr  val = typedVar.getElement(i);

            lst.push_back( FieldTuple( name, offset, val ) );
        }

    } while(false);

    python::list pylst;
    
    for ( std::list<FieldTuple>::const_iterator it = lst.begin(); it != lst.end(); ++it)
        pylst.append( python::make_tuple( it->get<0>(), it->get<1>(), it->get<2>() ) );

    return pylst;
}

///////////////////////////////////////////////////////////////////////////////

python::list TypedVarAdapter::getElementsDir(kdlib::TypedVar& typedVar)
{
    std::list<std::wstring>  lst;
    python::list pylst;

    try {

        AutoRestorePyState  pystate;

        for (size_t i = 0; i < typedVar.getElementCount(); ++i)
        {
            std::wstring  name = typedVar.getElementName(i);
            lst.push_back(name);
        }

    } catch(kdlib::DbgException&)
    {
        return pylst;
    }

    for (std::list<std::wstring>::const_iterator it = lst.begin(); it != lst.end(); ++it)
        pylst.append(*it);

    return pylst;
}

///////////////////////////////////////////////////////////////////////////////

kdlib::TypedVarPtr TypedVarAdapter::getFieldAttr(kdlib::TypedVar& typedVar, const std::wstring &name)
{
    {

        AutoRestorePyState  pystate;

        try
        {
            return typedVar.getElement( name );
        }
        catch (kdlib::TypeException&)
        {}

        try
        {
            return typedVar.getMethod( name );
        }
        catch (kdlib::TypeException&)
        {}
    }

    std::wstringstream sstr;
    sstr << L"typed var has no field " << L'\'' << name << L'\'';
    throw AttributeException(std::string(_bstr_t(sstr.str().c_str())).c_str());
}

///////////////////////////////////////////////////////////////////////////////

python::list TypedVarAdapter::getRawBytes(kdlib::TypedVar& typedVar)
{

   std::vector<unsigned char>   rawBytes;

   {
        AutoRestorePyState  pystate;
        kdlib::DataAccessorPtr   dataStream = kdlib::getCacheAccessor(typedVar.getSize());
        typedVar.writeBytes(dataStream);
        dataStream->readBytes(rawBytes, typedVar.getSize());

   }

   return vectorToList( rawBytes );
 }

///////////////////////////////////////////////////////////////////////////////

} // namesapce pykd

