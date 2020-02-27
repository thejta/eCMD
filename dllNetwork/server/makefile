# Makefile for a network DLL.

# *****************************************************************************
# Include the common base makefile
# *****************************************************************************
include ../../makefile.base

# *****************************************************************************
# The Common Setup stuff
# *****************************************************************************
TARGET       := server1p
CXXFLAGS     += -I ${ECMD_ROOT}/ecmd-core/capi -I ${ECMD_ROOT}/ecmd-core/ext/cip/capi -I ${ECMD_ROOT}/ecmd-core/dll -I${SRCPATH}
CXXFLAGS     += -Iadals -Iadals/scom -Iadals/scan -Iadals/sbefifo -Iadals/mbx -Iadals/iic_master
VPATH        := ${VPATH}:${ECMD_ROOT}/ecmd-core/capi:${ECMD_ROOT}/ecmd-core/ext/cip/capi:${ECMD_ROOT}/ecmd-core/dll:${SRCPATH}:adals:adals/scom:adals/scan:adals/sbefifo:adals/mbx:adals/iic_master

# *****************************************************************************
# Setup all the files going into the build
# *****************************************************************************

# The ECMD_INCLUDES are files provided by eCMD that if changed, we would want to recompile on
ECMD_INCLUDES := ecmdDataBufferBase.H
ECMD_INCLUDES += ecmdDataBuffer.H
ECMD_INCLUDES += ecmdStructs.H
ECMD_INCLUDES += cipStructs.H

# The source files and includes in our local dirs that are going into the build
TGT_INCLUDES := Instruction.H
TGT_INCLUDES += InstructionStatus.H
TGT_INCLUDES += ServerFSIInstruction.H
TGT_INCLUDES += FSIInstruction.H
TGT_INCLUDES += ServerGPIOInstruction.H
TGT_INCLUDES += GPIOInstruction.H
TGT_INCLUDES += ServerI2CInstruction.H
TGT_INCLUDES += I2CInstruction.H
TGT_INCLUDES += ControlInstruction.H
TGT_INCLUDES += InstructionFlag.H
TGT_INCLUDES += ecmdDataBuffer.H
TGT_INCLUDES += ecmdDataBufferBase.H
TGT_INCLUDES += OutputLite.H
TGT_INCLUDES += PNORInstruction.H
TGT_INCLUDES += fd_impl.H
TGT_INCLUDES += ServerSBEFIFOInstruction.H
TGT_INCLUDES += SBEFIFOInstruction.H
TGT_INCLUDES += ServerBrkptInstruction.H
TGT_INCLUDES += BrkptInstruction.H
TGT_INCLUDES += git_version.H

TGT_SOURCE := Instruction.C
TGT_SOURCE += InstructionStatus.C
TGT_SOURCE += ServerFSIInstruction.C
TGT_SOURCE += FSIInstruction.C
TGT_SOURCE += ServerGPIOInstruction.C
TGT_SOURCE += GPIOInstruction.C
TGT_SOURCE += ServerI2CInstruction.C
TGT_SOURCE += I2CInstruction.C
TGT_SOURCE += ControlInstruction.C
TGT_SOURCE += InstructionFlag.C
TGT_SOURCE += ecmdDataBuffer.C
TGT_SOURCE += ecmdDataBufferBase.C
TGT_SOURCE += OutputLite.C
TGT_SOURCE += PNORInstruction.C
TGT_SOURCE += fd_impl.C
TGT_SOURCE += ServerSBEFIFOInstruction.C
TGT_SOURCE += SBEFIFOInstruction.C
TGT_SOURCE += BrkptInstruction.C
TGT_SOURCE += git_version.C
TGT_SOURCE += cipStructs.C
TGT_SOURCE += ecmdStructs.C
TGT_SOURCE += ecmdWriteTarget.C
TGT_SOURCE += server1p.C

GENERATE_FILES := git_version.C

TGT_SOURCE_C := adal_base.c
TGT_SOURCE_C += adal_scom.c
TGT_SOURCE_C += adal_scan.c
TGT_SOURCE_C += adal_sbefifo.c
TGT_SOURCE_C += adal_mbx.c
TGT_SOURCE_C += adal_iic.c

-include ${ECMD_ROOT}/*/server_ext/makefile.config

# Combine all the includes into one variable for the build
INCLUDES      := ${ECMD_INCLUDES} ${TGT_INCLUDES}

DEFINES += -DREMOVE_SIM -DOTHER_USE -DCRONUS_SERVER_SIDE -D_FILE_OFFSET_BITS=64 -DCRONUS_FSP2 -DNO_GFW -DSERVER_TYPE=SERVER_BMC

LDFLAGS += -ldl -lpthread -lz

# *****************************************************************************
# The Main Targets
# *****************************************************************************
# The run-all rule is defined in makefile.rules
all:
	${run-all}

generate: ${GENERATE_FILES}

build: ${TARGET}

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

TGT_SOURCE_C_OBJS  = $(basename ${TGT_SOURCE_C})
TGT_SOURCE_C_OBJS := $(addprefix ${OBJPATH}, ${TGT_SOURCE_C_OBJS})
TGT_SOURCE_C_OBJS := $(addsuffix .o, ${TGT_SOURCE_C_OBJS})

# *****************************************************************************
# Compile code for the common C++ objects if their respective
# code has been changed.  Or, compile everything if a header
# file has changed.
# *****************************************************************************
${TGT_SOURCE_OBJS}: ${OBJPATH}%.o : %.C ${INCLUDES}
	@echo Compiling $<
	${VERBOSE}${CXX} -c ${CXXFLAGS} -std=c++17 $< -o $@ ${DEFINES}

${TGT_SOURCE_C_OBJS}: ${OBJPATH}%.o : %.c ${INCLUDES}
	@echo Compiling $<
	${VERBOSE}${CXX} -c ${CXXFLAGS} $< -o $@ ${DEFINES}

# *****************************************************************************
# Create a .C file containing the git hash to print out as part of the
# serverversion. If we are building from a .zip download, the .git
# folder doesn't exist. Create empty targets for the .git folder so
# make doesn't try to create them. If we build in this situation, the
# version is an empty string, which is the best we can do.
# *****************************************************************************
../../.git/HEAD:
../../.git/index:
git_version.C: ../../.git/HEAD ../../.git/index
	@echo "const char *git_version = \"$(shell git describe --dirty --always --tags)\";" > ${SRCPATH}/$@

# *****************************************************************************
# Create the Target
# *****************************************************************************
${TARGET}: ${TGT_SOURCE_OBJS} ${TGT_SOURCE_C_OBJS}
	@echo Linking $@
	${VERBOSE}${LD} -o ${OUTLIB}/${TARGET} $^ ${LDFLAGS}

# *****************************************************************************
# Include any global default rules
# *****************************************************************************
include ${ECMD_ROOT}/makefile.rules

-include ${ECMD_ROOT}/*/server_ext/makefile.rules
