CC = gcc
CFLAGS = -std=gnu99 -O2 -g2 -Wall
LDFLAGS = 
SOURCES = main.c ping.c
OBJECT_FILES = $(addprefix obj/, $(SOURCES:.c=.o))
EXECUTABLE = ping

all: obj $(SOURCES) $(EXECUTABLE)

obj:
	mkdir -p obj

$(EXECUTABLE): $(OBJECT_FILES)
	$(CC) $(OBJECT_FILES) $(LDFLAGS) $(CFLAGS) -o $@

obj/%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm -rf obj $(EXECUTABLE)
