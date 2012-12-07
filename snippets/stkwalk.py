
from pykd import *
from optparse import OptionParser
from fnmatch import fnmatch

nt = None

class PrintOptions:
    def __init__(self):
        self.ignoreNotActiveThread = True
        self.ignoreNotActiveProcess = True

def applayThreadFilter( thread,moduleFilter):

    try:
        setImplicitThread(thread)
        
        stk = getStack()
        
        moduleLst = set()
        for frame in stk:
            m = module( frame.instructionOffset )
            if moduleFilter( m, m.name() ):
                moduleLst.add(m)
           
        if len(moduleLst)==0:
            return False

    except BaseException:
        return False
        
    return True        
    
        
def printThread(process,thread,printopt):

    try:
        setImplicitThread(thread)
        
        stk = getStack()
           
        dprintln( "Thread %x, Process: %s" % ( thread, loadCStr( process.ImageFileName ) ) )                 
        for frame in stk:
            dprintln( findSymbol( frame.instructionOffset ) )
            
        dprintln("")                
        
    except BaseException:
        if not printopt.ignoreNotActiveThread:
            dprintln( "Thread %x, Process: %s" % ( thread, loadCStr( process.ImageFileName ) ) )     
            dprintln( "Failed to switch into thread context\n")
            dprintln("")    

    
    
def printProcess(process,processFilter,moduleFilter,printopt):

    processName = loadCStr( process.ImageFileName )
     
    if not processFilter(process, process.UniqueProcessId, processName ):
        return
    
    try:
        setCurrentProcess(process)
        
        dbgCommand( ".reload /user" )
        
        threadLst = nt.typedVarList(process.ThreadListHead, "_ETHREAD", "ThreadListEntry")
        filteredThreadLst = []
        for thread in threadLst:
            if applayThreadFilter( thread, moduleFilter ):
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
    
    (options, args) = parser.parse_args()
     
    processFilter =  lambda process, pid, name: True
    moduleFilter = lambda module, name: True
     
    if options.processfilter:
        processFilter = lambda process, pid, name: eval( options.processfilter )
        
    if options.modulefilter:
        moduleFilter = lambda module, name: eval(options.modulefilter)
        
    printopt = PrintOptions()
           
    currentProcess = getCurrentProcess()
    currentThread = getImplicitThread()

    processLst = nt.typedVarList( nt.PsActiveProcessHead, "_EPROCESS", "ActiveProcessLinks")  
    for process in processLst:
        printProcess( process, processFilter, moduleFilter, printopt )  
            
    setCurrentProcess(currentProcess)
    setImplicitThread(currentThread)
        

if __name__ == "__main__":
    main()
