
#include "stdafx.h"

#include <boost\tokenizer.hpp>

#include "udtutils.h"
#include "dbgexcept.h"
#include "typeinfo.h"

///////////////////////////////////////////////////////////////////////////////////

namespace pykd {
namespace UdtUtils {

/////////////////////////////////////////////////////////////////////////////////////

const Field &FieldCollection::lookup(ULONG index) const
{
    if (index >= Base::size())
        throw PyException( PyExc_IndexError, m_baseTypeName + " index out of range" );
    return at(index);
}

/////////////////////////////////////////////////////////////////////////////////////

const Field &FieldCollection::lookup(const std::string &name) const
{
    Base::const_reverse_iterator it = 
        std::find(Base::rbegin(), Base::rend(), name);

    if ( it == Base::rend() )
        throw TypeException( m_baseTypeName, name + ": field not found" );

    return *it;
}

///////////////////////////////////////////////////////////////////////////////////

ULONG getFieldOffsetRecirsive(TypeInfoPtr typeInfo, const std::string &fieldName)
{
    // "m_field1.m_field2" -> ["m_field1", "m_field2"]
    typedef boost::char_separator<char> CharSep;
    boost::tokenizer< CharSep > tokenizer(fieldName, CharSep("."));
    if (tokenizer.begin() == tokenizer.end())
        throw TypeException(typeInfo->getName(), fieldName + ": invalid field name");

    ULONG fieldOffset = 0;

    boost::tokenizer< CharSep >::iterator it = tokenizer.begin();
    for (; it != tokenizer.end(); ++it)
    {
        const std::string &name = *it;
        fieldOffset += typeInfo->getFieldOffsetByNameNotRecursively(name);
        typeInfo = typeInfo->getField(name);
    }

    return fieldOffset;
}

///////////////////////////////////////////////////////////////////////////////////

}   // namespace UdtUtils

///////////////////////////////////////////////////////////////////////////////////

}   // namespace pykd

///////////////////////////////////////////////////////////////////////////////////
