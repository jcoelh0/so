SRC_DIR = ../../
LIB_DIR = ../../../lib
BIN_DIR = ../../../bin

ifndef ARCH
ARCH = $(shell getconf LONG_BIT)
$(warning $(shell printf "warning: ARCH not defined, using ARCH=$(ARCH)"))
endif

CXX = g++
CXXFLAGS = -Wall 
CXXFLAGS += -I "$(SRC_DIR)/probing"
CXXFLAGS += -I "$(SRC_DIR)/exception"
CXXFLAGS += -I "$(SRC_DIR)/rawdisk"
CXXFLAGS += -I "$(SRC_DIR)/datatypes"
CXXFLAGS += -I "$(SRC_DIR)/dealers/czdealer"
CXXFLAGS += -I "$(SRC_DIR)/dealers/itdealer"
CXXFLAGS += -I "$(SRC_DIR)/ilayers/freelists"
CXXFLAGS += -m$(ARCH)
CXXFLAGS += -fPIC

SUFFIX = $(ARCH)

VERSION = sofs17

LIB_NAME = $(VERSION)Fileclusters

TARGET_LIB = lib$(LIB_NAME).a

OBJS =
OBJS += get_filecluster.o
OBJS += alloc_filecluster.o
OBJS += free_fileclusters.o
OBJS += read_filecluster.o
OBJS += write_filecluster.o

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

