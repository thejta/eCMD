
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

# Check the ECMD_ARCH env var to see if we should build 32 or 64 bit
ifeq ($(strip $(ECMD_ARCH)),)
  ifeq ($(strip $(OS)),AIX)
    TARGET_ARCH := aix
  else
    TARGET_ARCH := x86
  endif
else
  TARGET_ARCH := ${ECMD_ARCH}
endif

# The compiler will be different based on x86 or x86_64 - this is just a generic g++ path for the example
CC            := /usr/bin/g++

# Link the client
ecmdclient: ecmdclient.o
	${CC} -g -L${ECMD_PATH}/${TARGET_ARCH}/lib/ -ldl -lecmd ecmdclient.o ${ECMD_PATH}/${TARGET_ARCH}/lib/ecmdClientCapi.a ${ECMD_PATH}/${TARGET_ARCH}/lib/cmdClientCapi.a -o ecmdclient

# Compile the client code
ecmdclient.o: ecmdclient.C ${ECMD_PATH}/capi/ecmdClientCapi.H ${ECMD_PATH}/capi/ecmdStructs.H  ${ECMD_PATH}/capi/ecmdSharedUtils.H ${ECMD_PATH}/ext/cmd/capi/cmdClientCapi.H ${ECMD_PATH}/ext/cmd/capi/cmdStructs.H
	${CC} -g -I${ECMD_PATH}/capi/ -I${ECMD_PATH}/ext/cmd/capi/  -ftemplate-depth-30 -c ecmdclient.C -o ecmdclient.o
