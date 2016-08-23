CC = gcc
CCFLAGS = -c -O2 -DLINUX -march=native -mtune=generic \
	  -Wattributes -Wall -Wpedantic -Wextra -I../include -ggdb3 # -std=c99
TARGET = runtime.o

RUNTIME_RE_READ = ifiled_read_again

OBJS += re_read_config.o


$(TARGET): $(OBJS)
	objcopy re_read_config.o $@

re_read_config.o: re_read_config.c ../include/parse.h ../include/handle_events.h ../include/re_read_config.h
	$(CC) $(CCFLAGS) $<

$(RUNTIME_RE_READ): sendsigusr.c
	$(CC) $(CCFLAGS) $< && $(CC) sendsigusr.o -o $@

clean:
	rm -f *.o $(RUNTIME_RE_READ)
