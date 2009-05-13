# $Header$
# Makefile for the ecmd Extensions

EXTENSION_NAME   := cmd

# Base info and default build rules
SUBDIR     := ext/${EXTENSION_NAME}/capi/
include ../../../makefile.rules

# This is Additional extension for the client api
SOURCE       := 

# These are additional includes for the client api
INCLUDES     :=

# Strip the cmd extension, can't include ourselves
EXTENSIONS := $(subst cmd,,${EXTENSIONS})
# Strip the scand extension, it doesn't have a commandline
EXTENSIONS := $(subst scand,,${EXTENSIONS})


VPATH        := ${VPATH}:${OBJROOT}/ecmd/${OBJDIR}$(foreach ext, ${EXTENSIONS},:${OBJROOT}/ext/${ext}/cmd/${OBJDIR}:${OBJROOT}/ext/${ext}/capi/${OBJDIR}:../../${ext}/capi:../../${ext}/cmd)
# We need to tack on a seperator to get the vpath right in makefile.capi
VPATH          := ${VPATH}:

#These are needed to define a space char for substitutions later on
empty :=
space := ${empty} ${empty}
VPATH := $(subst ${space},,${VPATH})

# These are includes not to be exported
INT_INCLUDES     := ${INT_INCLUDES} $(foreach ext, ${EXTENSIONS}, ${ext}Interpreter.H ${ext}ClientCapi.H ${ext}Structs.H)

CFLAGS       := ${CFLAGS} -I../../../capi -I../../../ecmd
#######################
# Extension Objects   #
#######################

# LINK_OBJS - Capi
LINK_OBJS := ${LINK_OBJS} $(foreach ext, ${EXTENSIONS}, $(shell /bin/ls ${OBJROOT}/ext/${ext}/capi/${OBJDIR}/*.o) )
# LINK_OBJS - Cmd
LINK_OBJS := ${LINK_OBJS} $(foreach ext, ${EXTENSIONS}, $(shell /bin/ls ${OBJROOT}/ext/${ext}/cmd/${OBJDIR}/*.o) )

# The Core Interpreter
LINK_OBJS := ${LINK_OBJS} $(shell /bin/ls ${OBJROOT}/ecmd/${OBJDIR}/*.o)

# We need to pull unwanted objects like ecmdMain.o and others from the list
REMOVE_OBJS := ${OBJROOT}/ecmd/${OBJDIR}/ecmdMain.o ${OBJROOT}/ecmd/${OBJDIR}/ecmdVersion.o
LINK_OBJS := $(filter-out ${REMOVE_OBJS},${LINK_OBJS})



# Include the common extension makefile to do all the work
# For this extension we don't want any autogenned source though
DISABLE_AUTOGEN = yes
include ../../template/capi/makefile.capi

test:
	echo ${VPATH}
