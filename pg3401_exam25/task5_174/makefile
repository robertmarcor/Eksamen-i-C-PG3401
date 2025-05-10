INCLDIR = ./include
CC = gcc
CFLAGS = -O2 -include stdio.h
LDFLAGS = -lsocket
CFLAGS += -I$(INCLDIR)
OBJDIR = obj

_DEPS = ewpdef.h communication.h
DEPS = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

_OBJS = main.o communication.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

task5_174: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCLDIR)/*~ task5_174