INCLDIR = ./include
CC = gcc
CFLAGS = -O2 -include stdio.h -lpthread
CFLAGS += -I$(INCLDIR)
OBJDIR = obj

_DEPS = 
DEPS = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

_OBJS = task4_threads.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

task4_174: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCLDIR)/*~ task4_174 