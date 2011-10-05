

from PySide.QtCore import *
from PySide.QtGui import *  

import pykd


class CmdWidget( QDockWidget ):

    def __init__( self, parent ):
        QDockWidget.__init__( self )
        self.setWindowTitle( "Commands" )

        self.inCmd = QLineEdit()
        self.inCmd.returnPressed.connect( self.onCommandInput )

        self.outCmd = QTextEdit()
	self.outCmd.setReadOnly( True )


        layout = QVBoxLayout()
        layout.addWidget( self.outCmd )
        layout.addWidget( self.inCmd )



        layoutWidget = QWidget()
        layoutWidget.setLayout( layout )

        self.setWidget( layoutWidget )

        self.setVisible( False )
        parent.addDockWidget( Qt.LeftDockWidgetArea, self ) 
        parent.viewMenu.addAction( "Commands", self.onCommandsShow )

                             
    def onCommandsShow( self ):
        self.setVisible( not self.isVisible() )

   
    def onCommandInput( self ):
	self.inCmd.setReadOnly( True )
        cmdLine = self.inCmd.text()
	self.inCmd.setText( "running %s" % cmdLine )
        self.outCmd.append( pykd.dbgCommand( cmdLine ) )
	self.inCmd.setText( "" )
	self.inCmd.setReadOnly( False )
         
  
