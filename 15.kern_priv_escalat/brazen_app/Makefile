CC = gcc
CFLAGS = -std=c99 -O2 -Wall
LDFLAGS = 
SOURCES = main.c
OBJECT_FILES = $(addprefix obj/, $(SOURCES:.c=.o))
EXECUTABLE = brazenapp

all: obj $(SOURCES) $(EXECUTABLE)

obj:
	mkdir obj

$(EXECUTABLE): $(OBJECT_FILES)
	$(CC) $(OBJECT_FILES) $(LDFLAGS) $(CFLAGS) -o $@

obj/%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm -rf obj $(EXECUTABLE)
