CC = gcc
CCFLAGS = -pipe -c -O2 -DLINUX -D_GNU_SOURCE=600 -D_XOPEN_SOURCE=600 -march=native -mtune=generic \
	  -Wattributes -Wall -Wpedantic -Wextra -Wno-unused-parameter -Iinclude -Iruntime -flto #-ggdb3 # -std=c99

TARGET = ifiled
DAEMON_MANAGEMENT=daemon_manage


OBJS += main.o \
   	parse.o \
   	handle_events.o \
	daemon.o \
	savepid.o \
	runtime_read_config.o \
	signotify.o


.PHONY: run clean test install


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
runtime_read_config.o: runtime_read_config.c include/runtime_read_config.h
signotify.o: signotify.c include/signotify.h

$(DAEMON_MANAGEMENT): sendsig.c
	$(CC) $(CCFLAGS) $< && \
	$(CC) sendsig.o -o $@


clean:
	rm -f *.o $(TARGET) *.log *.LOG $(DAEMON_MANAGEMENT)

