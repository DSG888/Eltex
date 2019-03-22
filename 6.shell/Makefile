CC = gcc
CFLAGS = -O2 -g2 -Wall
LDFLAGS = 
SOURCES = main.c parser.c shell.c
OBJECT_FILES = $(addprefix obj/, $(SOURCES:.c=.o))
EXECUTABLE = shell

all: obj $(SOURCES) $(EXECUTABLE) dummy

obj:
	mkdir obj

dummy: dummy.c
	gcc dummy.c -o dummy

$(EXECUTABLE): $(OBJECT_FILES) 
	$(CC) $(OBJECT_FILES) $(LDFLAGS) $(CFLAGS) -o $@

obj/%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm -rf obj $(EXECUTABLE) dummy

