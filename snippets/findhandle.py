from pykd import *
import ntobj
import sys

nt = module("nt")


def findHanle(objaddr):

    processList = typedVarList( nt.PsActiveProcessHead, "nt!_EPROCESS", "ActiveProcessLinks"  )

    for process in processList:

        dprintln( "search in process %x " % process.UniqueProcessId +  "".join( [chr(i) for i in process.ImageFileName if i != 0] ) )
        
        if process.ObjectTable == 0:
            continue

        objects = ntobj.getListByHandleTable( process.ObjectTable )
        for obj in objects:
            if obj[0] == objaddr:
                dprintln("\tHandle: %x" % ( obj[1],) )
               

def usage():
    dprintln("!py findhandle object_address")

def main():

    if not isKernelDebugging():
        dprintln("This script for kernel debugging only")
        return

    if len(sys.argv) < 2:
        usage();
        return;

    objaddr = expr(sys.argv[1])
    
    objectType = ntobj.getType(objaddr)
    
    dprintln("Object Type: " + ntobj.getObjectName(objectType) )
    dprintln("Object Name: "+ ntobj.getObjectName(objaddr) )
    dprintln("")

    findHanle( objaddr )

if __name__ == "__main__":
    main()