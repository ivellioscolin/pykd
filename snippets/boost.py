########################################################
#   
#   Author: Mlody   
#
#   This scripts enumerates boost types:
#   Currently supported types: list(also slist) and rbtree
#
#   Usage:
#   !py boost list     <list_address>   <stored_element_type> [additional params for dt cmd]
#   !py boost rbtree   <rbtree_address> <stored_element_type> [additional params for dt cmd]
#
#   Currently works only when member_hook is the first class member.
#
#######################################################

import sys
from pykd import *

tb = typeBuilder()

rbtreeNode = tb.createStruct("rbtreeNode")
rbtreeNode.append("parent_", tb.VoidPtr)
rbtreeNode.append("left_", tb.VoidPtr)
rbtreeNode.append("right_", tb.VoidPtr)

rbtreeType = tb.createStruct("rbtreeType")
rbtreeType.append("size_", tb.Long)
rbtreeType.append("header_", rbtreeNode)
rbtreeType.append("colour_", tb.Long) 

listType = tb.createStruct("listType")
listType.append("size_", tb.Long)
listType.append("next_", tb.VoidPtr)
listType.append("prev_", tb.VoidPtr)

def enumerateList(listAddr, elemType,params):
    try:
        list = typedVar(listType, eval(listAddr))
        size = ptrPtr(list.size_.getAddress())
        print "List size: " + hex(size) + "\n"
    
        prev = list.next_.getAddress()
        next = ptrPtr(list.next_.getAddress())  
        counter = 0
        while(next != prev):
            print "###List element"+"("+str(counter)+")"+" at "+  hex(next)
            s = dbgCommand("dt " +elemType+ " 0n" + str(next) + params)
            dprint(s)
            print ""
            next = ptrPtr(next)  
            counter += 1
    except MemoryException as e:
        print e
        print "Probably You've passed an invalid address"
        quit(0)
        
    if counter != size:
        print "!!! Number of printed elements("+counter+") differs from number of elements declared in list("+size+") !!!"
            
def inorderTreeSearch(Node, elemType,params):
    counter = 0
    nodeStack = []
    done = False
    currNode = Node
    
    while not done:
        try:    
            if currNode.getAddress() != 0:
                nodeStack.append(currNode)
                leftAddr = ptrPtr(currNode.left_.getAddress())
                currNode = typedVar(rbtreeNode, leftAddr)
            else:
                if len(nodeStack) == 0:
                    done = True
                else:
                    currNode = nodeStack.pop()
                
                    addr = currNode.getAddress()
                    print "###RBTree element"+"("+str(counter)+")"+" at "+  hex(addr)
                    s = dbgCommand("dt " +elemType+ " 0n" + str(addr) + params)
                    dprint(s)
                    print ""
                    counter += 1
                
                    rightAdrr = ptrPtr(currNode.right_.getAddress())
                    currNode = typedVar(rbtreeNode, rightAdrr)
                
        except MemoryException as e:
            print e
            print "Probably You've passed an invalid address"
            quit(0)
            
    return counter
    
def enumerateRBTree(rbtreeAdrr, elemType,params):
    try:
        rbtree = typedVar(rbtreeType, eval(rbtreeAdrr))
        size = ptrPtr(rbtree.size_.getAddress())
        print "RBTree size: " + hex(size) + "\n"
        
        nodeAddr = ptrPtr(rbtree.header_.parent_.getAddress())
        if  nodeAddr != 0:
            node = typedVar(rbtreeNode, nodeAddr)
            counter = inorderTreeSearch(node,elemType,params)
            
            if counter != size:
                print "!!! Number of printed elements("+str(counter)+") differs from number of elements declared in RBTree("+str(size)+") !!!"
    
    except MemoryException as e:
        print e
        print "Probably You've passed an invalid address"
        quit(0)

def printUsage():
   dprintln("Usage:")
   dprintln("!py boost list     <list_address>   <store_element_type> [additional params for dt cmd]")
   dprintln("!py boost rbtree   <rbtree_address> <store_element_type> [additional params for dt cmd]")
   
def main():
    argc = len(sys.argv)
    if (argc < 4 or (sys.argv[1] != "list" and sys.argv[1] != "rbtree")):
       printUsage()
       quit(0)
    
    address = sys.argv[2]
    if not address.startswith("0x"):
        address = "0x" + address
    
    try:
        elemType = typeInfo(sys.argv[3])    
    except:
        print "Cannot find type: " + sys.argv[3]
        quit(0)
        
    params = " "
    for i in range(4 ,argc):
        params += sys.argv[i] + " "
        
    if  sys.argv[1] == "list":
        enumerateList(address,sys.argv[3],params)
    elif  sys.argv[1] == "rbtree":
        enumerateRBTree(address,sys.argv[3],params)
               
if __name__ == "__main__":
    main()
    