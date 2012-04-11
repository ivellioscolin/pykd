
import pykd

from PySide.QtCore import QThread
from PySide.QtCore import QObject
from PySide.QtCore import Signal

class DbgThread( QThread ):

    def __init__(self, func):
        QThread.__init__(self)
        self.func = func
    
    def run(self):
        self.func()
        self.exit()

class DbgCore( QObject ):

    targetBreak = Signal()
    targetRunning = Signal()
    targetAttached = Signal()
    targetDetached = Signal()
  
    def close(self):
        if self.processOpened:
            if self.thread != None:
               self.breakin()

    def openProcess( self, name ):
        pykd.startProcess( name ) 
        self.processOpened = True
        self.targetAttached.emit()
        self.targetBreak.emit()

    def detachProcess(self):
        pykd.detachProcess()
        self.processOpened = False
        self.targetDetached.emit()

    def killProcess(self):
        pykd.killProcess()
        self.processOpened = False
        self.targetDetached.emit()


    def breakin( self ):
        pykd.breakin()	    

    def go( self ):
        self.thread = DbgThread( pykd.go )
        self.thread.finished.connect( self.onDebugStop )
        self.targetRunning.emit()
        self.thread.start()      

    def step( self ):
        self.thread = DbgThread( pykd.step )
        self.thread.finished.connect( self.onDebugStop )
        self.targetRunning.emit()
        self.thread.start()     
 
    def onDebugStop(self):
        self.thread.wait(100)
        self.thread = None
        self.targetBreak.emit()

    def __init__(self):
        QObject.__init__(self)
        self.thread = None 
        self.processOpened = False