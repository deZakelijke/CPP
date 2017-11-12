PROGNAME = assign1_2
SRCFILES = assign1_2.c file.c timer.c simulate.c
TARNAME = assign1_2.tgz

RUNARGS_1 = 1000000 100000 1 # i_max t_max num_threads, increase this when testing on the DAS4!
RUNARGS_2 = 1000000 100000 2 # i_max t_max num_threads, increase this when testing on the DAS4!
RUNARGS_4 = 1000000 100000 4 # i_max t_max num_threads, increase this when testing on the DAS4!
RUNARGS_6 = 1000000 100000 6 # i_max t_max num_threads, increase this when testing on the DAS4!
RUNARGS_8 = 1000000 100000 8 # i_max t_max num_threads, increase this when testing on the DAS4!

IMAGEVIEW = display
CC = gcc

WARNFLAGS = -Wall -Werror-implicit-function-declaration -Wshadow \
		  -Wstrict-prototypes -pedantic-errors
CFLAGS = -std=c99 -ggdb -O2 $(WARNFLAGS) -D_POSIX_C_SOURCE=200112 -fopenmp
LFLAGS = -lm -lrt

# Do some substitution to get a list of .o files from the given .c files.
OBJFILES = $(patsubst %.c,%.o,$(SRCFILES))

.PHONY: all run runlocal plot clean dist todo

all: $(PROGNAME)

$(PROGNAME): $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

run_1: $(PROGNAME)
	prun -v -np 1 $(PROGNAME) $(RUNARGS_1)
run_2: $(PROGNAME)
	prun -v -np 1 $(PROGNAME) $(RUNARGS_2)
run_4: $(PROGNAME)
	prun -v -np 1 $(PROGNAME) $(RUNARGS_4)
run_6: $(PROGNAME)
	prun -v -np 1 $(PROGNAME) $(RUNARGS_6)
run_8: $(PROGNAME)
	prun -v -np 1 $(PROGNAME) $(RUNARGS_8)


runlocal: $(PROGNAME)
	./$(PROGNAME) $(RUNARGS)

plot: result.txt
	gnuplot plot.gnp
	$(IMAGEVIEW) plot.png

dist:
	tar cvzf $(TARNAME) Makefile *.c *.h data/

clean:
	rm -fv $(PROGNAME) $(OBJFILES) $(TARNAME) result.txt plot.png
