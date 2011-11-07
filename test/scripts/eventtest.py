
import unittest
import target
import pykd

class handler( pykd.eventHandler ):

    def __init__(self):
        pykd.eventHandler.__init__(self)
        self.counter=0

    def onException(self, param):
        self.counter += 1
        return pykd.DEBUG_STATUS_NO_CHANGE

class EventTest( unittest.TestCase ):

    def testDebugBreak( self ):
         h = handler()
         pykd.go()
         pykd.go()
         self.assertEqual( 2, h.counter )
