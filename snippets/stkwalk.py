
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
        self.showUnique = False

def applayThreadFilter(thread,threadFilter,moduleFilter,funcFilter,printopt,stackHashes):

    filterMatch = False

    if not moduleFilter and not funcFilter and not threadFilter:
        if printopt.showUnique == False: 
            return True
        else:
            filterMatch = True
  
    if threadFilter and threadFilter( thread.Tcb, thread.Cid.UniqueThread ):
        if printopt.showUnique  == False:
            return True
        else:
            filterMatch = True
        
    try:
    
        setImplicitThread(thread)
       
        stk = getStack()
        
        strStack = ""
        
        for frame in stk:
            m = module( frame.instructionOffset )
            if filterMatch == False and moduleFilter and moduleFilter( m, m.name() ):
                filterMatch = True
                if printopt.showUnique  == False:
                    return True
                
            sym = m.findSymbol( frame.instructionOffset, showDisplacement = False )
            if filterMatch == False and funcFilter and funcFilter( sym ):
                filterMatch = True 
                if printopt.showUnique  == False:
                    return True
            
            if printopt.showUnique  == True:
                strStack += sym
                
        if is64bitSystem():
            processorMode = getProcessorMode()
            try:
                setProcessorMode("X86")
                dbgCommand( ".reload /user" )
                stk = getStackWow64()
                for frame in stk:
                    m = module( frame.instructionOffset )
                    if filterMatch == False and moduleFilter and moduleFilter( m, m.name() ):
                        filterMatch = True
                        if printopt.showUnique  == False:
                            return True
                
                    sym = m.findSymbol( frame.instructionOffset, showDisplacement = False )
                    if filterMatch == False and funcFilter and funcFilter( sym ):
                        filterMatch = True 
                        if printopt.showUnique  == False:
                            return True
            
                    if printopt.showUnique  == True:
                        strStack += sym
            except BaseException:
                pass         
            setProcessorMode(processorMode)                          

        if printopt.showUnique  == False or filterMatch == False:
            return filterMatch
            
        hashStack = hash( strStack )
        if hashStack in stackHashes:
            return False
            
        stackHashes.add( hashStack )

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
        
        threadLst = nt.typedVarList(process.ThreadListHead, "_ETHREAD", "ThreadListEntry")
        filteredThreadLst = []
        stackHashes = set()
        
        for thread in threadLst:     
            if applayThreadFilter( thread, threadFilter, moduleFilter, funcFilter, printopt, stackHashes ):
                filteredThreadLst.append( thread )
                
        if filteredThreadLst == []:
            return     

        dprintln( "Process %x" %  process )
        dprintln( "Name: %s  Pid: %#x" %  ( processName, process.UniqueProcessId ) )
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
    parser.add_option("-u", "--unique", action="store_true", dest="uniquestack",
        help="show only unique stacks" )    
    
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
    printopt.showUnique = True if options.uniquestack else False
           
    currentProcess = getCurrentProcess()
    currentThread = getImplicitThread()

    processLst = nt.typedVarList( nt.PsActiveProcessHead, "_EPROCESS", "ActiveProcessLinks")  
    for process in processLst:
        printProcess( process, processFilter, threadFilter, moduleFilter, funcFilter, printopt )  
            
    setCurrentProcess(currentProcess)
    setImplicitThread(currentThread)
        

if __name__ == "__main__":
    main()
