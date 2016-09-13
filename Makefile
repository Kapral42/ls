prog := ls
prog_objs := main.o directory.o

CC := gcc
CFLAGS := -Wall
LDFLAGS :=

.PHONY: all clean

all: $(prog)

$(prog): $(prog_objs)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c
directory.o: directory.c

clean:
	@rm -rf *.o $(prog)
