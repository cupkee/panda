#
#

BASE = ${PWD}

MAKE_DIR  = ${BASE}/make
TEST_DIR  = ${BASE}/test

export BASE
export PREFIX

.PHONY: all test cunit lang example

all: lang

lang:
	@printf "[Build] lang\n"
	@${MAKE} -C lang -f ${MAKE_DIR}/Makefile.pub

cunit:
	@printf "[Build] cunit\n"
	@${MAKE} -C cunit -f ${MAKE_DIR}/Makefile.pub

test: cunit lang
	@printf "[Build] test\n"
	@${RM} ${TEST_DIR}/test
	@${MAKE} -C test -f ${MAKE_DIR}/Makefile.pub
	@${TEST_DIR}/test

example: lang
	@printf "[Build] example\n"
	@${MAKE} -C example -f ${MAKE_DIR}/Makefile.pub

clean:
	@${MAKE} -C cunit clean -f ${MAKE_DIR}/Makefile.pub
	@${MAKE} -C test  clean -f ${MAKE_DIR}/Makefile.pub
	@${MAKE} -C example clean -f ${MAKE_DIR}/Makefile.pub
	@${MAKE} -C lang  clean -f ${MAKE_DIR}/Makefile.pub

