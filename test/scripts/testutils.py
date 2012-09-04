"""PyKd test heplers/wrappers"""

import pykd

class ContextCallIt:
    """Context manager/with statement"""
    def __init__(self, callIt):
        self.callIt = callIt

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, exc_tb):
        try: self.callIt()
        except: pass

class KillProcess:
    """Kill process"""
    def __init__(self, processId):
        self.processId = processId

    def __call__(self):
        pykd.killProcess( self.processId )
        pykd.detachProcess( self.processId )

