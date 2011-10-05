
from PySide.QtCore import *
from PySide.QtGui import *  

import pykd


class DisasmWidget( QDockWidget ):

    def __init__( self, parent ):
        QDockWidget.__init__( self )
        self.setWindowTitle( "Disassembler" )
        self.textArea = QTextEdit()
        self.setWidget( self.textArea )
        self.setVisible( False )
        parent.addDockWidget( Qt.LeftDockWidgetArea, self ) 
        parent.updated.connect(self.onUpdate ) 
        parent.viewMenu.addAction( "Disassembler", self.onDisassemblerShow )

                             
    def onUpdate( self ):
        disasm = pykd.disasm()        
        self.textArea.setPlainText( "\n".join( [ disasm.disasm() for i in xrange(50) ] ) )


    def onDisassemblerShow( self ):
        self.setVisible( not self.isVisible() )
        