
from pykd import *
from optparse import OptionParser
from fnmatch import fnmatch
import traceback
import sys

nt = None

class PrintOptions:
    def __init__(self):
        self.ignoreNotActiveThread = True
        self.ignoreNotActiveProcess = True
        self.showWow64stack = True
        self.showIP = True
        self.showSP = True

def applayThreadFilter(thread,threadFilter,moduleFilter,funcFilter,printopt):

    if not moduleFilter and not funcFilter and not threadFilter:
        return True

    if threadFilter and threadFilter( thread.Tcb, thread.Cid.UniqueThread ):
        return True
        
    try:
    
        setImplicitThread(thread)
       
        stk = getStack()
        
        for frame in stk:
            m = module( frame.instructionOffset )
            if moduleFilter and moduleFilter( m, m.name() ):
                return True
            sym = m.findSymbol( frame.instructionOffset, showDisplacement = False )
            if funcFilter and funcFilter( sym ):
                return True            

    except BaseException:
        pass
        
    return False
    
def printFrame(frame, printopt):
    if printopt.showIP:
        dprint( "%016x\t" % frame.instructionOffset )
    if printopt.showSP:
        dprint( "%016x\t" % frame.stackOffset )  
    
    dprintln( findSymbol( frame.instructionOffset ) )     
    
        
def printThread(process,thread,printopt):

    try:
        setImplicitThread(thread)
        
        stk = getStack()
           
        dprintln( "Thread %x, Process: %s (%x)" % ( thread, loadCStr( process.ImageFileName ), process )  )                 
        for frame in stk:
            printFrame(frame, printopt)
            
        if is64bitSystem():
            processorMode = getProcessorMode()
            try:
                setProcessorMode("X86")
                dbgCommand( ".reload /user" )
                stk = getStackWow64()
                dprintln("\nWOW64 stack")
                for frame in stk:
                    printFrame(frame, printopt)
            except BaseException:
                pass         
            setProcessorMode(processorMode)            
         
        dprintln("")
         
    except BaseException:
    
        if not printopt.ignoreNotActiveThread:
            dprintln( "Thread %x, Process: %s" % ( thread, loadCStr( process.ImageFileName ) ) )     
            dprintln( "Failed to switch into thread context\n")
            dprintln("")    

       
    
def printProcess(process,processFilter,threadFilter,moduleFilter,funcFilter,printopt):

    processName = loadCStr( process.ImageFileName )
     
    if processFilter and not processFilter(process, process.UniqueProcessId, processName ):
        return
    
    try:
        #setCurrentProcess(process)
        dbgCommand(".process /p %x" % process )
       
        dbgCommand( ".reload /user" )

        reloadWow64 = False        
        
        threadLst = nt.typedVarList(process.ThreadListHead, "_ETHREAD", "ThreadListEntry")
        filteredThreadLst = []
        for thread in threadLst:     
            if applayThreadFilter( thread, threadFilter, moduleFilter, funcFilter, printopt ):
                filteredThreadLst.append( thread )
                
        if filteredThreadLst == []:
            return        

        dprintln( "Process %x" %  process )
        dprintln( "Name: %s" %  processName )
        dprintln( "" )            

        for thread in filteredThreadLst:
            printThread(process,thread, printopt)        
        
    except BaseException:
        if not printopt.ignoreNotActiveProcess:
            dprintln( "Process %x" %  process )
            dprintln( "Name: %s" %  processName )
            dprintln( "Failed to switch into process context\n")
            dprintln( "" )      
        
    
def main():
    dprintln("Stack walker. ver 1.0")
    
    if not isKernelDebugging():
        dprintln("This script is only for kernel debugging")
        return
    
    global nt
    nt = module("nt")
  
    parser = OptionParser()
    parser.add_option("-p", "--process", dest="processfilter", 
        help="process filter: boolean expression with python syntax" )
    parser.add_option("-m", "--module", dest="modulefilter", 
        help="module filter: boolean expression with python syntax" )
    parser.add_option("-f", "--function", dest="funcfilter",
        help="function filter: boolean expression with python syntax" )
    parser.add_option("-t", "--thread", dest="threadfilter",
        help="thread filter: boolean expresion with python syntax" )    
    
    
    (options, args) = parser.parse_args()
     
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
           
    currentProcess = getCurrentProcess()
    currentThread = getImplicitThread()

    processLst = nt.typedVarList( nt.PsActiveProcessHead, "_EPROCESS", "ActiveProcessLinks")  
    for process in processLst:
        printProcess( process, processFilter, threadFilter, moduleFilter, funcFilter, printopt )  
            
    setCurrentProcess(currentProcess)
    setImplicitThread(currentThread)
        

if __name__ == "__main__":
    main()
