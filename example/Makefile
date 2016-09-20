lib_NAMES = example
bin_NAMES = compile dump repl panda

example_SRCS = sal.c native.c
example_CPPFLAGS = -I${BASE} -Wall -Werror
example_CFLAGS   = -g

compile_SRCS = compile.c
compile_CPPFLAGS = -I${BASE}
compile_CFLAGS   = -g
compile_LDFLAGS  = -L. -L${BASE}/lang -lexample -llang

dump_SRCS = dump.c
dump_CPPFLAGS = -I${BASE}
dump_CFLAGS   = -g
dump_LDFLAGS  = -L. -L${BASE}/lang -lexample -llang

repl_SRCS = interactive.c
repl_CPPFLAGS = -I${BASE}
repl_CFLAGS   = -g
repl_LDFLAGS  = -L. -L${BASE}/lang -lexample -llang -lreadline

panda_SRCS = interpreter.c
panda_CPPFLAGS = -I${BASE}
panda_CFLAGS   = -g
panda_LDFLAGS  = -L. -L${BASE}/lang -lexample -llang

