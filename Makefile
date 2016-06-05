CC = gcc
CCFLAGS = -c -O2 -DLINUX -march=native -mtune=generic -DLINUX -ggdb3 -Wattributes -Wall -I include
TARGET = ifiled

OBJS += main.o \
   	parse.o \
   	handle_events.o \
	create_daemon.o \
	util.o


.PHONY: run clean

run: $(TARGET)
	cd const_pollfd; make;
	./$(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

main.o: main.c watching.conf
	$(CC) $(CCFLAGS) $<

parse.o: parse.c include/parse.h
	$(CC) $(CCFLAGS) $<

handle_events.o: handle_events.c include/handle_events.h
	$(CC) $(CCFLAGS) $<

create_daemon.o: create_daemon.c include/create_daemon.h
	$(CC) $(CCFLAGS) $<

util.o: util.c include/util.h
	$(CC) $(CCFLAGS) $<


clean:
	rm -f $(OBJS) $(TARGET) *.log *.LOG
