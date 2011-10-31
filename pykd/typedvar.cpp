#include "stdafx.h"    

#include "typedvar.h"


namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

TypedVarPtr
TypedVar::getField( const std::string &fieldName ) 
{
    TypeInfo fieldType = m_typeInfo.getField( fieldName );

    if ( fieldType.isBasicType() )
    {
        return TypedVarPtr( new BasicTypedVar( fieldType, 0 ) );
    }

    throw DbgException( "can not get field" );
}

///////////////////////////////////////////////////////////////////////////////////

} // end pykd namespace