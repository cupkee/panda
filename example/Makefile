lib_NAMES = example
bin_NAMES = compile dump repl panda

example_SRCS = sal.c native.c
example_CPPFLAGS = -I.. -Wall -Werror
example_CFLAGS   = -g

compile_SRCS = compile.c
compile_CPPFLAGS = -I..
compile_CFLAGS   = -g
compile_LDFLAGS  = -L. -L../lang -lexample -llang

dump_SRCS = dump.c
dump_CPPFLAGS = -I..
dump_CFLAGS   = -g
dump_LDFLAGS  = -L. -L../lang -lexample -llang

repl_SRCS = interactive.c
repl_CPPFLAGS = -I..
repl_CFLAGS   = -g
repl_LDFLAGS  = -L. -L../lang -lexample -llang -lreadline

panda_SRCS = interpreter.c
panda_CPPFLAGS = -I..
panda_CFLAGS   = -g
panda_LDFLAGS  = -L. -L../lang -lexample -llang

