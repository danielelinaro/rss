LIBS   = -lcurl -pthread -lxml2 -lncurses
CFLAGS = -Wall -g -O0
SRC    = $(wildcard *.c)
PROG   = rss
CC     = gcc
RM     = rm -f

all: $(PROG)

$(PROG): $(SRC)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	$(RM) $(PROG)
