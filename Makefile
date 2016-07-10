CC = gcc
CCFLAGS = -c -O2 -DLINUX -D_GNU_SOURCE -D_XOPEN_SOURCE -march=native -mtune=generic \
	  -Wattributes -Wall -Wpedantic -Wextra -I include -Iruntime -ggdb3 # -std=c99
TARGET = ifiled

OBJS += main.o \
   	parse.o \
   	handle_events.o \
	daemon.o \
	savepid.o \
	runtime/runtime.o \
	runtime/signotify.o


.PHONY: run clean test


run: $(TARGET)
	./$(TARGET) -c watching.conf -l ifiled.log;

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

main.o: main.c include/parse.h include/handle_events.h include/core.h
	$(CC) $(CCFLAGS) $<

parse.o: parse.c include/parse.h include/handle_events.h
	$(CC) $(CCFLAGS) $<

handle_events.o: handle_events.c include/handle_events.h include/parse.h include/core.h
	$(CC) $(CCFLAGS) $<

daemon.o: daemon.c include/daemon.h
	$(CC) $(CCFLAGS) $<

util.o: util.c include/util.h
	$(CC) $(CCFLAGS) $<

runtime/runtime.o: runtime/add_watch.c include/add_watch.h
	cd runtime; make; cd ../

runtime/signotify.o: runtime/signotify.c include/signotify.h
	$(CC) $(CCFLAGS) $< -o $@

savepid.o: savepid.c
	$(CC) $(CCFLAGS) $<

clean:
	rm -f $(OBJS) $(TARGET) *.log *.LOG
