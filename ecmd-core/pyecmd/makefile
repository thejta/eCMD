# Makefile for pyeCMD, the Pythonic eCMD wrapper

# *****************************************************************************
# Include the common base makefile
# *****************************************************************************
include ../../makefile.base

GENERATED_PY := ${OUTPYECMD}/generated.py
CONSTANTS_PY := ${OUTPYECMD}/constants.py
COPIED_PY := base.py __init__.py ecmdbitstring.py
DOXYGEN_TO_PARSE := ecmdClientPyapi_8H.html ecmdSharedUtils_8H.html ecmdUtils_8H.html
PYTHON_INSTALL_PATH := ${INSTALL_PATH}/${TARGET_ARCH}/python
PYECMD_INSTALL_PATH := ${PYTHON_INSTALL_PATH}/pyecmd

# *****************************************************************************
# The Main Targets
# *****************************************************************************
# The run-all rule is defined in makefile.rules
all:
	${run-all}

doxygen:
	${VERBOSE}${MAKE} -C ../.. doxygen ${MAKEFLAGS} --no-print-directory

generate: doxygen ${GENERATED_PY} ${CONSTANTS_PY}

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

# *****************************************************************************
# Generate most of the wrapper Python code
# *****************************************************************************
${GENERATED_PY}: $(addprefix ${DOXYGEN_PYAPI_PATH}/html/, $(DOXYGEN_TO_PARSE))
	@echo "Generating $@ ..."
	@./generate_pyecmd.py $^ > $@

${CONSTANTS_PY}: ${OUTPY}/ecmd.py
	@echo "Generating $@ ..."
	@{ \
	    echo -n "from ecmd import "; \
	    grep -E "([A-Z0-9_]+)\s+=\s+_ecmd\.\1" $< | cut -d' ' -f1 | tr '\n' ',' | sed 's/,$$//'; \
	 } > $@

# *****************************************************************************
# Include any global default rules
# *****************************************************************************
include ${ECMD_ROOT}/makefile.rules
