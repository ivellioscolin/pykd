from widget import *

class DebuggerController(BaseController):

    def __init__(self,dbgCore,mainWindow):
        BaseController.__init__(self,dbgCore,mainWindow)
        debugMenu = QMenu( "Debug" )

        self.breakAction = QAction("Break", debugMenu )
        self.breakAction.triggered.connect( self.onBreak )
        self.breakAction.setDisabled(True)
        debugMenu.addAction( self.breakAction ) 

        self.goAction = QAction("Go", debugMenu )
        self.goAction.triggered.connect( self.onGo )
        self.goAction.setDisabled(True)
        debugMenu.addAction( self.goAction ) 

        self.stepAction = QAction("Step", debugMenu )
        self.stepAction.triggered.connect( self.onStep )
        self.stepAction.setDisabled(True)
        debugMenu.addAction( self.stepAction )         

        mainWindow.menuBar().addMenu( debugMenu )

    def onBreak( self ):
        self.dbgCore.breakin()

    def onGo( self ):
        self.dbgCore.go()

    def onStep( self ):
        self.dbgCore.step()

    def onDbgBreak(self):
        self.breakAction.setDisabled(True)
        self.goAction.setDisabled(False)
        self.stepAction.setDisabled(False)

    def onDbgRun(self):
        self.breakAction.setDisabled(False)
        self.goAction.setDisabled(True)
        self.stepAction.setDisabled(True)

    def onDbgAttach(self):
        self.breakAction.setDisabled(False)
        self.goAction.setDisabled(False)
        self.stepAction.setDisabled(False)

    def onDbgDetach(self):
        self.breakAction.setDisabled(False)
        self.goAction.setDisabled(False)
        self.stepAction.setDisabled(False)

