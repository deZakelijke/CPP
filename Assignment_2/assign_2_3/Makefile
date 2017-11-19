PROGNAME = assign2_3
SRCFILES = ring_network_broadcast.c

RUNARGS = 3 7 Message    #sendeing node
PROCESSESPERNODE = 6
NODES = 4

PRUNARGS = -v -np $(NODES) -$(PROCESSESPERNODE) \
		   -sge-script $$PRUN_ETC/prun-openmpi

CC = mpicc

WARNFLAGS = -Wall -Werror-implicit-function-declaration -Wshadow \
		  -Wstrict-prototypes -pedantic-errors
CFLAGS = -std=c99 -ggdb -O2 $(WARNFLAGS) -D_POSIX_C_SOURCE=200112
LFLAGS = -lm -lrt

OBJFILES = $(patsubst %.c, %.o, $(SRCFILES))

$(PROGNAME): $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

run: $(PROGNAME)
	prun $(PRUNARGS) $(PROGNAME) $(RUNARGS)

runlocal: $(PROGNAME)
	mpirun -n $(PROCESSESPERNODE) $(PROGNAME) $(RUNARGS)

