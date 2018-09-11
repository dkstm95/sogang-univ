CC=gcc
CFLAGS=-Wall
OBJS=20141543.o
TARGET=20141543.out

$(TARGET) : $(OBJS)
	$(CC) -o $@ $(OBJS)
	
20141543.o: 20141543.h 20141543.c

clean:
	rm -f *.o
	rm -f $(TARGET)
	rm -f *.tmp
	rm -f *.lst
