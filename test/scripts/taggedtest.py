import unittest
import pykd
import sys
import os

class TaggedTest(unittest.TestCase):
    def setUp(self):
        dump_file = os.path.join( os.path.dirname(sys.argv[0]),
                                  r"..\..\kdlibcpp\kdlib\tests\dumps\win8_x64_mem\win8_x64_mem.cab" )
        self.dump_id = pykd.loadDump( dump_file )

        self._existing_id = "D03DC06F-D88E-44C5-BA2A-FAE035172D19"
        self._non_existing_id = "88597A32-1493-41CA-BF87-2A950DF4CEE0"

    def tearDown(self):
        pykd.closeDump( self.dump_id )

    def testEnum(self):
        ids = pykd.enumTagged()

        self.assertTrue( self._existing_id.lower() in [i.lower() for i in ids] )

        self.assertFalse( self._non_existing_id.lower() in [i.lower() for i in ids] )


    def testLoadBuffer(self):
        buff = pykd.loadTaggedBuffer( self._existing_id )
        self.assertEqual( len(buff), 0x410 )

        self.assertRaises( pykd.DbgException, pykd.loadTaggedBuffer, self._non_existing_id )

