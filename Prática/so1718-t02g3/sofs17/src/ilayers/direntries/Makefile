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
CXXFLAGS += -I "$(SRC_DIR)/ilayers/inodeattr"
CXXFLAGS += -I "$(SRC_DIR)/ilayers/fileclusters"
CXXFLAGS += -m$(ARCH)
CXXFLAGS += -fPIC

SUFFIX = $(ARCH)

VERSION = sofs17

LIB_NAME = $(VERSION)Direntries

TARGET_LIB = lib$(LIB_NAME).a

OBJS =
OBJS += get_direntry.o
OBJS += add_direntry.o
OBJS += rename_direntry.o
OBJS += delete_direntry.o
OBJS += traverse_path.o
OBJS += check_emptiness.o

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

