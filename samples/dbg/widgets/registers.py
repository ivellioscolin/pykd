from widget import *
import pykd

class RegisterWidget( DebugWidget ):

    def __init__(self, dbgCore, mainWindow, visible = False ):
        BaseWidget.__init__( self, dbgCore, mainWindow, "Registers", visible )

        self.addMenuTriggerAction( "Registers" )

        self.textArea = QTextEdit()
        self.setWidget( self.textArea )

    def updateView(self):

        s = ""
        
        try:
            i = 0
            while True:
                reg = pykd.reg(i)
                #print str(reg)
                s += "%s    %x ( %d )\r\n" % ( reg.name(), long(reg), long(reg) )
                i += 1

        except pykd.BaseException:
            pass

        self.textArea.setPlainText( s )   




             



  
 


