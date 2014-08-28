%pythonbegin %{
# This code lets _ecmd.so be loaded properly for fapi
import sys, dl
sys.setdlopenflags(dl.RTLD_NOW|dl.RTLD_GLOBAL)
%}
