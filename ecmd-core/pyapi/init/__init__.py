# import the right SWIG module depending on Python version
from sys import version_info
if version_info[0] >= 3:
    from .python3 import *
else:
    from .python2 import *
del version_info
