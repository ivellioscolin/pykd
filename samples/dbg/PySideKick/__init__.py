#  Copyright (c) 2009-2010, Cloud Matrix Pty. Ltd.
#  All rights reserved; available under the terms of the BSD License.
"""

PySideKick:  helpful utilities for working with PySide
======================================================


This package is a rather ad-hoc collection of helpers, utilities and custom
widgets for building applications with PySide.  So far we have:

  * PySideKick.Call:  helpers for calling functions in a variety of ways,
                      e.g. qCallAfter, qCallInMainThread

  * PySideKick.Console:   a simple interactive console to embed in your
                          application

  * PySideKick.Hatchet:   a tool for hacking frozen PySide apps down to size,
                          by rebuilding PySide with a minimal set of classes

"""

__ver_major__ = 0
__ver_minor__ = 2
__ver_patch__ = 3
__ver_sub__ = ""
__ver_tuple__ = (__ver_major__,__ver_minor__,__ver_patch__,__ver_sub__)
__version__ = "%d.%d.%d%s" % __ver_tuple__


import thread

from PySide import QtCore, QtGui
from PySide.QtCore import Qt


#  Older versions of PySide don't expose the 'thread' attribute of QObject.
#  In this case, assume the thread importing this module is the main thread.
if hasattr(QtCore.QCoreApplication,"thread"):
    def qIsMainThread():
        app = QtCore.QCoreApplication.instance()
        if app is None:
            return False
        return QtCore.QThread.currentThread() is app.thread()
else:
    _MAIN_THREAD_ID = thread.get_ident()
    def qIsMainThread():
        return thread.get_ident() == _MAIN_THREAD_ID


#  PySideKick.Call needs to create a singleton object in the main gui thread.
#  Since this is *usually* the thread that imports this module, loading it here
#  will provide a small speedup in the common case.
import PySideKick.Call


