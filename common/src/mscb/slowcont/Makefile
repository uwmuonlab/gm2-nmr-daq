#####################################################################
#
#  Name:         Makefile
#  Created by:   Stefan Ritt
#
#  Contents:     UNIX Makefile for MIDAS slow control frontend
#
#  $Id$
#
#####################################################################
#
#--------------------------------------------------------------------
# The MIDASSYS should be defined prior the use of this Makefile
ifndef MIDASSYS
missmidas::
	@echo "...";
	@echo "Missing definition of environment variable 'MIDASSYS' !";
	@echo "...";
endif

# get OS type from shell
OSTYPE = $(shell uname)

#--------------------------------------------------------------------
# The following lines contain specific switches for different UNIX
# systems. Find the one which matches your OS and outcomment the 
# lines below.

# This is for Darwin ---------------
ifeq ($(OSTYPE),Darwin)
OS_DIR = darwin
LIBS = -lpthread
OSFLAGS = -DOS_LINUX -DOS_DARWIN -DHAVE_STRLCPY
endif

# This is for Linux ----------------
ifeq ($(OSTYPE),Linux)
OS_DIR = linux
LIBS = -lm -lutil -lpthread -lrt -lz -lnsl
OSFLAGS = -DOS_LINUX
CFLAGS = -g -O2 -Wall
endif

#-----------------------------------------
# ROOT flags and libs
#
ifdef ROOTSYS
ROOTCFLAGS := $(shell  $(ROOTSYS)/bin/root-config --cflags)
ROOTCFLAGS += -DHAVE_ROOT -DUSE_ROOT
ROOTLIBS   := $(shell  $(ROOTSYS)/bin/root-config --libs) -Wl,-rpath,$(ROOTSYS)/lib
ROOTLIBS   += -lThread
else
missroot:
	@echo "...";
	@echo "Missing definition of environment variable 'ROOTSYS' !";
	@echo "...";
endif
#-------------------------------------------------------------------
# The following lines define direcories. Adjust if necessary
#                 
INC_DIR 	= $(MIDASSYS)/include
LIB_DIR 	= $(MIDASSYS)/$(OS_DIR)/lib
DRV_DIR		= $(MIDASSYS)/drivers
MSCB_DIR        = $(MIDASSYS)/mscb
MXML_DIR        = $(MIDASSYS)/../mxml

#-------------------------------------------------------------------
# List of analyzer modules
#
MODULES   = envi.o TEnvi.o 

#-------------------------------------------------------------------
# Drivers needed by the frontend program
#                 
DRIVERS         = hv.o multi.o null.o nulldev.o mscbdev.o mscb.o

####################################################################
# Lines below here should not be edited
####################################################################

LIB = $(LIB_DIR)/libmidas.a

# compiler
CC = cc
CFLAGS = -g -Wall -I$(INC_DIR) -I$(DRV_DIR) -I$(MSCB_DIR) -I$(MXML_DIR) -fPIC
LDFLAGS =

all: scfe mscb_fe analyzer libDataClasses.so
#all: scfe mscb_fe

DataLibraryDict.o: DataLibraryDict.cpp
	$(CXX) -c -Wall $(CFLAGS) $(ROOTLIBS) $(ROOTCFLAGS) $<
	$(CXX) -MM $(CFLAGS) $(ROOTCFLAGS) $< > $(notdir $(patsubst %.cpp,%.d,$<))

scfe:  $(LIB) $(LIB_DIR)/mfe.o frontend.o $(DRIVERS)
	$(CC) -o scfe frontend.o $(LIB_DIR)/mfe.o $(DRIVERS) $(LIB) $(LDFLAGS) $(LIBS)

mscb_fe:  $(LIB) $(LIB_DIR)/mfe.o mscb_fe.o $(DRIVERS)
	$(CC) -o mscb_fe mscb_fe.o $(LIB_DIR)/mfe.o $(DRIVERS) $(LIB) $(LDFLAGS) $(LIBS)

analyzer: $(LIB) $(LIB_DIR)/rmana.o analyzer.o $(MODULES) libDataClasses.so
	$(CXX) $(CFLAGS) -o $@ $(LIB_DIR)/rmana.o analyzer.o $(MODULES) \
	$(LIB) $(LDFLAGS) $(ROOTLIBS) $(LIBS) $(ROOTCFLAGS) libDataClasses.so

DataLibraryDict.cpp: TEnvi.h DataLibraryLinkDef.h
	$(ROOTSYS)/bin/rootcint -f $@ -c $(UIUC_CFLAGS) -p $^

libDataClasses.so: TEnvi.o DataLibraryDict.o
	$(CXX) $(ICCLIBS) $(ROOT_LIBS) -shared -o libDataClasses.so $(shell root-config --ldflags) -I$(ROOTSYS)/include $^

hv.o: $(DRV_DIR)/class/hv.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

frontend.o: frontend.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

mscb_fe.o: mscb_fe.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

multi.o: $(DRV_DIR)/class/multi.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

nulldev.o: $(DRV_DIR)/device/nulldev.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

mscbdev.o: $(DRV_DIR)/device/mscbdev.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

mscb.o: $(MSCB_DIR)/mscb.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

null.o: $(DRV_DIR)/bus/null.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

%.o: %.c experim.h
	$(CXX) $(USERFLAGS) $(ROOTCFLAGS) $(CFLAGS) $(OSFLAGS) -o $@ -c $<

.c.o:
	$(CC) $(CFLAGS) $(OSFLAGS) -c $<

clean:
	rm -f *.o *Dict.cpp *Dict.h libDataClasses.so *~ \#*

