
from PySide.QtCore import *
from PySide.QtGui import *


from dbgcore import DbgCore
from settings import settings

class MainForm(QMainWindow):

    def __init__(self):
        QMainWindow.__init__(self, None)
        self.resize( 800, 600 )
        self.setWindowTitle("Pykd Debugger Sample")
        self.setDockNestingEnabled( True )

        self.dbgCore = DbgCore()

        self.fileMenu = QMenu( "&File" )
        self.menuBar().addMenu( self.fileMenu )

        self.viewMenu = QMenu( "View" )
        self.menuBar().addMenu( self.viewMenu )

        self.widgets = settings["default"](self.dbgCore, self )

        self.fileMenu.addAction( "Exit", self.onExit )
 
    def onExit(self):
        self.dbgCore.close()
        self.close()

def main():      
    app = QApplication( [] )
    mainForm = MainForm()
    mainForm.show()
    exitres = app.exec_()

if __name__ == "__main__":
    main()