CC = gcc
CCFLAGS = -c -O2 -DLINUX -march=native -mtune=generic -DLINUX \
	  -Wattributes -Wall -Wpedantic -Wextra -I include -ggdb3 # -std=c99
TARGET = ifiled

OBJS += main.o \
   	parse.o \
   	handle_events.o \
	daemon.o 


.PHONY: run clean test


run: $(TARGET)
	./$(TARGET) -c watching.conf -l ifiled.log;

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

main.o: main.c watching.conf
	$(CC) $(CCFLAGS) $<

parse.o: parse.c include/parse.h
	$(CC) $(CCFLAGS) $<

handle_events.o: handle_events.c include/handle_events.h
	$(CC) $(CCFLAGS) $<

daemon.o: daemon.c include/daemon.h
	$(CC) $(CCFLAGS) $<

util.o: util.c include/util.h
	$(CC) $(CCFLAGS) $<


clean:
	rm -f $(OBJS) $(TARGET) *.log *.LOG
