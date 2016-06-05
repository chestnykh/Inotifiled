CC = gcc
CCFLAGS = -c -O2 -march=native -DLINUX -mtune=generic


npfd: npfd.o
	$(CC) $< -o $@

npfd.o: main.c
	$(CC) $(CCFLAGS) $< -o $@
