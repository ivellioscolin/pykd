
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

    std::stringstream   sstr;
    sstr << "field \"" << name << " not found";

    throw TypeException( m_name, sstr.str() );
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

    throw TypeException( name,  "field not found" );
}

///////////////////////////////////////////////////////////////////////////////////

TypeInfoPtr SymbolUdtField::getTypeInfo()
{
    return TypeInfo::getTypeInfo(m_symbol);
}

///////////////////////////////////////////////////////////////////////////////////

}   // namespace pykd

///////////////////////////////////////////////////////////////////////////////////
