# Pykd - not only python extension for WinDBG

## Purpose
This project can help to automate debugging and crash dump analysis using Python. It allows one to take the best from both worlds: the expressiveness and convenience of Python with the power of WinDbg!

## Why do I need this?
If you want to automate the debugger but dislike the WinDbg built-in script machine syntax it's time to try again using the handy and powerful Python language. Even if you don't want to create your own script, maybe some existing scripts will be of interest to you?

## Getting started
* You should have installed python. Supported python: 2.7, 3.5 - 3.9
* Install pykd with command: 'pip install pykd'
* Use windbg extension (bootstrapper) to run pykd with windbg

## Windbg extension (bootstarpper)
Use it to run pykd within windbg.     
[Home Page](https://githomelab.ru/pykd/pykd-ext)     
[Download](https://githomelab.ru/pykd/pykd-ext/wikis/Downloads)   

## Releases
Just run 'pip install pykd' to install or 'pip install pykd --upgrade' to upgrade to the last version   
[PyPi](https://pypi.org/project/pykd)   
[All Releases](/../wikis/All Releases)  

## Documentation
[Wiki Home](/../wikis/Home)    
[User Manual](/../wikis/User Manual rus)    
[API Reference](/../wikis/API Reference)   

## Useful tools for pykd and WinDBG
[windbg-pack](https://githomelab.ru/pykd/windbg-pack)    
Set of python scripts for WinDBG

[pykdwin](https://githomelab.ru/pykd/pykdwin)   
A library with auxiliary modules for writing small pykd scripts and daily use.

[windbg-kernel](https://githomelab.ru/pykd/windbg-kernel)    
Set of pykd script for kernel debugging


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

## Projects used pykd
[mona.py](https://github.com/corelan/mona)    
the exploit writer's swiss army knife

[flare-dbg](https://github.com/fireeye/flare-dbg)    
flare-dbg is a project meant to aid malware reverse engineers in rapidly developing debugger scripts. 

[voltron](https://github.com/snare/voltron)    
Voltron is an extensible debugger UI toolkit written in Python