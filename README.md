# Pykd - python extension for WinDBG

## Purpose
This project can help to automate debugging and crash dump analysis using Python. It allows one to take the best from both worlds: the expressiveness and convenience of Python with the power of WinDbg!

## Why do I need this?
If you want to automate the debugger but dislike the WinDbg built-in script machine syntax it's time to try again using the handy and powerful Python language. Even if you don't want to create your own script, maybe some existing scripts will be of interest to you?

## Pykd bootstrapper
Use it to run pykd within windbg. [link](https://githomelab.ru/pykd/pykd-ext)

## Releases:
[Last Release](/../wikis/0.3.4.6)    
[PyPi](https://pypi.org/project/pykd)

## Documentation
[User Manual](/../wikis/User Manual rus)    
[API Reference](/../wikis/API Reference)     

## How to build ?
* install VS 2017     
https://visualstudio.microsoft.com/downloads/

* install cmake   
https://cmake.org/

* install git    
https://git-scm.com/

* get sources:

```
git clone --recurse-submodules https://githomelab.ru/pykd/pykd.git local_dir
```
* open pykd.sln 
* choose a python version and a target platform then build pykd projects