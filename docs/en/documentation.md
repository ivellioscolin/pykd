# Table of Contents
* [1. Introduction](#intro)
  * [1.1 General Information](#commoninfo)
  * [1.2 Quick Start](#gettingStarted)
  * [1.3 Building from Source](#Building)
  * [1.4 Manual Installation](#Installing)
  * [1.5 API Changes](#APIchange)
* [2. Windbg Commands](#windbg)
  * [2.1 Loading the Plugin](#loadplugin)
  * [2.2 Running a Script](#runscript)
  * [2.3 Console Mode](#console)
* [3. Debugging Management](#debugging)
  * [3.1 Pausing and Resuming Debugging](#break)
  * [3.2 Step-by-Step Execution](#step)
  * [3.3 Debugging from Python Applications](#consoledebug)
  * [3.4 Printing Debug Information](#dbgprint)
  * [3.5 Executing Debugger Commands](#dbgcommand)
  * [3.6 Creating a Crash Dump](#createcrash)
* [4. Working with Memory and Registers](#memory)
  * [4.1 Accessing General-Purpose Registers](#reg)
  * [4.2 Accessing Model-Specific Registers](#msr)
  * [4.3 Virtual Address Normalization](#addr64)
  * [4.4 Direct Memory Access](#memaccess)
  * [4.5 Memory Access Errors](#memerror)
  * [4.6 Reading Strings from Memory](#memstr)
* [5. Modules](#modules)
  * [5.1 Module Class](#moduleclass)
  * [5.2 Module Load and Unload Events](#moduleload)
* [6. Retrieving Symbolic Information](#syminfo)
  * [6.1 Symbol Files (PDB)](#pdbfile)
  * [6.2 Type Information](#types)
* [7. Typed Variables](#typedVar)
  * [7.1 TypedVar Class](#typedVar)
  * [7.2 Creating an Instance of TypedVar](#typedVarClass)
  * [7.3 TypedVar Class Methods](#typedVarMethod)
  * [7.4 Classes and Structures](#typedVarUDT)
  * [7.5 Arrays and Pointers](#typedVarArray)
  * [7.6 Enumerations](#typedVarEnum)
  * [7.7 Casting to Other Types](#typedVarCast)
* [8. Processes and Threads](#ProcessThreads)
  * [8.1 User-Mode Threads](#UserModeThreads)
  * [8.2 Kernel-Mode Threads](#KernelModeThreads)
  * [8.3 Kernel-Mode Processes](#KernelModeProcess)
* [9. Local Variables](#Locals)
  * [9.1 Current Local Variables](#CurrentLocals)
* [10. Breakpoints](#breakpoints)
  * [10.1 Setting Breakpoints](#setBreakpoints)
  * [10.2 Conditional Breakpoints](#condBreakpoints)
* [11. Debugging Events](#eventHandler)
* [12. Disasm Class](#disasm)
* [API Reference | PYKD 0.2 API Reference]

---

## 1. Introduction  

### 1.1 General Information  
The **pykd** project started in 2010. The main motivation for its development was the inconvenience of built-in tools for writing debugging scripts for **WinDbg**. The **Python** language was chosen as an alternative scripting engine for several reasons:  
- Ease of learning the language  
- A large standard library  
- A powerful and convenient framework for creating extension modules  

**pykd** is a module for the **CPython** interpreter. It is written in **C++** and uses **Boost.Python** to export functions and classes to Python. **pykd** provides access to debugging management on the Windows platform via the **Debug Engine** library and retrieves symbolic information via the **MS DIA** library.  

It is important to note that **pykd** does not provide direct access to the **COM interfaces** of **Debug Engine** and **MS DIA**. Instead, it implements its own interface, making the development process faster and more convenient (at least, that is the hope).  

**pykd** can work in two modes:  
1. **As a plugin for WinDbg**, providing commands for running scripts within a debugging session.  
2. **As a standalone module for the Python interpreter**, which is useful for creating automated tools to analyze crash dumps, for example.  

[Back to Table of Contents](#table)  

---

### 1.2 Quick Start  
For a quick start, it is best to download the **automatic installer**. It will install all necessary components, including **Python** (if it is not already installed).  

To verify the installation, launch **WinDbg** and start debugging an application or analyzing a dump. Then, load **pykd**:  
```cmd
.load pykd.pyd
```  
If no error messages appear, the installation was successful. To double-check that everything works, try running the Python interactive console within **WinDbg**:  
```cmd
>!pycmd
Python 2.6.5 (r265:79096, Mar 19 2010, 18:02:59) [MSC v.1500 64 bit (AMD64)] on win32
Type "help", "copyright", "credits" or "license" for more information.
(InteractiveConsole)
>>> print("Hello world!")
Hello world!
>>> quit()
>
```  
Try running example scripts:  
```cmd
!py help
!py samples
```  
If everything works, you can proceed to writing your own scripts.  

[Back to Table of Contents](#table)  

---

### 1.3 Building from Source  

#### Getting the Source Code  
Download the source code from the [repository](http://pykd.codeplex.com/SourceControl/list/changesets).  

#### Installing Python  
Download the required version from [here](http://www.python.org).  

#### Installing and Configuring Boost  
Boost can be obtained from [this link](http://www.boost.org). Installation and build instructions are also available there.  

#### Setting Up Environment Variables  
The following environment variables need to be set for compilation:  
- `$(DIA_SDK_ROOT)` – Path to the **MS DIA** library. It should look like:  
  ```
  C:\Program Files (x86)\Microsoft Visual Studio 9.0\DIA SDK
  ```
  The **MS DIA** library is installed with Visual Studio.  
- `$(DBG_SDK_ROOT)` – Path to the **Debug Engine SDK**:  
  ```
  C:\Program Files (x86)\Debugging Tools for Windows (x86)\sdk
  ```
  The **Debug Engine SDK** is included in **Debugging Tools for Windows** (now part of **Platform SDK**).  
- `$(BOOST_ROOT)` – Path to the **Boost** installation directory.  
- `$(PYTHON_ROOT)` – Path to the **Python** installation directory. The expected directory structure:  
  ```
  C:\Python26\x86\...
  C:\Python26\x64\...
  ```
  If Python installation does not differentiate between x86 and x64, the project file may need modification.  

#### Building Boost.Python Libraries  
To compile the required **Boost.Python** static libraries, use the following commands:  
```cmd
bjam --stagedir=stage --with-python stage
bjam address-model=64 --stagedir=stage64 --with-python stage
```
If **bjam** is not installed, download it from [here](http://www.boost.org/users/download/boost_jam_3_1_18.html).  

[Back to Table of Contents](#table)  

---

### 1.4 Manual Installation  

#### Installing pykd.pyd and Required Dependencies  
To install manually, you will need:  
- `pykd.pyd`  
- **C++ Runtime Redistributable** (`vcredist`) matching the version used to build `pykd.pyd`.  

#### Where to Copy pykd.pyd?  
The location depends on how you intend to use `pykd`:  
1. **As a WinDbg plugin** – Copy `pykd.pyd` to the `winext` directory inside the **WinDbg installation folder**. You may rename it to `pykd.dll` so it can be loaded without specifying an extension:  
   ```cmd
   kd>.load pykd
   ```
2. **For Python scripting** – Place `pykd.pyd` in a directory where Python can find it. Possible options:  
   - Inside Python's `Lib` subdirectory.  
   - Any custom directory, added to the `PYTHONPATH` environment variable.  
   - Any directory, without modifying `PYTHONPATH`, but always launching Python from that directory.  

#### Installing vcredist  
Make sure to install the appropriate **C++ Runtime Redistributable (vcredist)**.  

#### Registering MS DIA  
The **MS DIA** library must be registered after installation. Find the `msdia90.dll` file and execute:  
```cmd
regsvr32 msdia90.dll
```
If you built the module yourself using **Visual Studio**, then **vcredist** is already installed, and **MS DIA** should be registered automatically.  

[Back to Table of Contents](#table)  

---

### 1.5 API Changes  

#### `loadModule` Function Removal  
The `loadModule` function has been removed. Use the `module` class constructor instead:  
```python
# Old way (no longer valid)
mod = loadModule("mymodule")

# New way
mod = module("mymodule")
```

[Back to Table of Contents](#table)  

---

## 2. Windbg Commands  

### 2.1 Loading the Plugin  
To load the **pykd** plugin in **WinDbg**, run:  
```cmd
kd>.load pykd_path/pykd.pyd
```
If `pykd.pyd` is inside the **winext** folder (a subdirectory of **Debugging Tools for Windows**), the path is not needed:  
```cmd
kd>.load pykd.pyd
```
If `pykd.pyd` is renamed to `pykd.dll`, the extension can be omitted:  
```cmd
kd>.load pykd
```
To view loaded extensions in **WinDbg**, use:  
```cmd
kd>.chain
```
To unload the plugin:  
```cmd
kd>.unload pykd_path/pykd.pyd
kd>.unload pykd.pyd
kd>.unload pykd
```
To avoid manually loading **pykd** every session, load it once and then save the workspace:  
```cmd
kd> Save Workspace
```

[Back to Table of Contents](#table)  

---

### 2.2 Running a Script  
To run a **Python script** using **pykd**, use the `!py` command:  
```cmd
kd>!py script_path/script_name.py param1 param2 ...
```
The `.py` extension can be omitted.  
To avoid specifying full paths, add the script directory to `PYTHONPATH` or modify the Windows registry:  
```
HKEY_LOCAL_MACHINE\SOFTWARE\Python\PythonCore\2.6\PythonPath
```
In Python, command-line parameters are available in `sys.argv`:  
```python
import sys
print("Script path: " + sys.argv[0])
print("param1: " + sys.argv[1])
print("param2: " + sys.argv[2])
```

[Back to Table of Contents](#table)  

---

### 2.3 Console Mode  
To launch an interactive **Python console** within **WinDbg**, use:  
```cmd
kd>!pycmd
```
Example session:  
```cmd
1: kd> !pycmd
Python 2.6.5 (r265:79096, Mar 19 2010, 18:02:59) [MSC v.1500 64 bit (AMD64)] on win32
Type "help", "copyright", "credits" or "license" for more information.
(InteractiveConsole)
>>> 
```
The `pykd` module is automatically imported, so you can start using **pykd functions immediately**.  
To **exit the Python console**, use `quit()`. The state of the interpreter is preserved:  
```cmd
>>> a = 10
>>> quit()
1: kd> !pycmd
Python 2.6.5 (r265:79096, Mar 19 2010, 18:02:59) [MSC v.1500 64 bit (AMD64)] on win32
Type "help", "copyright", "credits" or "license" for more information.
(InteractiveConsole)
>>> print(a)
10
>>> 
```

[Back to Table of Contents](#table)  

---

## 3. Debugging Management  

### 3.1 Pausing and Resuming Debugging  

In **WinDbg**, you can pause and resume debugging using:  
- **Break (Ctrl + Break)**  
- **Go (F5)**  

Equivalent **pykd** functions:  
- `go()` – Resumes execution and returns control when the debugger stops.  
- `breakin()` – Forces a break into the debugger.  

#### Example: Continuous Debugging Loop  
```python
try:
    while True:
        go()
        print("Break detected")
except:
    print("Process terminated")
```
⚠ **Warning:** `breakin()` is rarely needed. Since scripts usually execute only while the debugger is paused, calling `breakin()` inside a script has no effect.  
To stop debugging from a script, create a separate thread for `breakin()`.  

⚠ **Do NOT use `breakin()`, `go()`, or `trace()` inside event handlers (e.g., conditional breakpoints).**  

[Back to Table of Contents](#table)  

---

### 3.2 Step-by-Step Execution  

For step debugging (tracing), use:  
- `step()` – Equivalent to **Step Into** in WinDbg.  
- `trace()` – Equivalent to **Step Over** in WinDbg.  

Both functions may raise a `DbgException` if the debugged process has exited.  

[Back to Table of Contents](#table)  

---

### 3.3 Debugging from Python Applications  

If you want to run scripts **outside WinDbg**, you must first create a debugging session.  
If only one session is used, it is **created automatically** with these functions:  

#### Creating a Debugging Session  
- `loadDump(dumpName)` – Loads a crash dump.  
- `startProcess(imageName)` – Starts a new process in debugging mode.  
- `attachProcess(processId)` – Attaches the debugger to an existing process.  
- `attachKernel(parameterStr)` – Attaches the debugger to a kernel debugging session.  

#### Detaching or Terminating the Debugged Process  
- `detachProcess(id)` – Detaches the debugger from a process.  
- `killProcess(id)` – Stops debugging and terminates the process.  

#### Checking Debugger Mode  
To determine if the debugger is analyzing a **memory dump**:  
```python
if isDumpAnalyzing():
    print("Debugger is analyzing a memory dump.")
```
To check if debugging is in **kernel mode**:  
```python
if isKernelDebugging():
    print("Kernel debugging mode detected.")
```
This is useful when scripts depend on **kernel symbols** and should only run in kernel mode.  

[Back to Table of Contents](#table)  

---

### 3.4 Printing Debug Information  

Instead of using `print()`, **pykd** provides specialized functions:  
- `dprint(message, dml=False)` – Prints a message without a newline.  
- `dprintln(message, dml=False)` – Prints a message **with a newline**.  

The `dml` parameter enables **DML (Debug Markup Language)** formatting, which works in **WinDbg only**.  

#### Example: Using DML for Clickable Output  
```python
dprintln("<b><u>Click here to reload all symbols:</u></b>", True)
dprintln("<link cmd=\".reload /f\">Reload Symbols</link>", True)
```
This displays a **clickable command** inside WinDbg.  

[Back to Table of Contents](#table)  

---

### 3.5 Executing Debugger Commands  

To run **WinDbg commands** from Python, use:  
```python
dbgCommand("!analyze -v")
```
Example:  
```python
output = dbgCommand("!analyze -v")
dprint(output)
```

To evaluate an **expression** (similar to the `?` command in WinDbg):  
```python
expr("@rax + 10")
```

#### Loading & Calling **WinDbg Extensions**  
```python
extHandle = loadExt("ext_path")   # Load an extension
result = callExt(extHandle, "command", "params")  # Call an extension command
removeExt(extHandle)  # Unload extension
```
⚠ **Important:** The `ext` class was removed in **pykd 0.2**. Use `loadExt()` instead.  

[Back to Table of Contents](#table)  

---

### 3.6 Creating a Crash Dump  

You can save the system state as a **crash dump** for later analysis.  

#### Creating a Dump File  
```python
writeDump("C:\\dump\\fulldump.dmp", False)  # Full memory dump
writeDump("C:\\dump\\minidump.dmp", True)   # Minidump (smaller)
```
The second parameter specifies the **dump type**:  
- `False` = **Full dump** (contains all memory pages).  
- `True` = **Minidump** (smaller, excludes unnecessary memory pages).  

Crash dumps can be created in **both user-mode and kernel-mode debugging**.  

[Back to Table of Contents](#table)  

---

## 4. Working with Memory and Registers  

---

### 4.1 Accessing General-Purpose Registers  

To access **general-purpose registers (GPRs)**, use:  
```python
* cpuReg reg( regName )   # Access register by name
* cpuReg reg( regIndex )  # Access register by index
```
#### Example: Accessing a Register  
```python
r = reg("eax")
print(r / 10 * 234)
```

#### Exception Handling  
Both versions of `reg()` return an instance of the **`cpuReg` class**.  
If the register information **cannot be retrieved**, a `BaseException` is raised.  

#### `cpuReg` Class Methods  
The `cpuReg` class provides the following methods:  
- `name()` – Returns the register name.  
- `index()` – Returns the register index.  

#### Arithmetic Operations with Registers  
The `cpuReg` class **supports arithmetic operations directly**, without type conversion:  
```python
r = reg("eax")
print(r / 10 * 234)
```

#### Enumerating All Registers  
```python
import pykd

try:
    i = 0
    while True:
        r = pykd.reg(i)
        pykd.dprintln(f"{r.name()} {hex(r)} ({r})")
        i += 1
except pykd.BaseException:
    pass
```

⚠ **Important:** `pykd` currently supports **only integer registers**.  
FPU, MMX, and SSE registers are **not supported** (planned for future versions).  

[Back to Table of Contents](#table)  

---

### 4.2 Accessing Model-Specific Registers (MSR)  

Use `rdmsr(msrNumber)` to read **MSR registers**:  
```python
print(findSymbol(rdmsr(0x176)))
```
[Back to Table of Contents](#table)  

---

### 4.3 Virtual Address Normalization  

All **pykd** functions return **normalized** 64-bit virtual addresses. On **32-bit** platforms, addresses are **sign-extended** to 64 bits:  

| Original Address | Normalized Address |
|-----------------|-------------------|
| `0x00100000`   | `0x0000000000100000` |
| `0x80100000`   | `0xFFFFFFFF80100000` |

For comparison, use `addr64()`:  
```python
import pykd
nt = pykd.module("nt")

if nt > addr64(0x80000000):
    print("NT module is in higher address space")
```
[Back to Table of Contents](#table)  

---

### 4.4 Direct Memory Access  

To read **unsigned integers** from memory:  
```python
ptrByte(va)
ptrWord(va)
ptrDWord(va)
ptrQWord(va)
```
For **signed integers**:  
```python
ptrSignByte(va)
ptrSignWord(va)
ptrSignDWord(va)
ptrSignQWord(va)
```
For **architecture-independent** reads:  
```python
ptrMWord(va)
ptrSignMWord(va)
ptrPtr(va)
```
To read a block of memory:  
```python
loadBytes(va, count)
loadWords(va, count)
loadDWords(va, count)
loadQWords(va, count)
```
Returns a **list**.  

[Back to Table of Contents](#table)  

---

### 4.5 Memory Access Errors  

If a memory access fails, `MemoryException` is raised:  
```python
try:
    a = ptrByte(0)
except MemoryException:
    print("Memory exception occurred")
```
To check if an address is valid:  
```python
isValid(va)
```
[Back to Table of Contents](#table)  

---

### 4.6 Reading Strings from Memory  

Instead of using `loadBytes()`, **pykd** provides:  
```python
loadChars(va, count)  # Returns ASCII string
loadWChars(va, count) # Returns Unicode string
```
For **null-terminated strings**:  
```python
loadCStr(va)
loadWStr(va)
```
⚠ **Maximum string length is 64 KB**. Longer strings will raise `MemoryException`.  

For Windows kernel **UNICODE_STRING** and **ANSI_STRING** structures:  
```python
loadAnsiString(va)
loadUnicodeString(va)
```
[Back to Table of Contents](#table)  

---

## 5. Modules  

### 5.1 Module Class  

A **module** is an executable file mapped into memory. A typical program consists of a main module (usually a `.exe` file) and a set of libraries.  

#### Creating a Module Instance  

The `module` class has two constructor forms:  
```python
module(moduleName)
module(va)
```
- The first form creates a module object by name.  
- The second form creates a module object by a virtual address belonging to the module.  

If the module is not found, a `BaseException` is raised.  

Example:
```python
from pykd import *

try:
    ntdll = module("ntdll")
    print(ntdll.name(), hex(ntdll.begin()), hex(ntdll.size()))
except BaseException:
    print("Module not found")
```

#### Retrieving Module Information  

Use these methods of the `module` class:  
- `name()` – Returns the module's name.  
- `image()` – Returns the module's executable file name.  
- `pdb()` – Returns the full path to the PDB (symbol) file.  
- `begin()` – Returns the module's base virtual address.  
- `end()` – Returns the module's end virtual address.  
- `checksum()` – Returns the module's checksum.  
- `timestamp()` – Returns the module's timestamp.  
- `getVersion()` – Returns the module's version as a tuple, e.g., `(1, 0, 6452, 0)`.  
- `queryVersion(valueName)` – Retrieves a specific version resource value.  

#### Loading and Accessing Symbols  

To load symbol information, use:  
```python
mod.reload()
```
To find a symbol's virtual address:  
```python
addr = mod.offset("symbolName")
```
If the symbol is not found, a `BaseException` is raised.  

Instead of explicitly calling `offset()`, you can use attributes:
```python
nt = module("nt")
print(hex(nt.offset("PsLoadedModuleList")))
print(hex(nt.PsLoadedModuleList))  # Shortcut
```
To retrieve the **Relative Virtual Address (RVA)** of a symbol:  
```python
rva = mod.rva("symbolName")
```

#### Module Type Information  

To retrieve type information, use:  
```python
t = mod.type("_MDL")
print(t)
```
Example output:
```
struct/class: _MDL Size: 0x1c (28)
   +0000 Next                : _MDL*
   +0004 Size                : Int2B
   +0006 MdlFlags            : Int2B
   +0008 Process             : _EPROCESS*
   +000c MappedSystemVa      : Void*
   +0010 StartVa             : Void*
   +0014 ByteCount           : ULong
   +0018 ByteOffset          : ULong
```

#### Typed Variables  

`pykd` provides the `typedVar` class for working with structured data. You can obtain an instance using:  
```python
mod.typedVar(va)
mod.typedVar(symbolName)
mod.typedVar(typeName, va)
```
Example:
```python
nt = module("nt")
print(nt.typedVar("_LIST_ENTRY", nt.PsLoadedModuleList))
```
Output:
```
struct/class: _LIST_ENTRY at 0xfffff8000369c650
   +0000 Flink               : _LIST_ENTRY*   0xfffffa8003c64890
   +0008 Blink               : _LIST_ENTRY*   0xfffffa80092f8f30
```

[Back to Table of Contents](#table)  

---

### 5.2 Handling Module Load and Unload Events  

To handle module load/unload events, subclass `eventHandler`:  
- `onLoadModule` – Called when a module is loaded.  
- `onUnloadModule` – Called when a module is unloaded.  

[Back to Table of Contents](#table)  

---

## 6. Retrieving Symbolic Information  

### 6.1 Symbol Files (PDB)  

A **PDB (Program Database) file** contains debugging symbols. Depending on compiler settings, it may include:  
- Global variables and constants.  
- Function and method names with parameters.  
- User-defined types (structs, classes, enums).  
- Constant values.  
- Local variables.  

**pykd** uses the **MS DIA library** to work with symbols and provides a custom interface for direct access.  

[Back to Table of Contents](#table)  

---

### 6.2 Type Information  

#### `typeInfo` Class  

The `typeInfo` class represents type information, including:  
- **Structures**  
- **Classes**  
- **Unions**  
- **Enumerations**  
- **Bit fields**  
- **Pointers**  
- **Basic types**  

Methods:  
```python
name()          # Returns type name  
size()          # Returns type size  
staticOffset()  # Returns static field offset  
fieldOffset()   # Returns field offset  
bitOffset()     # Returns bit field offset  
bitWidth()      # Returns bit field width  
field()         # Retrieves a field  
asMap()         # Retrieves enum name-value mapping  
deref()         # Dereferences a pointer  
ptrTo()         # Creates a pointer to a type  
arrayOf()       # Creates an array of the type  
append()        # Adds a field (for manually created structs)  
```

#### Getting a `typeInfo` Object  

Use the constructor with a type name:  
```python
t = typeInfo("ntdll!_UNICODE_STRING")
print(t)
```
Example output:
```
class/struct : _UNICODE_STRING Size: 0x10 (16)
   +0000 Length                  : UInt2B
   +0002 MaximumLength           : UInt2B
   +0008 Buffer                  : UInt2B*
```
To retrieve all types in a module:  
```python
nt = module("ntdll")
for typeName in nt.enumTypes():
    print(typeName)
```

#### Creating Custom Types  

If a required type is missing, use `typeBuilder`:  
```python
tb = typeBuilder()
us = tb.createStruct("_UNICODE_STRING")
us.append("Length", tb.UInt2B)
us.append("MaximumLength", tb.UInt2B)
us.append("Buffer", tb.WChar.ptrTo())
print(us)
```
Output:
```
class/struct : _UNICODE_STRING Size: 0x10 (16)
   +0000 Length                  : UInt2B
   +0002 MaximumLength           : UInt2B
   +0008 Buffer                  : WChar*
```

[Back to Table of Contents](#table]  

---

## 7. Typed Variables  

### 7.1 `typedVar` Class  

The `typedVar` class simplifies working with complex structures:  

```python
t1 = typedVar("MyModule!MyVar")
t2 = typedVar("MyModule!MyType", addr)
ti = typeInfo("MyModule!MyType")
t3 = typedVar(ti, addr)
```
If the variable or type does not exist, `SymbolException` is raised.  

Example:
```python
try:
    typedVar("MyModule!NonExistentVar")
except SymbolException:
    print("Variable does not exist")
```

[Back to Table of Contents](#table)  

---

### 7.2 `typedVar` Class Methods  

- `getAddress()` – Returns variable address.  
- `sizeof()` – Returns variable size.  
- `offset()` – Returns field offset within the parent structure.  
- `field(fieldName)` – Retrieves a structure field.  
- `deref()` – Dereferences a pointer.  
- `type()` – Retrieves variable type.  

[Back to Table of Contents](#table)  

---

### 7.3 `typedVar` Class Methods  

The `typedVar` class provides methods to access and manipulate structured data.  

#### Retrieving Variable Properties  
- `getAddress()` – Returns the memory address of the variable.  
- `sizeof()` – Returns the size of the variable in bytes.  
- `offset()` – If the variable is a structure field, returns the **offset** within its parent.  

#### Accessing Structure Fields  
- `field(fieldName)` – Returns the value of a specific field as a `typedVar` object.  

Example:  
```python
tv = typedVar("structVar")
print(tv.field("fieldName"))
```
Alternatively, access fields as **attributes**:  
```python
print(tv.fieldName)
```
Both methods return the same result.  

#### Dereferencing Pointers  
- `deref()` – Returns the value pointed to by a `typedVar` pointer.  

Example:  
```python
ptr = typedVar("ptrStruct")
dereferenced_value = ptr.deref()
```

#### Retrieving Type Information  
- `type()` – Returns the `typeInfo` object for the variable.  

[Back to Table of Contents](#table)  

---

### 7.4 Classes and Structures  

**Enumerating Structure Fields**  

To loop through all fields of a structure:  
```python
tv = typedVar("structVar")
for fieldName, fieldValue in tv:
    print(fieldName, fieldValue)
```
This returns a **tuple** containing the field name and value.  

[Back to Table of Contents](#table)  

---

### 7.5 Arrays and Pointers  

`typedVar` supports **arrays** and **multidimensional arrays**.  

#### Accessing Array Elements  
Use the `[]` operator:  
```python
arr = typedVar("intArray")
print(arr[0])  # First element
print(arr[2])  # Third element
```

#### Accessing Multidimensional Arrays  
```python
matrix = typedVar("intMatrix")
print(matrix[1][2])  # Element at row 1, column 2
```

#### Working with Pointers  
Use `deref()` to get the value stored at a pointer’s address:  
```python
ptr = typedVar("ptrIntArray")
print(ptr.deref()[1])  # Access second element through pointer
```

⚠ **Important:** `typedVar` **does not follow C pointer arithmetic rules**. Pointer arithmetic treats addresses as raw numbers.  

[Back to Table of Contents](#table)  

---

### 7.6 Enumerations  

Use `typeInfo.asMap()` to get an enumeration’s **name-value mapping**.  

Example:  
```python
var = typedVar("myStruct")
enum_type = var.structType.type().asMap()
print(enum_type[var.structType])  # Get enum name from value
```

#### Using Enums in Conditional Statements  
```python
if var.structType == var.structType.type().TYPE_ONE:
    print("TYPE_ONE detected")
else:
    print("ANOTHER_TYPE")
```

[Back to Table of Contents](#table)  

---

### 7.7 Casting to Other Types  

#### Converting `typedVar` to String  
```python
print(str(typedVar("g_struct")))
```
Example output:  
```
struct/class: struct3 at 0x13f4391f8
   +0000 m_arrayField   : Int4B[2]
   +0008 m_noArrayField : Int4B  0x3 (3)
```

#### Converting `typedVar` to Integer  
Use `int()` or `long()`. The conversion depends on the variable type:  
| **Type**      | **Conversion Result** |
|--------------|---------------------|
| **Basic Types** | Direct value |
| **Structures** | Pointer to the structure |
| **Enums** | Numeric value |
| **Pointers** | Address stored in pointer |
| **Arrays** | Address of first element |

Example:  
```python
var = typedVar("g_struct")
print(int(var.m_noArrayField))  # Integer conversion
print(hex(int(var.m_arrayField)))  # Get array address
```
Output:  
```
3
0x13f4391f8
```

[Back to Table of Contents](#table)  

---

## 8. Processes and Threads  

### 8.1 User-Mode Threads  

In user mode, the debugger operates in the context of the debugged process. If the process has multiple threads, you can switch contexts.  

**Key concepts:**  
- **Current thread** – The thread that will continue execution after resuming debugging.  
- **Implicit thread** – The thread the debugger is currently analyzing.  

#### Switching the Debugger's Thread Context  

To change the thread context, use:  
```python
setImplicitThread(teb_address)
```
To get the current implicit thread:  
```python
getImplicitThread()
```
To get a list of all process threads:  
```python
getProcessThreads()
```

[Back to Table of Contents](#table)  

---

### 8.2 Kernel-Mode Threads  

In kernel mode:  
- `setImplicitThread()` and `getImplicitThread()` use **ETHREAD** instead of TEB.  
- `getProcessThreads()` is **not available**.  

To enumerate process threads, use the `_EPROCESS` structure:  
```python
nt = module("nt")
process = nt.typedVar("_EPROCESS", processAddr)
threadList = nt.typedVarList(process.ThreadListHead, "_ETHREAD", "ThreadListEntry")
```
⚠ **Switching threads does not switch the process context.**  

[Back to Table of Contents](#table)  

---

### 8.3 Kernel-Mode Processes  

In kernel debugging, you can work with **system processes** and **driver execution contexts**.  

[Back to Table of Contents](#table)  

---

## 9. Local Variables  

### 9.1 Retrieving Local Variables  

If debug symbols contain local variable information, you can access them without manually handling registers and stack values.  

Use:  
```python
locals_dict = getLocals()
```
Example:
```python
# Retrieve a specific local variable
print(getLocals()["argc"])

# Print all local variables
for varName, varValue in getLocals().items():
    print(varName, varValue)
```

[Back to Table of Contents](#table)  

---

## 10. Breakpoints  

### 10.1 Setting Breakpoints  

Use `setBp()` to set breakpoints.  
It returns an **ID** that can be used to remove the breakpoint with `removeBp()`.  

**Setting a software breakpoint:**  
```python
nt = module("nt")
bpid = setBp(nt.NtCreateFile)
```
**Setting a hardware breakpoint:**  
```python
bpid = setBp(nt.NtCreateFile, 1, 4)
```
- The second parameter is the **memory access size**.  
- The third parameter is the **access type**:  
  - `1` – Read  
  - `2` – Write  
  - `4` – Execute  

[Back to Table of Contents](#table)  

---

### 10.2 Conditional Breakpoints  

Breakpoints can have **conditions** using callback functions.  

Example:  
```python
import fnmatch
from pykd import *

nt = module('nt')
objAttrType = nt.type("_OBJECT_ATTRIBUTES")

def onCreateFile(id):
    objattr = typedVar(objAttrType, ptrPtr(reg('esp') + 0xC))
    return fnmatch.fnmatch(loadUnicodeString(objattr.ObjectName), '*.exe')

setBp(nt.NtCreateFile, onCreateFile)
```
In this case, the breakpoint triggers only when an executable file (`*.exe`) is accessed.  

You can also use **lambda functions** for conditions:  
```python
setBp(myAddr, lambda id: reg('rax') > 0x1000)
```

### **Handling Breakpoint Persistence**  

If a script sets a breakpoint but then exits, the breakpoint **will be removed**.  

To persist it, use:  
1. **Script-controlled execution:**  
   ```python
   setBp(nt.NtCreateFile, onCreateFile)
   go()
   ```
   This waits for the breakpoint to trigger before exiting.  

2. **Using `!pycmd`**  
   Run the script inside `!pycmd`, so it remains active even after execution:  
   ```cmd
   >!pycmd
   >>> import setmybreak
   >>> quit()
   >g
   ```
   The breakpoint stays active even after exiting the Python console.  

⚠ **Restricted functions inside breakpoints:**  
Do **not** use the following inside breakpoint handlers:  
- `go()`, `breakin()`, `trace()`  
- `startProcess()`, `killProcess()`, `openDump()`  
- `setCurrentProcess()`, `setImplicitThread()`  

[Back to Table of Contents](#table)  

---

## 11. Debugging Events  

### 11.1 Handling Breakpoints (`onBreakpoint`)  

[Back to Table of Contents](#table)  

---

### 11.2 Handling Exceptions (`onException`)  

[Back to Table of Contents](#table)  

---

### 11.3 Handling Module Load Events (`onLoadModule`)  

[Back to Table of Contents](#table)  

---

### 11.4 Handling Module Unload Events (`onUnloadModule`)  

[Back to Table of Contents](#table)  


---

## 12. Disassembly (`disasm` Class)  

The `disasm` class is a wrapper over the **WinDbg** disassembler. It provides equivalent results to the `u` command.  

### 12.1 Creating a Disassembler  

```python
d = disasm()        # Start from current instruction
d = disasm(offset)  # Start from a specific address
```

### 12.2 Disassembling Instructions  

```python
instr = d.disasm()       # Disassemble next instruction
instr = d.disasm(offset) # Disassemble from a specific offset
```

### 12.3 Assembling Instructions  

```python
d.asm("mov eax, 1")  # Modify machine code
```

### 12.4 Getting Current Instruction Info  

```python
print(d.current())    # Current address
print(d.instruction()) # Instruction mnemonic
print(d.length())     # Instruction size
```

### 12.5 Getting Effective Address  

```python
print(d.ea())  # Effective address of last instruction operand
```

### 12.6 Resetting Disassembler  

```python
d.reset()  # Restart from initial address
```

[Back to Table of Contents](#table)  

---
