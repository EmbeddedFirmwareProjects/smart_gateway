CROSS_COMPILE=

CC=$(CROSS_COMPILE)gcc
CFLAGS=-c -g -rdynamic -funwind-tables -Wall -I../inc/xbee_stack -I../inc/application -I../inc/platform
LDFLAGS=   -lpthread -lrt

STRIP=$(CROSS_COMPILE)strip

APP_SRC_PATH=../src/application

SOURCES= $(APP_SRC_PATH)/main.c

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
