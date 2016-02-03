# Makefile for the ecmd Extensions

EXTENSION_NAME   := cmd

# *****************************************************************************
# Define base info and include any global variables
# *****************************************************************************
SUBDIR     := ext/${EXTENSION_NAME}/capi/
include ../../../../makefile.vars

# This is Additional extension for the client api
SOURCE       := 

# These are additional includes for the client api
INCLUDES     :=

# Strip the cmd extension, can't include ourselves
EXTENSIONS := $(subst cmd,,${EXTENSIONS})
# Strip the scand extension, it doesn't have a commandline
EXTENSIONS := $(subst scand,,${EXTENSIONS})
# Strip the fapi extension, it doesn't have a commandline
EXTENSIONS := $(subst fapi2,,${EXTENSIONS})
EXTENSIONS := $(subst fapi,,${EXTENSIONS})

VPATH        := ${VPATH}:${OBJPATH})
# We need to tack on a seperator to get the vpath right in makefile.capi
VPATH          := ${VPATH}:

VPATH := $(subst ${space},,${VPATH})

# These are includes not to be exported
INT_INCLUDES     := ${INT_INCLUDES} $(foreach ext, ${EXTENSIONS}, ${ext}Interpreter.H ${ext}ClientCapi.H ${ext}Structs.H)

CFLAGS       := ${CFLAGS} -I../../../capi -I../../../cmd
#######################
# Extension Objects   #
#######################

# LINK_OBJS - Capi
#LINK_OBJS := ${LINK_OBJS} $(foreach ext, ${EXTENSIONS}, $(shell /bin/ls ${OBJROOT}/ext/${ext}/capi/${OBJDIR}/*.o) )
LINK_OBJS := ${LINK_OBJS} $(foreach ext, ${EXTENSIONS}, $(shell ar t ${OUTLIB}/${ext}ClientCapi.a))
# LINK_OBJS - Cmd
#LINK_OBJS := ${LINK_OBJS} $(foreach ext, ${EXTENSIONS}, $(shell /bin/ls ${OBJROOT}/ext/${ext}/cmd/${OBJDIR}/*.o) )
LINK_OBJS := ${LINK_OBJS} $(foreach ext, ${EXTENSIONS}, $(shell ar t ${OUTLIB}/${ext}CmdInterpreter.a)) 

# The Core Interpreter
LINK_OBJS := ${LINK_OBJS} $(shell ar t ${OUTLIB}/ecmdClientCmd.a)

# Include the common extension makefile to do all the work
# For this extension we don't want any autogenned source though
DISABLE_AUTOGEN = yes
include ../../template/capi/makefile.capi
