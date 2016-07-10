CC = gcc
CCFLAGS = -c -O2 -DLINUX -march=native -mtune=generic \
	  -Wattributes -Wall -Wpedantic -Wextra -I../include -ggdb3 # -std=c99
TARGET = runtime.o

OBJS += add_watch.o


$(TARGET): $(OBJS)
	objcopy add_watch.o $@

add_watch.o: add_watch.c ../include/parse.h ../include/handle_events.h ../include/add_watch.h
	$(CC) $(CCFLAGS) $<


clean:
	rm -f $(OBJS) $(TARGET)
