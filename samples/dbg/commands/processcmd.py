
from widget import *

class ProcessController(BaseController):
     
    def __init__(self, dbgCore, mainWindow):
        BaseController.__init__(self,dbgCore,mainWindow)

        self.openProcessAction = QAction( "Open process...", mainWindow.fileMenu )
        self.openProcessAction.triggered.connect(self.onOpenProcess) 
        mainWindow.fileMenu.addAction(self.openProcessAction)

        self.detachProcessAction = QAction( "Detach process", mainWindow.fileMenu )
        self.detachProcessAction.triggered.connect(self.onDetachProcess)
        self.detachProcessAction.setDisabled(True)
        mainWindow.fileMenu.addAction(self.detachProcessAction)

    def onOpenProcess(self):
       	fileDlg = QFileDialog( self.mainWnd )
        fileDlg.setNameFilter( "Executable (*.exe)" )
        self.dbgCore.openProcess( fileDlg.getOpenFileName()[0] ) 

    def onDetachProcess(self):
        self.dbgCore.detachProcess()     

    def onDbgAttach(self):
        self.openProcessAction.setDisabled(True)
        self.detachProcessAction.setDisabled(True)

    def onDbgDetach(self):
        self.openProcessAction.setDisabled(False)
        self.detachProcessAction.setDisabled(True)

    def onDbgBreak(self):
        self.detachProcessAction.setDisabled(False)

    def onDbgRun(self):
        self.detachProcessAction.setDisabled(True)