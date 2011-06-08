"""
Print list of all drivers
"""

from pykd import *
import ntobj


if __name__ == "__main__":

  def printDriverFromDir(dirName):
    lstDrvs = ntobj.getListByDirectoryObject( 
      ntobj.getObjectByName(dirName), ptrPtr( getOffset("nt", "IoDriverObjectType") )
    )
    for drv in lstDrvs:
      cmdDriver = "\t<link cmd=\"!drvobj 0x%x 7\">0x%x</link>" % (drv, drv)
      dprintln(dirName + "\\" + ntobj.getObjectName(drv) + cmdDriver, True )


  if not isKernelDebugging:
    dprintln( "Script for kernel mode only" )
  else:
    printDriverFromDir("\\Driver")
    printDriverFromDir("\\FileSystem")


