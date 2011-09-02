"""
Exception watchdog
"""

from pykd import *
import sys

# known exception codes
knownExcepCodes = {
  0xc0000005 : "EXCEPTION_ACCESS_VIOLATION",
  0x80000002 : "EXCEPTION_DATATYPE_MISALIGNMENT",
  0x80000003 : "EXCEPTION_BREAKPOINT",
  0x80000004 : "EXCEPTION_SINGLE_STEP",
  0xc000008c : "EXCEPTION_ARRAY_BOUNDS_EXCEEDED",
  0xc000008d : "EXCEPTION_FLT_DENORMAL_OPERAND",
  0xc000008e : "EXCEPTION_FLT_DIVIDE_BY_ZERO",
  0xc000008f : "EXCEPTION_FLT_INEXACT_RESULT",
  0xc0000090 : "EXCEPTION_FLT_INVALID_OPERATION",
  0xc0000091 : "EXCEPTION_FLT_OVERFLOW",
  0xc0000092 : "EXCEPTION_FLT_STACK_CHECK",
  0xc0000093 : "EXCEPTION_FLT_UNDERFLOW",
  0xc0000094 : "EXCEPTION_INT_DIVIDE_BY_ZERO",
  0xc0000095 : "EXCEPTION_INT_OVERFLOW",
  0xc0000096 : "EXCEPTION_PRIV_INSTRUCTION",
  0xc0000006 : "EXCEPTION_IN_PAGE_ERROR",
  0xc000001d : "EXCEPTION_ILLEGAL_INSTRUCTION",
  0xc0000025 : "EXCEPTION_NONCONTINUABLE_EXCEPTION",
  0xc00000fd : "EXCEPTION_STACK_OVERFLOW",
  0xc0000026 : "EXCEPTION_INVALID_DISPOSITION",
  0x80000001 : "EXCEPTION_GUARD_PAGE",
  0xc0000008 : "EXCEPTION_INVALID_HANDLE",
  0xc0000194 : "EXCEPTION_POSSIBLE_DEADLOCK",
  0xc000013a : "CONTROL_C_EXIT"
};


class ExceptionHandler(debugEvent):

  def onException(self, exceptData):
    dprintln("\n *** shit happens")

    exceptCode = exceptData["Code"]
    dprint("Exception code    : ")
    if exceptCode in knownExcepCodes:
      dprintln( knownExcepCodes[exceptCode] )
    else:
      dprintln( "0x%08x" % exceptCode )

    dprint("Exception flags   : ")
    exceptFlags = exceptData["Flags"]
    if exceptFlags:
      if exceptFlags & NONCONTINUABLE_EXCEPTION:
        exceptFlags &= ~NONCONTINUABLE_EXCEPTION
        dprint( "NONCONTINUABLE " )
      if exceptFlags:
        dprintln( "| 0x%02x" % exceptFlags)
      else:
        dprintln( "" )
    else:
      dprintln( "0" )

    dprintln("Exception record  : 0x%X" % exceptData["Record"])

    exceptAddr = exceptData["Address"]
    dprintln("\nException address : 0x%X" % exceptAddr)
    dprintln( dbgCommand("ln 0x%X" % exceptAddr) )

    if len( exceptData["Parameters"] ):
      dprintln("Parameters        : ")
      for param in exceptData["Parameters"]:
        dprintln("\t0x%X" % param)

    dprintln("\nFirst chance      : " + str( exceptData["FirstChance"] ))

    dbgCommand( ".reload" )
    dprintln( "\n " + dbgCommand( "r" ) )
    dprintln( dbgCommand( "kb" ) )

    return DEBUG_STATUS_BREAK

if __name__ == '__main__':

  if len(sys.argv) != 1:
    startComamnd = ""
    for i in range(1, len(sys.argv)):
      startComamnd += sys.argv[i] + " "
    startProcess(startComamnd)

  exceptionHandler = ExceptionHandler()

  try:
    go()

  except WaitEventException:
    dprintln("none of the targets could generate events")


