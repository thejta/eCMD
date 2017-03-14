# Makefile for a network DLL.

# *****************************************************************************
# Include the common base makefile
# *****************************************************************************
include ../../makefile.base

# *****************************************************************************
# The Common Setup stuff
# *****************************************************************************
TARGET       := server1p
CFLAGS       := ${CFLAGS} -I ${ECMD_ROOT}/ecmd-core/capi -I ${ECMD_ROOT}/ecmd-core/dll -I${SRCPATH}
CFLAGS       += -Iadals -Iadals/scom -Iadals/scan -Iadals/sbefifo -Iadals/mbx -Iadals/iic_master
VPATH        := ${VPATH}:${ECMD_ROOT}/ecmd-core/capi:${ECMD_ROOT}/ecmd-core/dll:${SRCPATH}:adals/scom:adals/scan:adals/sbefifo:adals/mbx:adals/iic_master

# *****************************************************************************
# Setup all the files going into the build
# *****************************************************************************

# The ECMD_INCLUDES are files provided by eCMD that if changed, we would want to recompile on
ECMD_INCLUDES := ecmdDataBufferBase.H
ECMD_INCLUDES += ecmdDataBuffer.H

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
TGT_SOURCE += server1p.C

TGT_SOURCE_C := adal_scom.c
TGT_SOURCE_C += adal_scan.c
TGT_SOURCE_C += adal_sbefifo.c
TGT_SOURCE_C += adal_mbx.c
TGT_SOURCE_C += adal_iic.c

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

generate:
  # Do nothing

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
	${VERBOSE}${CC} -c ${CFLAGS} $< -o $@ ${DEFINES}

${TGT_SOURCE_C_OBJS}: ${OBJPATH}%.o : %.c ${INCLUDES}
	@echo Compiling $<
	${VERBOSE}${CC} -c ${CFLAGS} $< -o $@ ${DEFINES}

# *****************************************************************************
# Create the Target
# *****************************************************************************
${TARGET}: ${TGT_SOURCE_OBJS} ${TGT_SOURCE_C_OBJS}
	@echo Linking $@
	${VERBOSE}${LD} ${LDFLAGS} -o ${OUTLIB}/${TARGET} $^

# *****************************************************************************
# Include any global default rules
# *****************************************************************************
include ${ECMD_ROOT}/makefile.rules
