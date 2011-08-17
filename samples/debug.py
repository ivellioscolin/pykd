#
#
#

from PySide.QtCore import *
from PySide.QtGui import *  

import pykd


class UpdateEvent( QEvent ):
    def __init__(self):
        QEvent.__init__(self, QEvent.Type(QEvent.User + 1))


class GoThread( QThread ):

    def __init__(self, func):
        QThread.__init__(self)
        self.start()
        self.func = func
    
    def run(self):
        self.func()
        app.postEvent( mainForm, UpdateEvent() ) 
        self.exit()


class DisasmWidget( QDockWidget ):

    def __init__( self ):
        QDockWidget.__init__( self )
        self.setWindowTitle( "Disassembler" )
        self.textArea = QTextEdit()
        self.textArea.setReadOnly( True )
        self.setWidget( self.textArea )
        

    def onUpdate( self ):

        disasm = pykd.disasm()        
        disasmStr = disasm.instruction() + "\n"
        for i in xrange(50):
            disasmStr += disasm.next() + "\n"

        self.textArea.setPlainText( disasmStr )


class RegistersWidget( QDockWidget ):

    def __init__( self ):
        QDockWidget.__init__( self )
        self.setWindowTitle( "Registers" )
        self.textArea = QTextEdit()
        self.setWidget( self.textArea )
                             
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


class StackWidget( QDockWidget ):

    def __init__( self ):
        QDockWidget.__init__( self )
        self.setWindowTitle( "Stack" )
        self.textArea = QTextEdit()
        self.setWidget( self.textArea )

    def onUpdate( self ):

        s = ""

        stackFrames = pykd.getCurrentStack()
        for frame in stackFrames: 
            s += pykd.findSymbol( frame.instructionOffset ) + "  (%x)" % frame.instructionOffset  + "\n"
       
        self.textArea.setPlainText( s )   


class MainForm( QMainWindow ):

    updated = Signal()
    
    def __init__( self ):


        QMainWindow.__init__( self, None )            
        self.setWindowTitle("Pykd Debugger Sample")      
        self.setDockNestingEnabled( True )
        
        self.goThread = None


        fileMenu = QMenu( "&File" )
        fileMenu.addAction( "Open process...",  self.onOpenProcess )
        fileMenu.addAction( "Exit", self.onExit )
        self.menuBar().addMenu( fileMenu )


        debugMenu = QMenu( "Debug" )
        debugMenu.addAction( "Break", self.onBreak )
        debugMenu.addAction( "Go", self.onGo )
        debugMenu.addAction( "Step", self.onStep )
        self.menuBar().addMenu( debugMenu )


        viewMenu = QMenu( "View" )
        viewMenu.addAction( "Disasm", self.onDisasmShow )
        viewMenu.addAction( "Regsiters", self.onRegistersShow )
        viewMenu.addAction( "Stack", self.onStackShow )
        self.menuBar().addMenu( viewMenu )

        self.disasmWidget = DisasmWidget() 
        self.disasmWidget.setVisible( False )
        self.addDockWidget( Qt.LeftDockWidgetArea, self.disasmWidget ) 
        self.updated.connect(self.disasmWidget.onUpdate ) 

        self.registersWidget = RegistersWidget()
        self.registersWidget.setVisible( False )
        self.addDockWidget( Qt.LeftDockWidgetArea, self.registersWidget ) 
        self.updated.connect(self.registersWidget.onUpdate ) 
                          
        self.stackWidget = StackWidget()
        self.stackWidget.setVisible( False )
        self.addDockWidget( Qt.LeftDockWidgetArea, self.stackWidget )
        self.updated.connect(self.stackWidget.onUpdate )      

        self.resize( 800, 600 )

    

    def stopGoThread( self ):
 
        if self.goThread != None and self.goThread.isRunning():
            self.goThread.quit()
            self.goThread.wait(10000)
   

    def event( self, ev ):
        
        if ev.type() == QEvent.User + 1:
            self.onUpdate()
            return True
        else:
            return QMainWindow.event( self, ev )        
   


    def onOpenProcess( self ):
	fileDlg = QFileDialog( self )
        fileDlg.setNameFilter( self.tr("Executable (*.exe)" ) )

        pykd.startProcess( fileDlg.getOpenFileName()[0] )

        self.goThread = GoThread( pykd.go )


    def onBreak( self ):
        pykd.breakin()

    def onGo( self ):
        self.stopGoThread()
        self.goThread = GoThread( pykd.go ) 

    def onStep( self ):
        self.stopGoThread()
        self.goThread = GoThread( pykd.step )

    def onExit( self ):
        self.close()

    def onDisasmShow( self ):
        self.disasmWidget.setVisible( not self.disasmWidget.isVisible() )

    def onRegistersShow( self ):
        self.registersWidget.setVisible( not self.registersWidget.isVisible() )


    def onStackShow( self ):
        self.stackWidget.setVisible( not self.stackWidget.isVisible() )

    def onUpdate( self ):
        self.updated.emit()


def main():      

    global app
    global mainForm

    app = QApplication( [] )
    mainForm = MainForm()
    mainForm.show()
    exitres = app.exec_()

if __name__ == "__main__":
    main()