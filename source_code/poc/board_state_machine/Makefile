CROSS_COMPILE=

CC=$(CROSS_COMPILE)gcc
CFLAGS=-c -g -rdynamic -funwind-tables -Wall -I./
LDFLAGS=   -lpthread -lrt

STRIP=$(CROSS_COMPILE)strip

SOURCES= main.c \
	sw_timer.c \
	xbee_operations.c
	
OBJECTS=$(SOURCES:.c=.o)

TARGET=state_machine_app

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) 
# 	$(STRIP) $(TARGET)
.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) *.o $(TARGET)
