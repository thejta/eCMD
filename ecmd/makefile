# Makefile for the ecmd Capi
# Written by Chris Engel

# $Header$

OS           := $(shell uname)

VPATH        := ../capi/export
INCLUDES     := ecmdClientCapi.H  ecmdDataBuffer.H  ecmdReturnCodes.H ecmdStructs.H
INT_INCLUDES := ecmdInterpreter.H
SOURCE       := ecmdMain.C ecmdInterpreter.C ecmdScomUser.C ecmdCommandUtils.C ecmdRingUser.C ecmdQueryUser.C ecmdSpyUser.C ecmdSimUser.C ecmdArrayUser.C ecmdJtagUser.C
DEFINES      := 
CFLAGS       := -I. -I../capi/export -g 


# *****************************************************************************
# The Linux Setup stuff
# *****************************************************************************
ifeq (${OS},Linux)
  SUBDIR  := linux/
  CC      := g++
  TARGET  := ecmd_x86
  LD      := ${CC}
  LDFLAGS := -ldl -L../capi/export -lecmd_x86
  LINK_OBJS := ../capi/export/ecmdClientCapi_x86.a
  CFLAGS := ${CFLAGS} -ftemplate-depth-30
  GPATH   := ${SUBDIR}
endif

# *****************************************************************************
# The Aix Setup stuff
# *****************************************************************************
ifeq (${OS},AIX)
  SUBDIR  := aix/
  CC      := xlC -+
  TARGET  := ecmd_aix
  LD      := ${CC}
  LDFLAGS := -brtl -bloadmap:PARM -w -L../capi/export -lecmd_aix
  LINK_OBJS := ../capi/export/ecmdClientCapi_aix.a
  CFLAGS  := ${CFLAGS}  # -qstaticinline
endif

VPATH     := ${VPATH}:${SUBDIR}

# *****************************************************************************
# The Main Targets
# *****************************************************************************
all: dir ${TARGET}
	@touch t.o t_x86 t_aix t_ppc
	@mv *.o *_x86 *_aix *_ppc  ${SUBDIR}
	@echo "Exporting eCMD Client to export/ ..."
	@cp ${SUBDIR}${TARGET} export/

clean:
	rm -rf ${SUBDIR}

dir:
	@mkdir -p ${SUBDIR}
	@mkdir -p export



# *****************************************************************************
# Object Build Targets
# *****************************************************************************
SOURCE_OBJS  = $(basename $(SOURCE))
SOURCE_OBJS := $(addsuffix .o, $(SOURCE_OBJS))

# *****************************************************************************
# Compile code for the common C++ objects if their respective
# code has been changed.  Or, compile everything if a header
# file has changed.
# *****************************************************************************
$(SOURCE_OBJS): %.o : %.C ${INCLUDES} ${INT_INCLUDES}
	$(CC) -c $(CFLAGS) $< -o $@ $(DEFINES)


# *****************************************************************************
# Create the Client 
# *****************************************************************************
${TARGET}: ${SOURCE_OBJS}
	${LD} ${LDFLAGS} $^ ${LINK_OBJS} -o $@ 


