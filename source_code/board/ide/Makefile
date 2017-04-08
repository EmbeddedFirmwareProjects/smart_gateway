CROSS_COMPILE=

CC=$(CROSS_COMPILE)gcc
CFLAGS=-c -g -rdynamic -funwind-tables -Wall -I../inc/xbee_stack -I../inc/application -I../inc/platform
LDFLAGS=   -lpthread -lrt

STRIP=$(CROSS_COMPILE)strip

APP_SRC_PATH=../src/application
XBEE_STACK_SRC_PATH=../src/xbee_stack

SOURCES= $(APP_SRC_PATH)/main.c $(APP_SRC_PATH)/c_buffer.c $(XBEE_STACK_SRC_PATH)/xbee_stack.c

OBJECTS_PATH=../obj

OBJECTS=$(SOURCES:.c=.o)

TARGET=../board_app

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) 
	mv $(OBJECTS) $(OBJECTS_PATH)		# move to temp path

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(OBJECTS_PATH)/* $(TARGET)
