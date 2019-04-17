CC = gcc
CFLAGS = -D_GNU_SOURCE -std=c99 -O2 -g2 -Wall
LDFLAGS = -lpthread
SOURCES = main.c net.c oth.c
OBJECT_FILES = $(addprefix obj/, $(SOURCES:.c=.o))
EXECUTABLE = exec

all: obj $(SOURCES) $(EXECUTABLE)

obj:
	mkdir -p obj

$(EXECUTABLE): $(OBJECT_FILES)
	$(CC) $(OBJECT_FILES) $(LDFLAGS) $(CFLAGS) -o $@

obj/%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm -rf obj $(EXECUTABLE)
