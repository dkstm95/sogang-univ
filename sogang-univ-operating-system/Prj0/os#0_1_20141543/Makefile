OBJECTS = main.o list.o hash.o bitmap.o

CC=gcc
CFLAGS=-g -c -Wall

TARGET=testlib

$(TARGET) : $(OBJECTS)
	$(CC) -o $(TARGET) $(CFAGS) $(OBJECTS)

clean:
	rm -rf $(OBJECTS) $(TARGET)

main.o : main.h main.c
list.o : list.h list.c
hash.o : hash.h hash.c
bitmap.o : bitmap.h bitmap.c
