#
#
#

from PySide.QtCore import *
from PySide.QtGui import *      

import pykd


class DbgThread( QThread ):

    def __init__(self):
        QThread.__init__(self)
    
    def run(self):
        pykd.startProcess( self.processName )
        pykd.go()
        #    pass         
        while True:
            self.sleep(100)


        self.exit()


class MainForm( QMainWindow ):
    
    def __init__( self ):

        QMainWindow.__init__( self, None )            
        self.setWindowTitle("Pykd Debugger Sample")      

        fileMenu = QMenu( "&File" )
        fileMenu.addAction( "Open process...",  self.onOpenProcess )
        fileMenu.addAction( "Exit", self.onExit )

        self.menuBar().addMenu( fileMenu )

        self.resize( 800, 600 )

    
    def __del__( self ):

        if self.debugger.isRunning():
            self.debugger.quit()
            self.debugger.wait(1000)
            self.debugger.terminate()


    def onOpenProcess( self ):
	fileDlg = QFileDialog( self )
        fileDlg.setNameFilter( self.tr("Executable (*.exe)" ) )

        self.debugger = DbgThread()
        self.debugger.processName = fileDlg.getOpenFileName()[0]
        self.debugger.start()


    def onExit( self ):
        self.close()


def main():      

    app = QApplication( [] )
    mainForm = MainForm()
    mainForm.show()
    exitres = app.exec_()

if __name__ == "__main__":
    main()