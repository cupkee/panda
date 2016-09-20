#
#

BASE = ${PWD}

MAKE_DIR  = ${BASE}/make
CUNIT_DIR = ${BASE}/cunit
SAL_DIR   = ${BASE}/sal
LANG_DIR  = ${BASE}/lang
TEST_DIR  = ${BASE}/test

export BASE
export PREFIX


#CPPFLAGS = -I${BASE} -Wall -Werror
#CFLAGS  = -g
#LDFLAGS = -L${CUNIT_DIR} -L${LANG_DIR} -L${SAL_DIR}

#export CPPFLAGS
#export CFLAGS
#export LDFLAGS

.PHONY: all test cunit lang sal panda

all: lang

cunit:
	@printf "[Build] cunit\n"
	@${MAKE} -C cunit -f ${MAKE_DIR}/Makefile.pub

sal:
	@printf "[Build] sal\n"
	@${MAKE} -C sal -f ${MAKE_DIR}/Makefile.pub

lang:
	@printf "[Build] lang\n"
	@${MAKE} -C lang -f ${MAKE_DIR}/Makefile.pub

test: cunit lang panda
	@printf "[Build] test\n"
	@${RM} ${TEST_DIR}/test
	@${MAKE} -C test -f ${MAKE_DIR}/Makefile.pub
	@${TEST_DIR}/test

panda: lang sal
	@printf "[Build] panda\n"
	@${MAKE} -C panda -f ${MAKE_DIR}/Makefile.pub

clean:
	@${MAKE} -C cunit clean -f ${MAKE_DIR}/Makefile.pub
	@${MAKE} -C sal   clean -f ${MAKE_DIR}/Makefile.pub
	@${MAKE} -C lang  clean -f ${MAKE_DIR}/Makefile.pub
	@${MAKE} -C test  clean -f ${MAKE_DIR}/Makefile.pub
	@${MAKE} -C panda  clean -f ${MAKE_DIR}/Makefile.pub

