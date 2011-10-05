

from PySide.QtCore import *
from PySide.QtGui import *  

import pykd


class RegistersWidget( QDockWidget ):

    def __init__( self, parent ):
        QDockWidget.__init__( self )
        self.setWindowTitle( "Registers" )
        self.textArea = QTextEdit()
        self.setWidget( self.textArea )
        self.setVisible( False )
        parent.addDockWidget( Qt.LeftDockWidgetArea, self ) 
        parent.updated.connect(self.onUpdate ) 
        parent.viewMenu.addAction( "Regsiters", self.onRegistersShow )

                             
    def onUpdate( self ):

        s = ""

        for reg in self.getRegisterSet():
            s += "%s    %x ( %d )\r\n" % ( reg.name(), reg.value(), reg.value() )

        self.textArea.setPlainText( s )   


    def getRegisterSet(self):
        regSet=[]
        try:
            i = 0
            while True:
                reg = pykd.cpuReg(i)
                regSet.append(reg)
                i += 1

        except pykd.BaseException:
            pass

        return regSet

    def onRegistersShow( self ):
        self.setVisible( not self.isVisible() )
  
