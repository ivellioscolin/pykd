# 
# Print content of object table (handle table)
# 

from pykd import *
import ntobj
import sys


if __name__ == "__main__":

  if not isSessionStart():
    print "Script is launch out of WinDBG"
    quit(0)

  if (2 != len(sys.argv)):
    dprintln("Invalid command line")
    dprintln("Usage: " + sys.argv[0] + " <ObjectTableAddr>")
  else:
    lstObejcts = ntobj.getListByHandleTable(int(sys.argv[1], 16))
    dprintln("%u objects:" % len(lstObejcts))
    for obj in lstObejcts:
      dprintln("obj: %X" % obj + ", type:%X" % ntobj.getType(obj))
