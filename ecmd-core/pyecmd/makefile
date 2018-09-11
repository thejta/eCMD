# Makefile for pyeCMD, the Pythonic eCMD wrapper

# *****************************************************************************
# Include the common base makefile
# *****************************************************************************
include ../../makefile.base

GENERATED_PY := ${OUTPYECMD}/generated.py
CONSTANTS_PY := ${OUTPYECMD}/constants.py
COPIED_PY := base.py __init__.py ecmdbitstring.py
HEADERS_TO_PARSE := $(addprefix ${ECMD_CORE}/capi/, ecmdClientCapi.H ecmdSharedUtils.H ecmdUtils.H)
PYTHON_INSTALL_PATH := ${INSTALL_PATH}/${TARGET_ARCH}/python
PYECMD_INSTALL_PATH := ${PYTHON_INSTALL_PATH}/pyecmd

# *****************************************************************************
# The Main Targets
# *****************************************************************************
# The run-all rule is defined in makefile.rules
all:
	${run-all}

doxygen:
	${VERBOSE}${MAKE} -C ../.. doxygen-create ${MAKEFLAGS} --no-print-directory

generate: ${GENERATED_PY} ${CONSTANTS_PY}

build:

install:
	@echo "Creating ${PYECMD_INSTALL_PATH} dir ..."
	@mkdir -p ${PYECMD_INSTALL_PATH}
	@echo " "

	@echo "Installing Pythonic eCMD Wrapper pyeCMD ${PYECMD_INSTALL_PATH} ..."
	cp ${GENERATED_PY} ${CONSTANTS_PY} ${COPIED_PY} ${PYECMD_INSTALL_PATH}
	cp bitstring.py ${PYTHON_INSTALL_PATH}
	@echo " "

test:
	@echo "***** If you see python load errors this build of the python module is invalid ****"
	@ln -sf `pwd`/*.py ${OUTPYECMD}
	@ECMD_DLL_FILE=${OUTLIB}/stub.dll PYTHONPATH=${OUTPY}:${OUTPYECMD}/.. LD_LIBRARY_PATH=${OUTLIB}:${OUTPY} ${ECMDPYTHONBIN} test_api.py

test-interactive:
	@ECMD_DLL_FILE=${OUTLIB}/stub.dll PYTHONPATH=${OUTPY}:${OUTPYECMD}/.. LD_LIBRARY_PATH=${OUTLIB}:${OUTPY} ${ECMDPYTHONBIN}

# *****************************************************************************
# Generate most of the wrapper Python code
# *****************************************************************************
${GENERATED_PY}: $(HEADERS_TO_PARSE)
	@echo "Generating $@ ..."
	@./generate_pyecmd.py $^ > $@

${CONSTANTS_PY}: ${OUTPY}/ecmd.py
	@echo "Generating $@ ..."
	@{ \
	    echo -n "from ecmd import "; \
	    grep -E "([[:upper:][:digit:]_]+)\s+=\s+_ecmd\.\1" $< | cut -d' ' -f1 | tr '\n' ',' | sed 's/,$$//'; \
	 } > $@

# *****************************************************************************
# Include any global default rules
# *****************************************************************************
include ${ECMD_ROOT}/makefile.rules
