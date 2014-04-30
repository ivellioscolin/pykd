
import pykd
from pykd import *
from optparse import OptionParser
from fnmatch import fnmatch
import traceback
import sys

nt = None
EPROCESS = None
ETHREAD = None

def setupGlobalObject():

    global nt, EPROCESS, ETHREAD

    try:
        nt = module("nt")
        EPROCESS = nt.type("_EPROCESS")
        ETHREAD = nt.type("_ETHREAD")
    except DbgException:
        dprintln("check symbol paths")

     

class PrintOptions:
    def __init__(self):
        self.ignoreNotActiveThread = True
        self.ignoreNotActiveProcess = True
        self.showWow64stack = True
        self.showIP = True
        self.showSP = True
        self.showUnique = False
        self.combineWow64 = True
        self.showCpu = True
    
def printFrame(frame, printopt):

    str = ""

    if printopt.showCpu:
        str += frame.cpuType + "\t"

    if printopt.showIP:
        str += "%016x\t" % frame.instructionOffset
    if printopt.showSP:
        str += "%016x\t" % frame.stackOffset
    
    str +=  findSymbol( frame.instructionOffset )

    dprintln(str)


def getModule(offset):
    try:
        return module(offset)
    except DbgException:
        pass
    return None

def getStackHash(stk):
    hashStr = ""
    for frame in stk:
        hashStr += str(frame.instructionOffset)
    return hash(hashStr)

def getStackModules(stk):
    modules = []
    for frame in stk:
        m = getModule(frame.instructionOffset)
        if m and not ( m in modules ):
            modules.append( m )
    return modules

def getStackSymbols(stk):
    symbols = []
    for frame in stk:
        sym = findSymbol( frame.instructionOffset, showDisplacement = False )
        if sym and not ( sym in symbols ):
            symbols.append(sym)
    return symbols


def printThread(thread, process):
    dprintln("")
    dprintln( "Thread %x, Process: %s (%x)" % ( thread, loadCStr( process.ImageFileName ), process )  )


def printProcess(process,processFilter,threadFilter,moduleFilter,funcFilter,printopt):

    processName = loadCStr( process.ImageFileName )

    if processFilter and not processFilter(process, process.UniqueProcessId, processName ):
        return

    dprintln( "Process %x" %  process )
    dprintln( "Name: %s  Pid: %#x" %  ( processName, process.UniqueProcessId ) )
    dprintln( "" )

    try:

        dbgCommand(".process /p /r %x" % process )
        dbgCommand( ".reload /user" )   

        threadLst = typedVarList(process.ThreadListHead, ETHREAD, "ThreadListEntry.Flink")

        stackHashes = set()

        for thread in threadLst:

            if threadFilter and not threadFilter( thread.Tcb, thread.Cid.UniqueThread ):
                continue

            setCurrentThread( thread )

            stkNative = getStack()
            stkWow64 = []

            if printopt.showWow64stack == True:
                try:

                    switchCPUMode();

                    try:
                        stkWow64 = getStack()

                    except MemoryException:
                        pass

                    switchCPUMode();

                except DbgException:
                    pass

            
            stk = []

            for frame in stkNative:

                mod = getModule(frame.instructionOffset)

                if mod and printopt.combineWow64 and stkWow64:
                    if mod.name() == "wow64cpu":
                        break

                frame.cpuType = str(getCPUMode())
                stk.append(frame)

            for frame in stkWow64:

                frame.cpuType = "WOW64"
                stk.append(frame)

            if printopt.showUnique:
                stackHash= getStackHash(stk)
                if stackHash in stackHashes:
                    continue
                stackHashes.add( stackHash )

            if moduleFilter:
                if not [ m for m in getStackModules(stk) if moduleFilter( m, m.name() ) ]:
                     continue

            if funcFilter:
                match = False
                for sym in getStackSymbols(stk):
                    if funcFilter(sym) or ( len( sym.split('!', 1) ) == 2 and funcFilter( sym.split('!', 1)[1] ) ):
                        match = True
                        break
                if not match:
                    continue

            printThread( thread, process )

            for frame in stk:
                printFrame(frame, printopt)


    except DbgException:

        if not printopt.ignoreNotActiveProcess:

            dprintln( "Process %x" %  process )
            dprintln( "Name: %s" %  processName )
            dprintln( "Failed to switch into process context\n")
            dprintln( "" )  


def main():

    dprintln("Stack walker. ver 1.1")

    if not hasattr(pykd, "__version__") or not fnmatch( pykd.__version__, "0.3.*"):
        dprintln ( "pykd has incompatible version" )
  
    parser = OptionParser()
    parser.add_option("-p", "--process", dest="processfilter", 
        help="process filter: boolean expression with python syntax" )
    parser.add_option("-m", "--module", dest="modulefilter", 
        help="module filter: boolean expression with python syntax" )
    parser.add_option("-f", "--function", dest="funcfilter",
       help="function filter: boolean expression with python syntax" )
    parser.add_option("-t", "--thread", dest="threadfilter",
       help="thread filter: boolean expresion with python syntax" )    
    parser.add_option("-u", "--unique", action="store_true", dest="uniquestack",
       help="show only unique stacks" )
    parser.add_option("-d", "--dump", dest="dumpname",
       help="open crach dump" )
    parser.add_option("-w", "--wow64", dest="wow64", 
       help="show WOW64 stacks")
    
    (options, args) = parser.parse_args()

    if not isWindbgExt():
        initialize()
        loadDump( options.dumpname )
    
    if not isKernelDebugging():
       dprintln("This script is only for kernel debugging")
       return

    setupGlobalObject()

    processFilter = None
    moduleFilter = None
    funcFilter = None
    threadFilter = None
    
    if options.processfilter:
        processFilter = lambda process, pid, name: eval( options.processfilter )
        
    if options.modulefilter:
        moduleFilter = lambda module, name: eval(options.modulefilter)
        
    if options.funcfilter:
        funcFilter = lambda name: eval( options.funcfilter)
        
    if options.threadfilter:
        threadFilter = lambda thread, tid: eval( options.threadfilter)

    printopt = PrintOptions()
    printopt.showUnique = True if options.uniquestack else False

    if options.wow64 != None:
        printopt.showWow64stack = options.wow64.lower() == 'true'

           
    processLst = nt.typedVarList( nt.PsActiveProcessHead, "_EPROCESS", "ActiveProcessLinks.Flink")  
    for process in processLst:
        printProcess( process, processFilter, threadFilter, moduleFilter, funcFilter, printopt )


if __name__ == "__main__":
    main()
