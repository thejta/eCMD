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

VPATH        := ${VPATH}:${OBJPATH}$(foreach ext,${EXT_CMD},:${EXT_${ext}_PATH}/capi:${EXT_${ext}_PATH}/cmd)
# The foreach creates spaces in the entries, take them out
VPATH        := $(subst ${space},,${VPATH})

# These are includes not to be exported
INT_INCLUDES := ${INT_INCLUDES} $(foreach ext, ${EXT_CMD}, ${ext}Interpreter.H ${ext}ClientCapi.H ${ext}Structs.H)

CFLAGS       := ${CFLAGS} -I${ECMD_CORE}/cmd $(foreach ext, ${EXT_CMD}, -I${EXT_${ext}_PATH}/capi -I${EXT_${ext}_PATH}/cmd)

#######################
# Extension Objects   #
#######################
# The archives the various builds create contain all the .o's we need in the command archive
# The issue is we can't just straight include them because then we end up with an archive of archives
# So we list all the .o's in each archive (ar t) so LINK_OBJS ends up with a list of .o's
# Then when LINK_OBJS is actually used, it pulls all the .o's from OBJPATH
# We can't just blanket include all OBJPATH .o's because everything gets dumped there
# LINK_OBJS - capi
LINK_OBJS := ${LINK_OBJS} $(foreach ext, ${EXT_CMD}, $(shell ar t ${OUTLIB}/${ext}ClientCapi.a))
# LINK_OBJS - cmd
LINK_OBJS := ${LINK_OBJS} $(foreach ext, ${EXT_CMD}, $(shell ar t ${OUTLIB}/${ext}CmdInterpreter.a)) 
# The Core Interpreter
LINK_OBJS := ${LINK_OBJS} $(shell ar t ${OUTLIB}/ecmdClientCmd.a)

# Include the common extension makefile to do all the work
# For this extension we don't want any autogenned source though
DISABLE_AUTOGEN = yes
include ${EXT_TEMPLATE_PATH}/capi/makefile.capi
