# import the right SWIG module depending on Python version
from sys import version_info
import sys, os
if version_info[0] >= 3:
    sys.path.insert(0, os.path.join(os.path.dirname(__file__), "python3"))
    from .python3 import *
else:
    sys.path.insert(0, os.path.join(os.path.dirname(__file__), "python2"))
    from .python2 import *
del sys, os, version_info
