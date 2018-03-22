## GPLv2 License
##
## Copyright (C) 2016-2018 Lixing Ding <ding.lixing@gmail.com>
##
## This program is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License
## as published by the Free Software Foundation; either version 2
## of the License, or (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

export BASE = ${PWD}
export PREFIX

MAKE_DIR  = ${BASE}/make

LANG_BUILD_DIR = ${BASE}/build/lang
TEST_BUILD_DIR = ${BASE}/build/test
EXAMPLE_BUILD_DIR = ${BASE}/build/example


.PHONY: all test cunit lang example build pre_lang pre_test pre_example

all: lang

pre_lang:
	@mkdir -p ${LANG_BUILD_DIR}

pre_test:
	@mkdir -p ${TEST_BUILD_DIR}

pre_example:
	@mkdir -p ${EXAMPLE_BUILD_DIR}

lang: pre_lang
	@printf "[Build] lang\n"
	@${MAKE} -C ${LANG_BUILD_DIR} -f ${MAKE_DIR}/lang.mk

test: lang pre_test
	@printf "[Build] test\n"
	@${RM} ${TEST_BUILD_DIR}/test
	@${MAKE} -C ${TEST_BUILD_DIR} -f ${MAKE_DIR}/test.mk
	@${TEST_BUILD_DIR}/test

example: lang pre_example
	@printf "[Build] example\n"
	@${MAKE} -C ${EXAMPLE_BUILD_DIR} -f ${MAKE_DIR}/example.mk

clean:
	@${RM} -rf build

