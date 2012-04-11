#  Copyright (c) 2009-2010, Cloud Matrix Pty. Ltd.
#  All rights reserved; available under the terms of the BSD License.
"""

PySideKick.Call:  helpers for managing function calls
=====================================================


This module defines a collection of helpers for managing function calls in
cooperation with the Qt event loop.  We have:

    * qCallAfter:   call function after current event has been processed
    * qCallLater:   call function after sleeping for some interval
    * qCallInMainThread:   (blockingly) call function in the main GUI thread
    * qCallInWorkerThread:   (nonblockingly) call function in worker thread


There is also a decorator to apply these helpers to all calls to a function:

    * qCallUsing(helper):  route all calls to a function through the helper

"""

import sys
import thread
import threading
from functools import wraps
import Queue

import PySideKick
from PySideKick import QtCore, qIsMainThread


class qCallAfter(QtCore.QObject):
    """Call the given function on a subsequent iteration of the event loop.

    This helper arranges for the given function to be called on a subsequent
    iteration of the main event loop.  It's most useful inside event handlers,
    where you may want to defer some work until after the event has finished
    being processed.

    The implementation is as a singleton QObject subclass.  It maintains a
    queue of functions to the called, and posts an event to itself whenever
    a new function is queued.
    """
 
    def __init__(self):
        super(qCallAfter,self).__init__(None)
        self.app = None
        self.event_id = QtCore.QEvent.registerEventType()
        self.event_type = QtCore.QEvent.Type(self.event_id)
        self.pending_func_queue = Queue.Queue()
        self.func_queue = Queue.Queue()
 
    def customEvent(self,event):
        if event.type() == self.event_type:
            self._popCall()

    def __call__(self,func,*args,**kwds):
        global qCallAfter
        #  If the app is running, dispatch the event directly.
        if self.app is not None:
            self.func_queue.put((func,args,kwds))
            self._postEvent()
            return
        #  Otherwise, we have some bootstrapping to do!
        #  Before dispatching, there must be a running app and we must
        #  be on the same thread as it.
        app = QtCore.QCoreApplication.instance()
        if app is None or not qIsMainThread():
            self.pending_func_queue.put((func,args,kwds))
            return
        #  This is the first call with a running app and from the 
        #  main thread.  If it turns out we're not on the main thread,
        #  replace ourselves with a fresh instance.
        if hasattr(self,"thread"):
            if self.thread() is not QtCore.QThread.currentThread():
                qCallAfter = self.__class__()
            else:
                self.app = app
        else:
            self.app = app
        #  OK, we now have the official qCallAfter instance.
        #  Flush all pending events.
        try:
            while True:
                (pfunc,pargs,pkwds) = self.pending_func_queue.get(False)
                qCallAfter(pfunc,*pargs,**pkwds)
        except Queue.Empty:
            pass
        qCallAfter(func,*args,**kwds)

    def _popCall(self):
        (func,args,kwds) = self.func_queue.get(False)
        func(*args,**kwds)

    def _postEvent(self):
        event = QtCore.QEvent(self.event_type)
        try:
            self.app.postEvent(self,event)
        except RuntimeError:
            #  This can happen if the app has been destroyed.
            #  Immediately empty the queue.
            try:
                while True:
                    self._popCall()
            except Queue.Empty:
                pass

#  Optimistically create the singleton instance of qCallAfter.
#  If this module is imported from a non-gui thread then this instance will
#  eventually be replaced, but usually it'll be the correct one.
qCallAfter = qCallAfter()



class Future(object):
    """Primative "future" class for executing functions in another thread.

    Instances of this class represent a compuation sent to another thread.
    Call the "get_result" method to wait until completion and get the result
    (or raise the exception).

    Existing Future objects are recycled to avoid the overhead of allocating
    a new lock primitive for each async call.
    """

    _READY_INSTANCES = []

    def __init__(self):
        self.ready = threading.Event()
        self.result = None
        self.exception = None

    @classmethod
    def get_or_create(cls):
        try:
            return cls._READY_INSTANCES.pop(0)
        except IndexError:
            return cls()

    def recycle(self):
        self.result = None
        self.exception = None
        self.ready.clear()
        self._READY_INSTANCES.append(self)

    def call_function(self,func,*args,**kwds):
        try:
            self.result = func(*args,**kwds)
        except Exception:
            self.exception = sys.exc_info()
        finally:
            self.ready.set()

    def get_result(self):
        self.ready.wait()
        try:
            if self.exception is None:
                return self.result
            else:
                raise self.exception[0], \
                      self.exception[1], \
                      self.exception[2]
        finally:
            self.recycle()

    def is_ready(self):
        return self.read.isSet()


def qCallInMainThread(func,*args,**kwds):
    """Synchronously call the given function in the main thread.

    This helper arranges for the given function to be called in the main
    event loop, then blocks and waits for the result.  It's a simple way to
    call functions that manipulate the GUI from a non-GUI thread.
    """
    if qIsMainThread():
        func(*args,**kwds)
    else:
        future = Future.get_or_create()
        qCallAfter(future.call_function,func,*args,**kwds)
        return future.get_result()


def qCallInWorkerThread(func,*args,**kwds):
    """Asynchronously call the given function in a background worker thread.

    This helper arranges for the given function to be executed by a background
    worker thread.  Eventually it'll get a thread pool; for now each task
    spawns a new background thread.

    If you need to know the result of the function call, this helper returns
    a Future object; use f.is_ready() to test whether it's ready and call
    f.get_result() to get the return value or raise the exception.
    """
    future = Future.get_or_create()
    def runit():
        future.call_function(func,*args,**kwds)
    threading.Thread(target=runit).start()
    return future


def qCallLater(interval,func,*args,**kwds):
    """Asynchronously call the given function after a timeout.

    This helper is similar to qCallAfter, but it waits at least 'interval'
    seconds before executing the function.  To cancel the call before the
    sleep interval has expired, call 'cancel' on the returned object.

    Currently this is a thin wrapper around threading.Timer; eventually it
    will be integrated with Qt's own timer mechanisms.
    """
    def runit():
        qCallAfter(func,*args,**kwds)
    t = threading.Timer(interval,runit)
    t.start()
    return t


def qCallUsing(helper):
    """Function/method decorator to always apply a function call helper.

    This decorator can be used to ensure that a function is always called
    using one of the qCall helpers.  For example, the following function can
    be safely called from any thread, as it will transparently apply the
    qCallInMainThread helper whenever it is called:

        @qCallUsing(qCallInMainThread)
        def prompt_for_input(msg):
            # ... pop up a dialog, return input

    """
    def decorator(func):
        @wraps(func)
        def wrapper(*args,**kwds):
            return helper(func,*args,**kwds)
        return wrapper
    return decorator


