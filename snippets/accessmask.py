#
# Access mask parser (c) Ignatich
#

from pykd import *
import sys
import re

AccessMask = {
    0x00010000 : 'DELETE',
    0x00020000 : 'READ_CONTROL',
    0x00040000 : 'WRITE_DAC',
    0x00080000 : 'WRITE_OWNER',
    0x00100000 : 'SYNCHRONIZE',
    0x01000000 : 'ACCESS_SYSTEM_SECURITY',
    0x02000000 : 'MAXIMUM_ALLOWED'
    }

GenericMask = {
    0x10000000 : 'GENERIC_ALL',
    0x20000000 : 'GENERIC_EXECUTE',
    0x40000000 : 'GENERIC_WRITE',
    0x80000000 : 'GENERIC_READ'
	}

FileAccessMask = {
    0x0001 : 'FILE_READ_DATA',
    0x0002 : 'FILE_WRITE_DATA',
    0x0004 : 'FILE_APPEND_DATA',
    0x0008 : 'FILE_READ_EA',
    0x0010 : 'FILE_WRITE_EA',
    0x0020 : 'FILE_EXECUTE',
    0x0040 : 'FILE_DELETE_CHILD',
    0x0080 : 'FILE_READ_ATTRIBUTES',
    0x0100 : 'FILE_WRITE_ATTRIBUTES'
    }

ProcessAccessMask = {
    0x0001 : 'PROCESS_TERMINATE',
    0x0002 : 'PROCESS_CREATE_THREAD',
    0x0004 : 'PROCESS_SET_SESSIONID',
    0x0008 : 'PROCESS_VM_OPERATION',
    0x0010 : 'PROCESS_VM_READ',
    0x0020 : 'PROCESS_VM_WRITE',
    0x0040 : 'PROCESS_DUP_HANDLE',
    0x0080 : 'PROCESS_CREATE_PROCESS',
    0x0100 : 'PROCESS_SET_QUOTA',
    0x0200 : 'PROCESS_SET_INFORMATION',
    0x0400 : 'PROCESS_QUERY_INFORMATION',
    0x0800 : 'PROCESS_SUSPEND_RESUME',
    0x1000 : 'PROCESS_QUERY_LIMITED_INFORMATION'
    }

ThreadAccessMask = {
    0x0001 : 'THREAD_TERMINATE',
    0x0002 : 'THREAD_SUSPEND_RESUME',
    0x0004 : 'THREAD_ALERT',
    0x0008 : 'THREAD_GET_CONTEXT',
    0x0010 : 'THREAD_SET_CONTEXT',
    0x0020 : 'THREAD_SET_INFORMATION',
    0x0040 : 'THREAD_QUERY_INFORMATION',
    0x0080 : 'THREAD_SET_THREAD_TOKEN',
    0x0100 : 'THREAD_IMPERSONATE',
    0x0200 : 'THREAD_DIRECT_IMPERSONATION',
    0x0400 : 'THREAD_SET_LIMITED_INFORMATION',
    0x0800 : 'THREAD_QUERY_LIMITED_INFORMATION'
    }

FileMaskSets = [FileAccessMask, AccessMask, GenericMask]
ProcessMaskSets = [ProcessAccessMask, AccessMask, GenericMask]
ThreadMaskSets = [ThreadAccessMask, AccessMask, GenericMask]
GenericMaskSets = [AccessMask, GenericMask]

def parseMask(mask, maskSets) :
    cnt = 0
    for i in range(0, 31) :
        bit = 1 << i
        for maskSet in maskSets :
            if (bit & mask) and bit in maskSet:
                if (cnt != 0) :
                    dprint(" | ")
                    if (cnt % 4 == 0) :
                        dprintln("")
                dprint("" + maskSet[bit])
                mask &= ~bit
                cnt += 1
    return mask

def main():

    argc = len(sys.argv)

    if argc == 1 :
        dprintln("Syntax: [object type] <;hex mask>;")
        dprintln("Supported object types: process, thread, file, generic")
        return

    type = (argc > 2 and sys.argv[1]) or "generic"
    if argc > 2 :
        mask = int(sys.argv[2], 16)
    else :
        mask = int(sys.argv[1], 16)

    if type == "file" :
        mask = parseMask(mask, FileMaskSets)
    if type == "process" :
        mask = parseMask(mask, ProcessMaskSets)
    if type == "thread" :
        mask = parseMask(mask, ThreadMaskSets)
    elif type == "generic" :
        mask = parseMask(mask, GenericMaskSets)

    dprintln("")

    if mask != 0 :
        dprintln("Unknown bits: 0x%x" % mask)

if __name__ == "__main__":
    main()

