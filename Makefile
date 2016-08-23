CC = gcc
CCFLAGS = -pipe -c -O2 -DLINUX -D_GNU_SOURCE -D_XOPEN_SOURCE -march=native -mtune=generic \
	  -Wattributes -Wall -Wpedantic -Wextra -Iinclude -Iruntime -flto #-ggdb3 # -std=c99
TARGET = ifiled

OBJS += main.o \
   	parse.o \
   	handle_events.o \
	daemon.o \
	savepid.o \
	runtime/runtime.o \
	runtime/signotify.o


.PHONY: run clean test install read_config


run: $(TARGET)
	./$(TARGET) -c $(shell pwd)/watching.conf -l $(shell pwd)/ifiled.log;

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@


.c.o: 
	$(CC) $(CCFLAGS) $<

main.o: main.c include/parse.h include/handle_events.h include/core.h
parse.o: parse.c include/parse.h include/handle_events.h
handle_events.o: handle_events.c include/handle_events.h include/parse.h include/core.h
daemon.o: daemon.c include/daemon.h
util.o: util.c include/util.h
savepid.o: savepid.c

runtime/runtime.o: runtime/re_read_config.c include/re_read_config.h
	cd runtime; make; cd ../


runtime/signotify.o: runtime/signotify.c include/signotify.h
	$(CC) $(CCFLAGS) $< -o $@


clean:
	rm -f *.o $(TARGET) *.log *.LOG

read_config: 
	cd runtime && make $(RUNTIME_RE_READ) && cd ../ && \
	objcopy runtime/ifiled_read_again ./$@
