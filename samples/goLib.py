"""
Using bp class without callback
"""

from pykd import *


if __name__ == "__main__":

  if not isKernelDebugging():

    if not isWindbgExt():
      startProcess("calc.exe")

    kernel32 = loadModule("kernel32")

    bpA = bp( kernel32.LoadLibraryA )
    bpW = bp( kernel32.LoadLibraryW )

    go()
    dbgCommand("gu")

    dprintln( dbgCommand("!dlls @$retreg") )

  else:

    dprintln("Script for user mode only")







