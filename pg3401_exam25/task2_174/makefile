INCLDIR = ./include
CC = gcc
CFLAGS = -O2 -include stdio.h
CFLAGS += -I$(INCLDIR)
OBJDIR = obj

_DEPS = source.h
DEPS = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

_OBJS = source.o task2_sum.o task2_hash.o task2_count.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

task2_174: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCLDIR)/*~