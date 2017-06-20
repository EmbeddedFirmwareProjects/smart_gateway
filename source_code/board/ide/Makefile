CROSS_COMPILE=

CC=$(CROSS_COMPILE)gcc
CFLAGS=-c -g -rdynamic -funwind-tables -Wall \
	-I../inc/xbee_stack \
	-I../inc/application \
	-I../inc/platform \
	-I../inc/utils \
	-I../inc/smart_power_stack

LDFLAGS=-lpthread -lrt

STRIP=$(CROSS_COMPILE)strip

APP_SRC_PATH=../src/application
PLATFORM_SRC_PATH=../src/platform
XBEE_STACK_SRC_PATH=../src/xbee_stack
SMART_POWER_STACK_SRC_PATH=../src/smart_power_stack
UTILS_SRC_PATH=../src/utils
TEST_SRC_PATH=../src/test

SOURCES=$(APP_SRC_PATH)/main.c \
	$(XBEE_STACK_SRC_PATH)/xbee_stack.c \
	$(PLATFORM_SRC_PATH)/platform.c \
	$(PLATFORM_SRC_PATH)/platform_usart.c \
	$(PLATFORM_SRC_PATH)/platform_linux_usart.c \
	$(PLATFORM_SRC_PATH)/rs232.c \
	$(APP_SRC_PATH)/smart_power_app.c \
	$(SMART_POWER_STACK_SRC_PATH)/smart_power_stack.c \
	$(XBEE_STACK_SRC_PATH)/xbee_at_command.c \
	$(XBEE_STACK_SRC_PATH)/xbee_zigbee_operations.c \
	$(UTILS_SRC_PATH)/timer.c \

OBJECTS_PATH=../obj

OBJECTS=$(SOURCES:.c=.o)

TARGET=../obj/board_app

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) 
	mv $(OBJECTS) $(OBJECTS_PATH)		# move to temp path

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(OBJECTS) $(TARGET) $(OBJECTS_PATH)/*
