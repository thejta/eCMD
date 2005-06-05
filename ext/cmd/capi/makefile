# Makefile for the ecmd Extensions
# Written by Chris Engel

# $Header$



EXTENSION_NAME   := cmd
OS           := $(shell uname)


# This is Additional extension for the client api
SOURCE       := 

# These are additional includes for the client api
INCLUDES     :=



# Extensions to grab the interpreters from
EXTENSIONS   := cip cro eip gip

ifeq (${OS},Linux)
  VPATH        := ${VPATH}../../../ecmd/linux $(foreach ext, ${EXTENSIONS},:../../${ext}/cmd/linux:../../${ext}/capi/linux)
endif
ifeq (${OS},AIX)
  VPATH        := ${VPATH}../../../ecmd/aix $(foreach ext, ${EXTENSIONS},:../../${ext}/cmd/aix:../../${ext}/capi/aix)
endif
# We need to tack on a seperator to get the vpath right in makefile.capi
VPATH          := ${VPATH}:

#These are needed to define a space char for substitutions later on
empty :=
space := ${empty} ${empty}
VPATH := $(subst ${space},,${VPATH})

INCLUDES     := ${INCLUDES} $(foreach ext, ${EXTENSIONS}, ${ext}Interpreter.H ${ext}ClientCapi.H ${ext}Structs.H)

CFLAGS       := ${CFLAGS} -I../../../capi/export -I../../../ecmd
#######################
# Extension Objects   #
#######################
# CIP Extension - Capi
LINK_OBJS := ${LINK_OBJS} cipClientCapi.o cipClientCapiFunc.o
# CIP Extension - Cmd
LINK_OBJS := ${LINK_OBJS} cipInterpreter.o cipProcUser.o

# EIP Extension - Capi
LINK_OBJS := ${LINK_OBJS} eipClientCapi.o eipClientCapiFunc.o
# EIP Extension - Cmd
LINK_OBJS := ${LINK_OBJS} eipInterpreter.o eipProcUser.o

# CRO Extension - Capi
LINK_OBJS := ${LINK_OBJS} croClientCapi.o croClientCapiFunc.o
# CRO Extension - Cmd
LINK_OBJS := ${LINK_OBJS} croInterpreter.o

# GIP Extension - Capi
LINK_OBJS := ${LINK_OBJS} gipClientCapi.o gipClientCapiFunc.o
# GIP Extension - Cmd
LINK_OBJS := ${LINK_OBJS} gipInterpreter.o


# The Core Interpreter
LINK_OBJS := ${LINK_OBJS} ecmdInterpreter.o ecmdProcUser.o ecmdJtagUser.o ecmdMemUser.o ecmdScomUser.o ecmdRingUser.o ecmdSpyUser.o ecmdMiscUser.o ecmdIstepUser.o ecmdSimUser.o ecmdCommandUtils.o ecmdQueryUser.o ecmdArrayUser.o ecmdVpdUser.o



# Include the common extension makefile to do all the work
# For this extension we don't want any autogenned source though
DISABLE_AUTOGEN = yes
include ../../template/capi/makefile.capi

test:
	echo ${VPATH}
