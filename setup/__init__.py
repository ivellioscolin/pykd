import sys

if sys.version_info >= ( 3, 0 ):
    from pykd.pykd import *
else:
    from pykd import *

__version__ = pykd.__version__
__file__ = pykd.__file__
