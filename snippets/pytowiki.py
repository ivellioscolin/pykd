#
#
#

import sys


def usage():
    pass


class CodeplexFormatter:

    def header1( self, s ):
        return "! " + s + "\n"

    def header2( self, s ):
        return "!! " + s + "\n"

    def header3( self, s ):
        return "!!! " + s + "\n"

    def bulletItem( self, s ):
        return "* " + s + "\n"


class ModuleInfo:

    def __init__ (self, module):
        self.funcs = sorted( [ item for item in module.__dict__.values() if type(item).__name__ == "function" ], key=lambda x: x.__name__ ) 
        self.classes = sorted( [ item for item in module.__dict__.values() if type(item).__name__ == "class" ],  key=lambda x: x.__name__ ) 


def buildDoc( ioStream, formatter, apiInfo ):
  
    ioStream.write( formatter.header2( "Functions" ) )

    for func in apiInfo.funcs:
        ioStream.write( formatter.bulletItem( func.__name__ ) )

    ioStream.write( formatter.header2( "Classes" ) )

    for cls in apiInfo.classes:
        ioStream.write( formatter.bulletItem( cls.__name__ ) )


    for func in apiInfo.funcs:
        ioStream.write( formatter.header3( func.__name__ ) )
        ioStream.write( func.__doc__ + "\n" )


    for cls in apiInfo.classes:
        ioStream.write( formatter.header3( cls.__name__ ) )	
        ioStream.write( cls.__doc__ + "\n" )



def buildFuncDir( ioStream, funcs ):
    pass

def buildClassDir( ioStream, classes ):
    pass


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
