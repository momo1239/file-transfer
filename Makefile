CC = gcc
CFLAGS = -O -Wall -Wshadow -Wextra -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition -D_POSIX_SOURCE -std=c99
LDFLAGS = -lm

SRCDIR = src
BUILDDIR = build

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SOURCES))

EXECUTABLE = $(BUILDDIR)/fileserver

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) | $(BUILDDIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
