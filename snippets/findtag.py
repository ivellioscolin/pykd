from pykd import *
from sys import argv

nt = module("nt")
LDR_DATA_TABLE_ENTRY = nt.type("_LDR_DATA_TABLE_ENTRY")
    

def getModuleList():
    ldrLst = typedVarList( nt.PsLoadedModuleList, LDR_DATA_TABLE_ENTRY, "InLoadOrderLinks.Flink")
    return [ module(m.DllBase) for m in ldrLst ]  
    
def findTagInModule(mod, tag):
    
    matchLst = []
    begin = mod.begin()
    end = mod.end()
    offset = begin
    size = mod.size()
    while True:
        match = searchMemory( offset, size, tag )
        if not match:
            break;
        matchLst.append(match)
        offset = match + 1
        size = end - offset
    return matchLst
    
    
def main():

    if len(argv) < 2:
        print "You should note tag's value"
        return

    if len(argv[1])!=4:
        print "Tag must have 4 symbols length"
        return
        
    tag = argv[1]      

    modLst = getModuleList()
    for m in modLst:
        matchLst = findTagInModule( m, tag )
        if len(matchLst) == 0:
            #print m.name(), "tag not found"
            pass
        else:
            print m.name(), "found", len(matchLst), "entries"
            for offset in matchLst:
                print "\t", hex(offset)
   
   
if __name__=="__main__":
    main()


