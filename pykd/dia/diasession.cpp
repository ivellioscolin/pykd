
#include "stdafx.h"

#include "dbghelp.h"
#include "dia\diasession.h"
#include "win\utils.h"

namespace pykd {

//////////////////////////////////////////////////////////////////////////////

SymbolPtr DiaSession::findByRva( ULONG rva, ULONG symTag, LONG* pdisplacement )
{
    DiaSymbolPtr child;
    LONG displacement;

    HRESULT hres = 
        m_session->findSymbolByRVAEx(
            rva,
            static_cast<enum ::SymTagEnum>(symTag),
            &child,
            &displacement);

    if (S_OK != hres)
        throw DiaException("Call IDiaSession::findSymbolByRVAEx", hres);
    if (!child)
        throw DiaException("Call IDiaSession::findSymbolByRVAEx", E_UNEXPECTED);
    if ( !pdisplacement && displacement)
        throw DiaException("Call IDiaSession::findSymbolByRVAEx failed to find suymbol" );

    if (pdisplacement)
        *pdisplacement = displacement;

    return SymbolPtr( new DiaSymbol(child, m_globalSymbol->getMachineType() ) );
}

///////////////////////////////////////////////////////////////////////////////

void DiaSession::getSourceLine( ULONG64 offset, std::string &fileName, ULONG &lineNo, LONG &displacement )
{
    DiaEnumLineNumbersPtr  lines;

    HRESULT hres = m_session->findLinesByVA( offset, 1, &lines );
    if (S_OK != hres)
        throw DiaException("failed to find source line");

    DiaLineNumberPtr  sourceLine;
    hres = lines->Item( 0, &sourceLine );
    if (S_OK != hres)
        throw DiaException("failed to find source line");

    DiaSourceFilePtr  sourceFile;
    hres = sourceLine->get_sourceFile( &sourceFile );
    if (S_OK != hres)
        throw DiaException("failed to find source line");
    
    autoBstr  fileNameBstr;
    hres = sourceFile->get_fileName ( &fileNameBstr );
    if (S_OK != hres)
        throw DiaException("failed to find source line");
    fileName = fileNameBstr.asStr();

    hres = sourceLine->get_lineNumber( &lineNo );
    if (S_OK != hres)
        throw DiaException("failed to find source line");

    ULONGLONG  va;
    hres = sourceLine->get_virtualAddress ( &va );
    if (S_OK != hres)
        throw DiaException("failed to find source line");

    displacement = (LONG)( (LONGLONG)offset - (LONGLONG)va );
}

///////////////////////////////////////////////////////////////////////////////

}; // pykd namespace end
