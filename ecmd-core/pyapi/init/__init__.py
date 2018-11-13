# import the right SWIG module depending on Python version
from sys import version_info
from sys import path as sys_path
from os import path as os_path
if version_info[0] >= 3:
    sys_path.append(os_path.join(os_path.dirname(__file__), "python3"))
    from .python3 import *
else:
    sys_path.append(os_path.join(os_path.dirname(__file__), "python2"))
    from .python2 import *
del sys_path, os_path, version_info
