# Makefile
CC=gcc
TARGET1=gateway
TARGET2=server
TARGET3=client
CFLAGS= -g -Wall

OBJFILES1=gateway.o list.o
OBJFILES2=server.o img_list.o
OBJFILES3=client.o API.o

THREAD=-lpthread

default: $(TARGET1) $(TARGET2) $(TARGET3)

$(TARGET1): $(OBJFILES1)
	$(CC) $(CFLAGS) -o $(TARGET1) $(OBJFILES1) $(THREAD)

$(TARGET2): $(OBJFILES2)
	$(CC) $(CFLAGS) -o $(TARGET2) $(OBJFILES2) $(THREAD)

$(TARGET3): $(OBJFILES3)
	$(CC) $(CFLAGS) -o $(TARGET3) $(OBJFILES3)
clean:
	rm -f $(TARGET1) $(TARGET2) $(TARGET3) $(OBJFILES1) $(OBJFILES2) $(OBJFILES3)

zip:
	zip proj.zip gateway.c client.c server.c message.h list.c list.h API.c API.h img_list.c img_list.h Makefile
