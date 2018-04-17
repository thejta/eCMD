from __future__ import print_function

print("\n\n#######################################################################################################")
print("# Importing ecmd from the python2 or python3 directory is deprecated!                                 #")
print("# There is a common python directory for both 2 and 3 now, and ecmdsetup sets up PYTHONPATH,          #")
print("# so no PYTHONPATH modification should be necessary on your part.                                     #")
print("# The python2 and python3 dirs will vanish soon, so please update your wrappers/boilerplate/etc.      #")
print("#######################################################################################################\n\n")

# this is a convoluted way to say "from this specific package directory import *"
import os, imp, sys
ecmd_path = os.path.join(os.path.dirname(__file__), "..", "python")
sys.path.insert(0, ecmd_path)
ecmd_indirect = imp.load_module("ecmd", None, os.path.join(ecmd_path, "ecmd"), ("", "", imp.PKG_DIRECTORY))
for name in dir(ecmd_indirect):
    if not name.startswith("_"):
       globals()[name] = getattr(ecmd_indirect, name)
del sys.path[0]
del os, imp, sys, ecmd_path, ecmd_indirect
