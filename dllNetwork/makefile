# Makefile for a network DLL.

# *****************************************************************************
# Include the common base makefile
# *****************************************************************************
include ../makefile.base

# *****************************************************************************
# The Common Setup stuff
# *****************************************************************************
TARGET       := network.dll
CXXFLAGS     := ${CXXFLAGS} -I ${ECMD_ROOT}/ecmd-core/capi -I ${ECMD_ROOT}/ecmd-core/dll -I${SRCPATH} -Iserver -Iserver/fips_headers
VPATH        := ${VPATH}:${ECMD_ROOT}/ecmd-core/capi:${ECMD_ROOT}/ecmd-core/dll:${SRCPATH}:server

# *****************************************************************************
# Setup all the files going into the build
# *****************************************************************************

# The ECMD_INCLUDES are files provided by eCMD that if changed, we would want to recompile on
ECMD_INCLUDES := ecmdClientCapi.H ecmdDataBuffer.H ecmdReturnCodes.H ecmdStructs.H ecmdUtils.H ecmdClientEnums.H ecmdSharedUtils.H ecmdDefines.H

# The source files and includes in our local dirs that are going into the build
TGT_INCLUDES  := ecmdDllCapi.H eth_transfer1.h transfer.h OutputLite.H ecmdTransfer.H Controller.H fd_impl.H Instruction.H InstructionStatus.H FSIInstruction.H GPIOInstruction.H I2CInstruction.H ControlInstruction.H InstructionFlag.H PNORInstruction.H git_version.H
TGT_SOURCE    := ecmdDllCapi.C ecmdDllNetwork.C ecmdDllNetworkInfo.C eth_transfer1.C OutputLite.C ecmdTransfer.C Controller.C fd_impl.C Instruction.C InstructionStatus.C FSIInstruction.C GPIOInstruction.C I2CInstruction.C ControlInstruction.C InstructionFlag.C PNORInstruction.C git_version.C

# Combine all the includes into one variable for the build
INCLUDES      := ${ECMD_INCLUDES} ${TGT_INCLUDES}

# Pull the sim functions out of the network DLL
DEFINES += -DOTHER_USE -DHW

# *****************************************************************************
# The Main Targets
# *****************************************************************************
# The run-all rule is defined in makefile.rules
all:
	${run-all}

generate:
	$(MAKE) --directory=server $@

build: ${TARGET}
        # Remove the object after each build to force a rebuild
        # This forces the date to update
	@rm ${OBJPATH}/ecmdDllNetworkInfo.o

test:
  # Do nothing

install:
  # Do nothing

# *****************************************************************************
# Object Build Targets
# *****************************************************************************
TGT_SOURCE_OBJS  = $(basename ${TGT_SOURCE})
TGT_SOURCE_OBJS := $(addprefix ${OBJPATH}, ${TGT_SOURCE_OBJS})
TGT_SOURCE_OBJS := $(addsuffix .o, ${TGT_SOURCE_OBJS})

# *****************************************************************************
# Compile code for the common C++ objects if their respective
# code has been changed.  Or, compile everything if a header
# file has changed.
# *****************************************************************************
${TGT_SOURCE_OBJS}: ${OBJPATH}%.o : %.C ${INCLUDES}
	@echo Compiling $<
	${VERBOSE}${CXX} -c ${CXXFLAGS} $< -o $@ ${DEFINES}

# *****************************************************************************
# Create the Target
# *****************************************************************************
${TARGET}: ${TGT_SOURCE_OBJS}
	@echo Linking $@
	${VERBOSE}${LD} ${SLDFLAGS} -o ${OUTLIB}/${TARGET} $^ -L${OUTLIB} -lecmd -lpthread

# *****************************************************************************
# Include any global default rules
# *****************************************************************************
include ${ECMD_ROOT}/makefile.rules
