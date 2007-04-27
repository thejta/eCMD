# Makefile for an eCMD Client

# Choose the eCMD Release to build against
# Are we setup for eCMD, if so let's get our eCMD Release from there 
ifeq ($(strip $(ECMD_RELEASE)),)
 ifneq ($(strip $(ECMD_DLL_FILE)),)
   ECMD_RELEASE := $(shell ecmdVersion)
   # Make sure we got a valid version back, if not default to rel
   ifeq ($(findstring ver,$(ECMD_RELEASE)),)
     ECMD_RELEASE := rel
   endif
 else
 # If not setup for eCMD, default to rel
   ECMD_RELEASE := rel
 endif
endif

# Ok, now set our eCMD Path, if not set already
ifeq ($(strip $(ECMD_PATH)),)
  ECMD_PATH   := ${CTEPATH}/tools/ecmd/${ECMD_RELEASE}/
endif

# Link the client
ecmd_databuff_compare_testcase: ecmd_databuff_compare_testcase.o
	g++ -g -L${ECMD_PATH}/lib/ -ldl -lecmd_x86 ecmd_databuff_compare_testcase.o ${ECMD_PATH}/capi/ecmdClientCapi_x86.a -o ecmd_databuff_compare_testcase

# Compile the client code
ecmd_databuff_compare_testcase.o: ecmd_databuff_compare_testcase.C ${ECMD_PATH}/capi/ecmdClientCapi.H ${ECMD_PATH}/capi/ecmdStructs.H ${ECMD_PATH}/capi/ecmdSharedUtils.H
	g++ -g -I${ECMD_PATH}/capi/  -ftemplate-depth-40 -c ecmd_databuff_compare_testcase.C -o ecmd_databuff_compare_testcase.o
