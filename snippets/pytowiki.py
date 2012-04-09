#
#
#

import sys


def usage():
    print "python pytowiki.py module_name"


class CodeplexFormatter:

    def endl( self ):
        return "\n"

    def header1( self, s ):
        return "! " + s + self.endl()

    def header2( self, s ):
        return "!! " + s + self.endl()

    def header3( self, s ):
        return "!!! " + s + self.endl()

    def header4( self, s ):
        return "!!!! " + s + self.endl()

    def bulletItem( self, s ):
        return "* " + s + self.endl()

    def escapeMarkup( self, s ):
        return "{\"" + s + "\"}"

    def link( self, text, link ):
        return "[" + text + "|#" + link + "]"

    def anchor( self, link ):
        return "{anchor:" + link + "}" + self.endl()



class ModuleInfo:

    def __init__ (self, module):
        self.funcs = sorted( [ item for item in module.__dict__.values() if type(item).__name__ == "function" ], key=lambda x: x.__name__ ) 
        self.classes = sorted( [ item for item in module.__dict__.values() if type(item).__name__ == "class" ],  key=lambda x: x.__name__ )      

        for cls in  self.classes:
            cls.methods = sorted( [ item for item in cls.__dict__.values() if type(item).__name__ == "function" ], key=lambda x: x.__name__ ) 


def buildDoc( ioStream, formatter, apiInfo ):
  
    ioStream.write( formatter.header2( "Functions" ) )

    for func in apiInfo.funcs:
        ioStream.write( formatter.bulletItem( formatter.link( func.__name__, func.__name__ ) ) )

    ioStream.write( formatter.header2( "Classes" ) )

    for cls in apiInfo.classes:
        ioStream.write( formatter.bulletItem( formatter.link( cls.__name__, cls.__name__ ) ) )


    for func in apiInfo.funcs:
        ioStream.write( formatter.anchor( func.__name__ ) )
        ioStream.write( formatter.header3( func.__name__ ) )
        if func.__doc__ != None:
            ioStream.write( formatter.escapeMarkup( func.__doc__) + formatter.endl() )


    for cls in apiInfo.classes:
        ioStream.write( formatter.anchor( cls.__name__ ) )
        ioStream.write( formatter.header3( cls.__name__ ) )	
        if cls.__doc__ != None:
            ioStream.write( formatter.escapeMarkup( cls.__doc__)  + formatter.endl() )

        for m in cls.methods:
            if m.__doc__ != None:
                ioStream.write( formatter.bulletItem( formatter.escapeMarkup( m.__name__ ) ) )


        for m in cls.methods:
            if m.__doc__ != None:
                ioStream.write( formatter.header4( formatter.escapeMarkup( m.__name__ ) ) )
                ioStream.write( formatter.escapeMarkup( m.__doc__ ) + formatter.endl() )


def main():

    if len(sys.argv) < 2:
        usage()
        return

    moduleName = sys.argv[1]

    try:

        module = __import__( moduleName )

        with file( "wiki.txt", "w" ) as wikiIo:

            apiInfo = ModuleInfo( module )

            formatter = CodeplexFormatter()

            buildDoc( wikiIo, formatter, apiInfo )         

    except ImportWarning:

        print "failed to import module " + moduleName
	

if __name__ == "__main__":
    main()
