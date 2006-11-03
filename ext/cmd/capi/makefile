# Makefile for the ecmd Extensions
# Written by Chris Engel

# $Header$



EXTENSION_NAME   := cmd
OS           := $(shell uname)

# We need to do extra for Linux
ifeq (${OS},Linux)
  ifneq ($(strip $(shell uname -a |grep ppc)),)
    OS := Linux_ppc
  else
    OS := Linux_x86
  endif
endif

# This is Additional extension for the client api
SOURCE       := 

# These are additional includes for the client api
INCLUDES     :=



# Extensions to grab the interpreters from
ifeq ($(strip ${EXTENSIONS}),)
  EXTENSIONS   := cip cro eip gip zse mbo
endif

# Strip the cmd extension, can't include ourselves
EXTENSIONS := $(subst cmd,,${EXTENSIONS})

ifeq (${OS},Linux_x86)
  OBJDIR := obj_x86
endif
ifeq (${OS},Linux_ppc)
  OBJDIR := obj_ppc
endif
ifeq (${OS},AIX)
  OBJDIR := obj_aix
endif


VPATH        := ${VPATH}../../../ecmd/${OBJDIR} $(foreach ext, ${EXTENSIONS},:../../${ext}/cmd/${OBJDIR}:../../${ext}/capi/${OBJDIR}:../../${ext}/capi/export:../../${ext}/cmd/export)
# We need to tack on a seperator to get the vpath right in makefile.capi
VPATH          := ${VPATH}:

#These are needed to define a space char for substitutions later on
empty :=
space := ${empty} ${empty}
VPATH := $(subst ${space},,${VPATH})

# These are includes not to be exported
INT_INCLUDES     := ${INT_INCLUDES} $(foreach ext, ${EXTENSIONS}, ${ext}Interpreter.H ${ext}ClientCapi.H ${ext}Structs.H)

CFLAGS       := ${CFLAGS} -I../../../capi/export -I../../../ecmd
#######################
# Extension Objects   #
#######################

# LINK_OBJS - Capi
LINK_OBJS := ${LINK_OBJS} $(foreach ext, ${EXTENSIONS}, $(shell /bin/ls ../../${ext}/capi/${OBJDIR}/*.o) )
# LINK_OBJS - Cmd
LINK_OBJS := ${LINK_OBJS} $(foreach ext, ${EXTENSIONS}, $(shell /bin/ls ../../${ext}/cmd/${OBJDIR}/*.o) )

# CIP Extension - Capi
#LINK_OBJS := ${LINK_OBJS} $(shell /bin/ls ../../cip/capi/${OBJDIR}/*.o)
# CIP Extension - Cmd
#LINK_OBJS := ${LINK_OBJS} $(shell /bin/ls ../../cip/cmd/${OBJDIR}/*.o)

# EIP Extension - Capi
#LINK_OBJS := ${LINK_OBJS} $(shell /bin/ls ../../eip/capi/${OBJDIR}/*.o)
# EIP Extension - Cmd
#LINK_OBJS := ${LINK_OBJS} $(shell /bin/ls ../../eip/cmd/${OBJDIR}/*.o)

# CRO Extension - Capi
#LINK_OBJS := ${LINK_OBJS} $(shell /bin/ls ../../cro/capi/${OBJDIR}/*.o)
# CRO Extension - Cmd
#LINK_OBJS := ${LINK_OBJS} $(shell /bin/ls ../../cro/cmd/${OBJDIR}/*.o)

# GIP Extension - Capi
#LINK_OBJS := ${LINK_OBJS} $(shell /bin/ls ../../gip/capi/${OBJDIR}/*.o)
# GIP Extension - Cmd
#LINK_OBJS := ${LINK_OBJS} $(shell /bin/ls ../../gip/cmd/${OBJDIR}/*.o)

# ZSE Extension - Capi
#LINK_OBJS := ${LINK_OBJS} $(shell /bin/ls ../../zse/capi/${OBJDIR}/*.o)
# ZSE Extension - Cmd
#LINK_OBJS := ${LINK_OBJS} $(shell /bin/ls ../../zse/cmd/${OBJDIR}/*.o)


# The Core Interpreter
LINK_OBJS := ${LINK_OBJS} $(shell /bin/ls ../../../ecmd/${OBJDIR}/*.o)

# We need to pull unwanted objects like ecmdMain.o and others from the list
REMOVE_OBJS := ../../../ecmd/${OBJDIR}/ecmdMain.o ../../../ecmd/${OBJDIR}/t.o ../../../ecmd/${OBJDIR}/ecmdVersion.o
LINK_OBJS := $(filter-out ${REMOVE_OBJS},${LINK_OBJS})



# Include the common extension makefile to do all the work
# For this extension we don't want any autogenned source though
DISABLE_AUTOGEN = yes
include ../../template/capi/makefile.capi

test:
	echo ${VPATH}
