CPPFLAGS = -g -O2 -finput-charset=UTF-8
LDFLAGS  =
LDLIBS   = -lmrubyc
MRBCOMP  = ../../mruby/bin/mrbc

PROG = aut

all: $(PROG)

$(PROG): $(PROG).o Makefile aut_sample01.c
	$(CC) -o $(PROG) $(PROG).o -lncurses

$(PROG).o: $(PROG).c
	$(CC) $(CPPFLAGS) -c -I. -D_GNU_SOURCE $(PROG).c

aut_sample01.c: aut_sample01.rb
	$(MRBCOMP) -Baut ./aut_sample01.rb

clean:
	rm -f ./$(PROG) *.o ./aut_sample01.c