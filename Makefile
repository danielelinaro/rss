LIBS   = -lcurl -pthread -lxml2 -lncurses
CFLAGS = -Wall -g -O0 -I/usr/local/Cellar/libxml2/2.8.0/include/libxml2 -L/usr/local/Cellar/libxml2/2.8.0/lib
SRC    = $(wildcard *.c)
PROG   = rss
CC     = gcc
RM     = rm -f

all: $(PROG)

$(PROG): $(SRC)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	$(RM) $(PROG)
