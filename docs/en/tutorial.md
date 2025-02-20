## Introduction

### Step 1: Getting Started
It is best to use the automatic installer for installation. It will install pykd in the correct location and also install and register all necessary components.

If the installation completes without errors, it is time to get acquainted with pykd. To do this, start **WinDbg** and begin a debugging session (open a process, dump file, or establish a connection with a kernel debugger). Now, you can load pykd by running the following command:

```
.load pykd.pyd
```

If any errors occur during loading, WinDbg will display a message. The absence of any messages indicates that the extension has been successfully loaded.

Now, you can start working. Execute the following command:

```
!pycmd
```
After running this command, the debugger will enter user input mode. All user input will be processed by the Python interpreter.

```
0:000> !pycmd
Python 2.6.6 (r266:84297, Aug 24 2010, 18:13:38) [MSC v.1500 64 bit (AMD64)] on win32
Type "help", "copyright", "credits" or "license" for more information.
(InteractiveConsole)
>>> print "Hello world!"
Hello world!
>>>
```

Now is a good time to get familiar with Python syntax if you haven’t already. Python is very easy to learn.

Let's recall some basic syntax of Python:

```
>>> def printHello():
...     i = 0
...     while i < 4:
...         print "Hello #%d" % i
...         i += 1
...
>>> printHello()
Hello #0
Hello #1
Hello #2
Hello #3
>>>
```

Note: The indentation of blocks is determined by leading spaces, which is a "signature" feature of Python. For now, this knowledge will be sufficient. Let’s move on.

### Step 2: Accessing Registers

Any debugger should provide three basic capabilities: reading processor registers, reading memory, and controlling debugging mode. Let’s start with registers. With **pykd**, this is quite simple:

```
>>> print hex(reg("eip"))
0x778ecb60
>>> print hex(reg("esp"))
0x1ef0e0
>>> print hex(reg("esp")+4)
0x1ef0e4
```

In this case, we use the **PYKD** function `reg`. It reads processor registers by name. A curious reader might ask: how do we use functions from **PYKD** without explicitly importing the module? In reality, the module must be imported. However, **PYKD** automatically does this when constructing the Python console.

Let's write a small example to check where the current instruction pointer is pointing:

```
>>> print findSymbol(reg("eip"))
ntdll!LdrpDoDebuggerBreak+30
```

The `findSymbol` function attempts to find a debug symbol for a given address. In this case, we see that the instruction pointer is at offset `0x30` within the function `LdrpDoDebuggerBreak`, which is located in the **ntdll** module. We can determine this because we have debugging information for **ntdll.dll** (the corresponding **pdb** file). If symbols are not displaying for some reason, check your symbol path settings in **WinDbg**.

### Step 3: Accessing Memory

**PYKD** provides a large set of functions for accessing memory, which can be divided into three categories:

#### 1. Reading values from memory:
- `ptrByte`
- `ptrWord`
- `ptrDWord`
- `ptrQWord`

There are other functions as well; the full set can be found in the **PYKD 0.2 API Reference**. All functions take an address as a parameter and return the value stored at that address.

#### 2. Reading arrays
- `loadBytes`
- `loadWords`
- `loadDWords`
- `loadQWords`

All functions take a pointer to the beginning of an array and its length in elements as parameters. They return a **list** object containing the array elements.

#### 3. Reading strings
- `loadCStr`
- `loadWStr`

These functions read **null-terminated** strings from memory and return Python strings.

Let’s modify the previous example to display function arguments. We assume that the function follows the **stdcall** calling convention and that its parameters are addressed by the **ebp** register.

```
>>> def printFunc():
...     print findSymbol( reg("eip") )
...     params = [  ptrDWord( reg("ebp") + 4*(i+1) ) for i in range(0,3) ]
...     print "var1: %x  var2: %x  var3:  %x" % ( params[0], params[1], params[2] )
...
>>> print printFunc()
ntdll32!LdrpDoDebuggerBreak+2c
var1: 774b1383  var2: fffdd000  var3:  fffde000
None
>>>
```

Pay attention to this construct:

```
params = [  ptrDWord( reg("ebp") + 4*(i+1) ) for i in range(0,3) ]
```

This is a **list comprehension**—a special Python construct that simplifies list initialization. It is equivalent to:

```
 [  ptrDWord( reg("ebp") + 4) ),  ptrDWord( reg("ebp") + 8) ), ptrDWord( reg("ebp") + 0xC) ) ]
```

### Step 4: Accessing Memory with Type Information

When debugging programs, we often work with **typed variables**. **PYKD** has powerful capabilities for accessing variables while preserving type information. This is one of the key features of the project: accessing struct and class fields in a way similar to source code.

For example, consider the following **C** code:

```
struct STRUCT_A {
  int  field1;
  char field2;
};

STRUCT_A a = { 100, 2 }
```

Now, during debugging, we want to check the state of variable `a` using **PYKD**:

```
   a = typedVar( "module!STRUCT_A",  getOffset("module!a") )
   if a.field1 != 100 or a.field2 != 2:       
       print "ERROR! a is not properly initialized!"
```