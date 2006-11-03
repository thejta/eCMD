# Makefile for the ecmd Extensions
# Written by Chris Engel

# $Header$

ECMD_ROOT    := ${PWD}/../../../
include ${ECMD_ROOT}/makefile.rules

EXTENSION_NAME   := cmd

# This is Additional extension for the client api
SOURCE       := 

# These are additional includes for the client api
INCLUDES     :=



# Strip the cmd extension, can't include ourselves
EXTENSIONS := $(subst cmd,,${EXTENSIONS})
# Strip the scand extension, it doesn't have a commandline
EXTENSIONS := $(subst scand,,${EXTENSIONS})


VPATH        := ${VPATH}../../../ecmd/${SUBDIR} $(foreach ext, ${EXTENSIONS},:../../${ext}/cmd/${SUBDIR}:../../${ext}/capi/${SUBDIR}:../../${ext}/capi/export:../../${ext}/cmd/export)
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
LINK_OBJS := ${LINK_OBJS} $(foreach ext, ${EXTENSIONS}, $(shell /bin/ls ../../${ext}/capi/${SUBDIR}/*.o) )
# LINK_OBJS - Cmd
LINK_OBJS := ${LINK_OBJS} $(foreach ext, ${EXTENSIONS}, $(shell /bin/ls ../../${ext}/cmd/${SUBDIR}/*.o) )

# CIP Extension - Capi
#LINK_OBJS := ${LINK_OBJS} $(shell /bin/ls ../../cip/capi/${SUBDIR}/*.o)
# CIP Extension - Cmd
#LINK_OBJS := ${LINK_OBJS} $(shell /bin/ls ../../cip/cmd/${SUBDIR}/*.o)

# EIP Extension - Capi
#LINK_OBJS := ${LINK_OBJS} $(shell /bin/ls ../../eip/capi/${SUBDIR}/*.o)
# EIP Extension - Cmd
#LINK_OBJS := ${LINK_OBJS} $(shell /bin/ls ../../eip/cmd/${SUBDIR}/*.o)

# CRO Extension - Capi
#LINK_OBJS := ${LINK_OBJS} $(shell /bin/ls ../../cro/capi/${SUBDIR}/*.o)
# CRO Extension - Cmd
#LINK_OBJS := ${LINK_OBJS} $(shell /bin/ls ../../cro/cmd/${SUBDIR}/*.o)

# GIP Extension - Capi
#LINK_OBJS := ${LINK_OBJS} $(shell /bin/ls ../../gip/capi/${SUBDIR}/*.o)
# GIP Extension - Cmd
#LINK_OBJS := ${LINK_OBJS} $(shell /bin/ls ../../gip/cmd/${SUBDIR}/*.o)

# ZSE Extension - Capi
#LINK_OBJS := ${LINK_OBJS} $(shell /bin/ls ../../zse/capi/${SUBDIR}/*.o)
# ZSE Extension - Cmd
#LINK_OBJS := ${LINK_OBJS} $(shell /bin/ls ../../zse/cmd/${SUBDIR}/*.o)


# The Core Interpreter
LINK_OBJS := ${LINK_OBJS} $(shell /bin/ls ../../../ecmd/${SUBDIR}/*.o)

# We need to pull unwanted objects like ecmdMain.o and others from the list
REMOVE_OBJS := ../../../ecmd/${SUBDIR}/ecmdMain.o ../../../ecmd/${SUBDIR}/t.o ../../../ecmd/${SUBDIR}/ecmdVersion.o
LINK_OBJS := $(filter-out ${REMOVE_OBJS},${LINK_OBJS})



# Include the common extension makefile to do all the work
# For this extension we don't want any autogenned source though
DISABLE_AUTOGEN = yes
include ../../template/capi/makefile.capi

test:
	echo ${VPATH}
