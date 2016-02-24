%pythonbegin %{
# This code lets _ecmd.so be loaded properly for some extensions
import sys, DLFCN
sys.setdlopenflags(DLFCN.RTLD_NOW|DLFCN.RTLD_GLOBAL)
%}
