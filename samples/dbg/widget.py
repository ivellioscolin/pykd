
from PySide.QtCore import *
from PySide.QtGui import *

class BaseWidget( QDockWidget ):

    def __init__( self, dbgCore, mainWindow, title = "", visible = False ):
        QDockWidget.__init__( self )
        self.setWindowTitle( title )
        self.setVisible( visible )
        mainWindow.addDockWidget( Qt.LeftDockWidgetArea, self )

        self.dbgCore=dbgCore
        self.mainWnd = mainWindow
        dbgCore.targetBreak.connect( self.onDbgBreak )
        dbgCore.targetRunning.connect( self.onDbgRun )
        dbgCore.targetAttached.connect( self.onDbgAttach )
        dbgCore.targetDetached.connect( self.onDbgDetach )    

    def addMenuTriggerAction( self, actionName ):
        self.action = QAction( actionName, self.mainWnd )
        self.action.triggered.connect(self.onTriggerAction) 
        self.action.setDisabled( True )
        self.mainWnd.viewMenu.addAction(self.action)

    def onDbgBreak(self):
        pass

    def onDbgRun(self):
        pass

    def onDbgAttach(self):
        pass

    def onDbgDetach(self):
        pass

    def onTriggerAction(self):
        self.setVisible( not self.isVisible() )


class DebugWidget( BaseWidget ):

    def __init__( self, dbgCore, mainWindow, title = "", visible = False ):
        BaseWidget.__init__( self, dbgCore, mainWindow, title, visible )
        self.action = None

    def onDbgAttach(self):
        if self.action != None:
            self.action.setDisabled( True )
        self.setDisabled( True )

    def onDbgDetach(self):
        if self.action != None:
            self.action.setDisabled( True )
        self.setDisabled( True )

    def onDbgBreak(self):
        if self.action != None:
            self.action.setDisabled( False )
        self.setDisabled( False )
        self.updateView()

    def onDbgRun(self):
        if self.action != None:
            self.action.setDisabled( True )
        self.setDisabled( True )

    def updateView(self):
        pass


class BaseController(QObject):

    def __init__(self,dbgCore,mainWindow):
        QObject.__init__(self,mainWindow)
        self.dbgCore=dbgCore
        self.mainWnd = mainWindow
        dbgCore.targetBreak.connect( self.onDbgBreak )
        dbgCore.targetRunning.connect( self.onDbgRun )
        dbgCore.targetAttached.connect( self.onDbgAttach )
        dbgCore.targetDetached.connect( self.onDbgDetach )    

    def onDbgBreak(self):
        self.onStateChange()

    def onDbgRun(self):
        self.onStateChange()

    def onDbgAttach(self):
        self.onStateChange()

    def onDbgDetach(self):
        self.onStateChange()

    def onStateChange(self):
        pass