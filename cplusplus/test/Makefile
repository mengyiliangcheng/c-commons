#!/usr/sh
#/**************************************************
#*file name         :makefile
#*descrption        : 
#*author            :pengyicheng
#*date              :20190406
#*version           :1.0
#***************************************************/
##compiler 
CC=gcc
CXX=g++
LINK=g++
TARGET=cross

##librarys
DYNAMIC_LIBS=
STATIC_LIBS=

INCLUDES= -I. \
	  -I../

C_SOURCES= main.c

CXX_SOURCES=myprint.cpp hello.cpp

CFLAGS=-Wall -fPIC -g

CXXFLAGS=$(CFLAGS) -std=c++11

OBJS = $(C_SOURCES:.c=.o) $(CXX_SOURCES:.cpp=.o) 

all:$(TARGET)

$(TARGET):$(OBJS)
	$(LINK) $^ $(DYNAMIC_LIBS) $(CFLAGS) -o $@

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@ 

%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@ 

install:
	tsxs -i -o $(TARGET)

clean:
	rm -rf $(TARGET)
	rm -rf $(OBJS)


