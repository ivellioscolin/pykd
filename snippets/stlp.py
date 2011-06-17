"""Dump STLPort containers"""

import sys
from pykd import *

StlpNodeBase = typeInfo()
StlpNodeBase.append(ptr_t, "color")
StlpNodeBase.append(ptr_t, "parent")
StlpNodeBase.append(ptr_t, "left")
StlpNodeBase.append(ptr_t, "right")

StlpMap = typeInfo()
StlpMap.append(StlpNodeBase, "header")
StlpMap.append(ptr_t, "node_count")

def stlpMapIncrement(addr):
    node = StlpNodeBase.load(addr)

    if (node.right != 0):
        node = StlpNodeBase.load(node.right)
        while (node.left != 0):
            node = StlpNodeBase.load(node.left)
    else:
        ynode = StlpNodeBase.load(node.parent)
        while (node.getAddress() == ynode.right):
            node = ynode
            ynode = StlpNodeBase.load(ynode.parent)
        # check special case: This is necessary if _M_node is the
        # _M_head and the tree contains only a single node __y. In
        # that case parent, left and right all point to __y!
        if (node.right != ynode.getAddress()):
          node = ynode

    return node.getAddress()


def dumpStlportMap(addr):
    """Returns the list of addresses of pair<key, value>"""
    addrList = list()
    #dprintln("Map address: 0x%x" % addr)
    map = StlpMap.load(addr)
    #dprintln("Map node count: %u" % map.node_count)

    count = 0
    begin = map.header.left
    end = addr
    it = begin
    while (it and it != end):
        addrList.append(it + map.header.sizeof())
        it = stlpMapIncrement(it)
        count += 1

    if (count != map.node_count):
        dprintln("Error: map was dumped incorrectly.")

    return addrList


def printUsage():
    dprintln("Usage:")
    dprintln("!py stlp map <map_address|variable_name> [\"accurate map pair type\"]")
    dprintln("Use dt command to retrive accurate map pair type:")
    dprintln("dt -r ModuleName!stlp_std::pair*")
    dprintln("Find required type in the list and copy paste it as script parameter. Don't forget about quotes.")

if __name__ == "__main__":
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
            dprintln("0x%x" % addr)
        else:
            s = "dt -r " + sys.argv[3] + " 0x%x" % addr
            #println(s)
            dprintln("------------------------------------------------")
            dprintln(dbgCommand(s))
