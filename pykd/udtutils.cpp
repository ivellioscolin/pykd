
#include "stdafx.h"

#include <boost\tokenizer.hpp>

#include "udtutils.h"
#include "dbgexcept.h"
#include "typeinfo.h"

///////////////////////////////////////////////////////////////////////////////////

namespace pykd {

/////////////////////////////////////////////////////////////////////////////////////

const UdtFieldPtr& FieldCollection::lookup(ULONG index) const
{
    if (index >= m_fields.size() )
        throw PyException( PyExc_IndexError, "index out of range" );

    return m_fields[index];
}

/////////////////////////////////////////////////////////////////////////////////////

const UdtFieldPtr& FieldCollection::lookup(const std::string &name) const
{
    FieldList::const_reverse_iterator it;
    for ( it = m_fields.rbegin(); it !=  m_fields.rend(); ++it )
    {
        if ( (*it)->getName() == name )
            return *it;
    }

    throw TypeException( "",  "field not found" );
}

/////////////////////////////////////////////////////////////////////////////////////

UdtFieldPtr &FieldCollection::lookup(ULONG index)
{
    if (index >= m_fields.size() )
        throw PyException( PyExc_IndexError, "index out of range" );

    return m_fields[index];
}

/////////////////////////////////////////////////////////////////////////////////////

UdtFieldPtr &FieldCollection::lookup(const std::string &name)
{
    FieldList::reverse_iterator it;
    for ( it = m_fields.rbegin(); it !=  m_fields.rend(); ++it )
    {
        if ( (*it)->getName() == name )
            return *it;
    }

    throw TypeException( "",  "field not found" );
}

///////////////////////////////////////////////////////////////////////////////////

ULONG getFieldOffsetRecursive(TypeInfoPtr typeInfo, const std::string &fieldName)
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

TypeInfoPtr SymbolUdtField::getTypeInfo()
{
    return TypeInfo::getTypeInfo(m_symbol);
}

///////////////////////////////////////////////////////////////////////////////////

}   // namespace pykd

///////////////////////////////////////////////////////////////////////////////////
