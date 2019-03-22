CC = gcc
CFLAGS = -D_GNU_SOURCE -std=c99 -O2 -g2 -Wall
LDFLAGS = -lpthread
SOURCES = main.c shop.c
OBJECT_FILES = $(addprefix obj/, $(SOURCES:.c=.o))
EXECUTABLE = shop

all: obj $(SOURCES) $(EXECUTABLE)

obj:
	mkdir obj

$(EXECUTABLE): $(OBJECT_FILES)
	$(CC) $(OBJECT_FILES) $(LDFLAGS) $(CFLAGS) -o $@

obj/%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm -rf obj $(EXECUTABLE)
