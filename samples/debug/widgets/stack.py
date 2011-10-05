
from PySide.QtCore import *
from PySide.QtGui import *  

import pykd


class StackWidget( QDockWidget ):

    def __init__( self, parent ):
        QDockWidget.__init__( self )
        self.setWindowTitle( "Stack" )
        self.textArea = QTextEdit()
        self.setWidget( self.textArea )
        self.setVisible( False )
        parent.addDockWidget( Qt.LeftDockWidgetArea, self ) 
        parent.updated.connect(self.onUpdate ) 
        parent.viewMenu.addAction( "Stack", self.onStackShow )

                             
    def onUpdate( self ):
        s = ""

        stackFrames = pykd.getCurrentStack()
        for frame in stackFrames: 
            s += pykd.findSymbol( frame.instructionOffset ) + "  (%x)" % frame.instructionOffset  + "\n"
       
        self.textArea.setPlainText( s )   


    def onStackShow( self ):
        self.setVisible( not self.isVisible() )
