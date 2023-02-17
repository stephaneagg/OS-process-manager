.phony all:
all: pman inf

pman: main.c linked_list.c
	gcc -Wall main.c linked_list.c -o pman

inf: inf.c
	gcc -Wall inf.c -o inf

.PHONY clean:
clean:
	-rm -rf *.o *.exe
