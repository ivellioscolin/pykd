#
#
#

import sys


def usage():
    print "python pytowiki.py module_name output_file"


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
        
class MarkdownFormatter:

    def endl( self ):
        return "\n"

    def header1( self, s ):
        return "# " + s + self.endl()

    def header2( self, s ):
        return "## " + s + self.endl()

    def header3( self, s ):
        return "### " + s + self.endl()

    def header4( self, s ):
        return "#### " + s + self.endl()

    def bulletItem( self, s ):
        return "* " + s + self.endl()

    def escapeMarkup( self, s ):
        return self.endl() + "```"  + self.endl() + s + self.endl() + "```" + self.endl()

    def link( self, text, link ):
        return "[" + text + "](#" + link + ")"

    def anchor( self, link ):
        return "<a name=\"" + link + "\"></a>"  + self.endl()
        
    def escapeSymbols(self, text):
        output = ""
        escapeSyms = ['_'] 
        for s in text:
            if s in escapeSyms:
                output += "\\" + s
            else:
                output += s
        return output
           



class ModuleInfo:

    def __init__ (self, module):
        self.funcs = sorted( [ item for item in module.__dict__.values() if type(item).__name__ == "function" ], key=lambda x: x.__name__ ) 
        self.classes = sorted( [ item for item in module.__dict__.values() if type(item).__name__ == "class" or type(item).__name__ == "type" ],  key=lambda x: x.__name__ )

        for cls in  self.classes:
            cls.methods = sorted( [ item for item in cls.__dict__.values() if type(item).__name__ == "function" ], key=lambda x: x.__name__ ) 
            cls.properties = sorted( [ item for item in cls.__dict__.items() if type(item[1]).__name__ == "property" ], key=lambda x: x[0] )
            cls.enum = cls.__dict__.get("names", [] )


def buildDoc( ioStream, formatter, apiInfo ):
  
    ioStream.write( formatter.header2( "Functions" ) )

    for func in apiInfo.funcs:
        ioStream.write( formatter.bulletItem( formatter.link( func.__name__, func.__name__ ) ) )
        
    ioStream.write( formatter.endl() )     

    ioStream.write( formatter.header2( "Classes" ) )

    for cls in apiInfo.classes:
        ioStream.write( formatter.bulletItem( formatter.link( cls.__name__, cls.__name__ ) ) )

    ioStream.write( formatter.endl() )     

    for func in apiInfo.funcs:
        ioStream.write( formatter.anchor( func.__name__ ) )
        ioStream.write( formatter.header3( "Function " +  func.__name__ ) )
        if func.__doc__ != None:
            ioStream.write( formatter.escapeMarkup( func.__doc__) + formatter.endl() )


    for cls in apiInfo.classes:
        ioStream.write( formatter.anchor( cls.__name__ ) )
        ioStream.write( formatter.header3( "Class " + cls.__name__ ) )
        if cls.__doc__ != None:
            ioStream.write( formatter.escapeMarkup( cls.__doc__)  + formatter.endl() )

        ioStream.write( formatter.header4( "Base classes:") )
        for b in cls.__bases__:
            if b in apiInfo.classes:
                ioStream.write( formatter.link( b.__name__,  b.__name__ ) + formatter.endl() )
            else:
                ioStream.write( b.__name__ + formatter.endl() )

        if cls.properties:
            ioStream.write( formatter.header4( "Properties:") )
            for p in cls.properties:  
                ioStream.write( formatter.bulletItem( formatter.link( p[0],  cls.__name__ + "." + p[0]) ) )
            ioStream.write( formatter.endl() )

        methods = filter( lambda m: m.__doc__ != None, cls.methods )
            
        if methods:
            ioStream.write( formatter.header4( "Methods:") )
            for m in methods:
               ioStream.write( formatter.bulletItem( formatter.link( formatter.escapeSymbols(m.__name__), cls.__name__ + "." + m.__name__)) )
            ioStream.write( formatter.endl() )

        if cls.enum:
            ioStream.write( formatter.header4( "Values:") )
            for v in cls.enum.items():
                ioStream.write( formatter.bulletItem( "%s: %d" % v ) )
            ioStream.write( formatter.endl() )
        
        if cls.properties:
            for p in cls.properties:  
                if p[1].__doc__ != None:
                    ioStream.write( formatter.anchor( cls.__name__ + "." + p[0] ) )
                    ioStream.write( formatter.header4( "Property  " + cls.__name__  + "."  +  p[0] ) )
                    ioStream.write( formatter.escapeMarkup( p[1].__doc__ ) + formatter.endl() )

        if methods:
            for m in methods:
                ioStream.write( formatter.anchor( cls.__name__ + "." + m.__name__ ) )
                ioStream.write( formatter.header4( formatter.escapeSymbols("Method  " + cls.__name__  + "."  + m.__name__ )) )
                ioStream.write( formatter.escapeMarkup( m.__doc__ ) + formatter.endl() )

def main():

    if len(sys.argv) < 3:
        usage()
        return

    moduleName = sys.argv[1]
    fileName = sys.argv[2]

    try:

        module = __import__( moduleName )

        with file( fileName, "w" ) as wikiIo:

            apiInfo = ModuleInfo( module )

            #formatter = CodeplexFormatter()
            formatter = MarkdownFormatter()

            buildDoc( wikiIo, formatter, apiInfo )

    except ImportWarning:

        print "failed to import module " + moduleName


if __name__ == "__main__":
    main()
