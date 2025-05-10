INCLDIR = ./include
CC = gcc
CFLAGS = -O2 -include stdio.h
LDFLAGS = -lsocket
CFLAGS += -I$(INCLDIR)
OBJDIR = obj

_DEPS = main.h
DEPS = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

_OBJS = main.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

task6_174: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCLDIR)/*~ task6_174