SRC_DIR = ../../
LIB_DIR = ../../../lib

ifndef ARCH
ARCH = $(shell getconf LONG_BIT)
$(warning $(shell printf "warning: ARCH not defined, using ARCH=$(ARCH)"))
endif

CXX = g++
CXXFLAGS = -Wall 
CXXFLAGS += -I $(SRC_DIR)/datatypes
CXXFLAGS += -I $(SRC_DIR)/rawdisk
CXXFLAGS += -I $(SRC_DIR)/probing
CXXFLAGS += -I $(SRC_DIR)/exception
CXXFLAGS += -I $(SRC_DIR)/dealers/sbdealer
CXXFLAGS += -m$(ARCH)
CXXFLAGS += -fPIC

SUFFIX = $(ARCH)

VERSION = sofs17

LIB_NAME = $(VERSION)ITDealer

TARGET_LIB = lib$(LIB_NAME).a

OBJS = itdealer.o 

all:			$(TARGET_LIB)

$(TARGET_LIB):		$(OBJS)
	ar -r $(TARGET_LIB) $^
	cp $(TARGET_LIB) $(LIB_DIR)
	rm -f $^ $(TARGET_LIB)

clean:
	rm -f $(OBJS) $(TARGET_LIB)
	rm -f *~ 

cleanall:	clean
	rm -f $(LIB_DIR)/$(TARGET_LIB)
