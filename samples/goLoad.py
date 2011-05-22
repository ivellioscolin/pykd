"""
Wait (execute) for load target module
"""

from pykd import *
import fnmatch
import sys

class modLoad(modEvents):
  def __init__(self, mask):
    modEvents.__init__(self)
    self.mask = mask

  def onLoad(self, module):
    if fnmatch.fnmatch( module.name(), self.mask ):
      return DEBUG_STATUS_BREAK
    return DEBUG_STATUS_NO_CHANGE

if __name__ == "__main__":
  if len(sys.argv) == 2:
    loadHandler = modLoad( sys.argv[1] )
    go()
  else:
    dprintln( "Wait (execute) for load target module\nInvalid command line" )
    dprintln( "Using" + sys.argv[0] + " <MOD_FILE_NAME>" )
    dprintln( "\tMOD_FILE_NAME - name of target module with wildcard" )
