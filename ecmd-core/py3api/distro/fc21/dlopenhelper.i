%pythonbegin %{
# This code lets _ecmd.so be loaded properly for fapi
import sys, os
sys.setdlopenflags(os.RTLD_NOW|os.RTLD_GLOBAL)
%}
