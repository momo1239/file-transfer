CC = gcc
CFLAGS = -O -Wall -Wshadow -Wextra -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition -D_POSIX_SOURCE -std=c99
LDFLAGS = -lm

SRCDIR = src
PROTODIR = protocol
SERVERDIR = server

SOURCES = $(wildcard $(SRCDIR)/*.c)
PROTOCOL_SOURCES = $(wildcard $(PROTODIR)/*.c)
SERVER_SOURCES = $(wildcard $(SERVERDIR)/*.c)

OBJECTS = $(SOURCES:.c=.o) $(PROTOCOL_SOURCES:.c=.o) $(SERVER_SOURCES:.c=.o)

EXECUTABLE = fileserver

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

