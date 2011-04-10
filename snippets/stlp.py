"""Dump STLPort containers"""

import sys
from pykd import *


def println(msg):
    if runningAsWinDbgExtension:
        dprintln(msg)
    else:
        print msg

class StlpNodeWrapper:
    def __init__(self, addr):
        # Wrapper specific field
        self.addr = addr
        # Wrapper specific field
        self.sizeOf = 4 * ptrSize()
        self.color = ptrByte(addr)
        # By default, fields in structure aligned by 4 Bytes on x86 and by 8 Bytes on x64
        addr += ptrSize()
        self.parent = ptrPtr(addr)
        addr += ptrSize()
        self.left = ptrPtr(addr)
        addr += ptrSize()
        self.right = ptrPtr(addr)


class StlpMapWrapper:
    def __init__(self, addr):
        # Wrapper specific field
        self.addr = addr
        self.rootNode = StlpNodeWrapper(addr)
        self.size = ptrPtr(addr + self.rootNode.sizeOf)


def stlpMapIncrement(addr):
    node = StlpNodeWrapper(addr)

    if (node.right != 0):
        node = StlpNodeWrapper(node.right)
        while (node.left != 0):
            node = StlpNodeWrapper(node.left)
    else:
        ynode = StlpNodeWrapper(node.parent)
        while (node.addr == ynode.right):
            node = ynode
            ynode = StlpNodeWrapper(ynode.parent)
        # check special case: This is necessary if _M_node is the
        # _M_head and the tree contains only a single node __y. In
        # that case parent, left and right all point to __y!
        if (node.right != ynode.addr):
          node = ynode

    return node.addr


def dumpStlportMap(addr):
    """Returns the list of addresses of pair<key, value>"""
    addrList = list()
    #println("Map address: 0x%x" % addr)
    map = StlpMapWrapper(addr)
    #println("Map node count: %u" % map.size)

    count = 0
    begin = map.rootNode.left
    end = addr
    it = begin
    while (it and it != end):
        addrList.append(it + map.rootNode.sizeOf)
        it = stlpMapIncrement(it)
        count += 1

    if (count != map.size):
        println("Error: map was dumped incorrectly.")

    return addrList


def printUsage():
    println("Usage:")
    println("!py stlp map <map_address|variable_name> [\"accurate map pair type\"]")
    println("Use dt command to retrive accurate map pair type:")
    println("dt -r ModuleName!stlp_std::pair*")
    println("Find required type in the list and copy paste it as script parameter. Don't forget about quotes.")

if __name__ == "__main__":
    global runningAsWinDbgExtension

    runningAsWinDbgExtension = not isSessionStart()
    mapAddr = 0

    argc = len(sys.argv)
    if (argc < 3 or sys.argv[1] != "map"):
        printUsage()
        quit(0)
    else:
        mapAddr = int(expr(sys.argv[2]))
    
    addrList = dumpStlportMap(mapAddr)
    for addr in addrList:
        if (argc == 3):
            println("0x%x" % addr)
        else:
            s = "dt -r " + sys.argv[3] + " 0x%x" % addr
            #println(s)
            println("------------------------------------------------")
            println(dbgCommand(s))
