
from pykd import *
import sys

def printThreadInfo():

    nt = module("nt")
    
    thread = nt.typedVar( "_KTHREAD", getThreadOffset( getCurrentThread() ) )
    
    stackPointer = addr64( reg("rsp") if is64bitSystem() else reg("esp") )
    
    dprintln("")    
    
    dprintln( "Stack Base: %x Limit: %x Current: %x Used: %x Unused: %x" % 
        ( thread.InitialStack, thread.StackLimit, stackPointer, thread.InitialStack - stackPointer, stackPointer - thread.StackLimit  )  )
        

def printDeltaStat():

    dprintln("")

    printThreadInfo()
    
    stk = getStack() 
    
    moduleLst = {}
    funcLst = {}
    
    for i in range( 0, len(stk) -1 ):
    
        try:
            mod = module( stk[i].ret )
        except BaseException:
            continue    
  
        delta = stk[i+1].fp - stk[i].fp
        if delta > 0:
       
            moduleName = mod.name()
       
            if moduleName in moduleLst:
                moduleLst[moduleName] = moduleLst[moduleName] + delta
            else:
                moduleLst[moduleName] = delta
                
            func = moduleName + "!" + mod.findSymbol( stk[i].ret, showDisplacement = False )

            if func in funcLst:
                funcLst[func] = funcLst[func] + delta
            else:
                funcLst[func] = delta    
                
    nt = module("nt")
    
    thread = nt.typedVar( "_KTHREAD", getThreadOffset( getCurrentThread() ) )
                
    stackSize = thread.InitialStack - thread.StackLimit
                
    dprintln("")
    dprintln( "%12s\t%s" % ("Stack usage:", "Module:" ) )
    dprintln( "="*30 )
    
    for mod,delta in sorted( moduleLst.iteritems(), key=lambda x: x[1], reverse=True ):
       dprintln( "%7d%5s\t%s" % (delta, "(%%%d)" % (delta*100/stackSize), mod ) )
   
    dprintln("")
    dprintln( "%12s\t%s" % ("Stack usage:", "Function" ) )
    dprintln( "="*30 )    
    
    for func,delta in sorted( funcLst.iteritems(), key=lambda x: x[1], reverse=True ):
       dprintln( "%7d%5s\t%s" % (delta, "(%%%d)" % (delta*100/stackSize), func ) )       

def printDeltaStack():
    
    printThreadInfo()

    stk = getStack()        
    
    dprintln( "Stack Delta:\tFunction:")
    
    for i in range( 0, len(stk) -1 ):
        dprint( "%12s\t" % long( stk[i+1].fp - stk[i].fp) )
        try:
            mod = module( stk[i].ret )
            dprintln( "%s!%s"% ( mod.name(), mod.findSymbol( stk[i].ret, showDisplacement = False ) ) )
        except BaseException:
            dprintln( findSymbol( stk[i].ret ) )        

def main():

    if len(sys.argv) > 1:
        if sys.argv[1] == "stat":
            printDeltaStat()
            return       

    printDeltaStack()
                    
if __name__ == "__main__":
    main()
