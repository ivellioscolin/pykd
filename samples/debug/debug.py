#
#
#

from PySide.QtCore import *
from PySide.QtGui import *  

import pykd
import widgets

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


class MainForm( QMainWindow ):

    updated = Signal()
    
    def __init__( self ):


        QMainWindow.__init__( self, None )            
        self.setWindowTitle("Pykd Debugger Sample")      
        self.setDockNestingEnabled( True )
        
        self.goThread = None


        self.fileMenu = QMenu( "&File" )
        self.fileMenu.addAction( "Open process...",  self.onOpenProcess )
        self.fileMenu.addAction( "Exit", self.onExit )
        self.menuBar().addMenu( self.fileMenu )


        self.debugMenu = QMenu( "Debug" )
        self.debugMenu.addAction( "Break", self.onBreak )
        self.debugMenu.addAction( "Go", self.onGo )
        self.debugMenu.addAction( "Step", self.onStep )
        self.menuBar().addMenu( self.debugMenu )


        self.viewMenu = QMenu( "View" )
        self.menuBar().addMenu( self.viewMenu )

        self.resize( 800, 600 )

        widgets.register( self )
    

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