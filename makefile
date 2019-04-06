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
#TARGET=cross

##librarys
DYNAMIC_LIBS= -L./xml/lib -L./libs -L./openssl/lib -lsdkxml -lsdkz -pthread
STATIC_LIBS= ./openssl/lib/libcrypto.a ./openssl/lib/libssl.a ./libs/libcurl.a

#include files
INCLUDES=   -I. \
			-I./mempool \
			-I./string \
			-I./example \
			-I./osadapter \
			-I./string \
			-I./log \
			-I./file \
			-I./json \
			-I./test \
			-I./time \
			-I./algorithm \
			-I./xml/inc \
			-I./xml/include \
			-I./thread \
			-I./includes \
			-I./zlib/contrib/minizip \
			-I./openssl/include \
			-I./network \
			-I./event

#c source code
C_SOURCES= $(wildcard *.c) $(wildcard */*.c) $(wildcard */src/*.c)

#cpp source code
CXX_SOURCES= $(wildcard *.cpp) $(wildcard */*.cpp) $(wildcard */src/*.cpp)

CFLAGS=-Wall -fPIC -g

CXXFLAGS=$(CFLAGS) -std=c++11

#objects
OBJS = $(C_SOURCES:.c=.o) $(CXX_SOURCES:.cpp=.o) 

all:$(TARGET)

$(TARGET):$(OBJS)
	$(LINK) $^ $(DYNAMIC_LIBS) $(CFLAGS) -o $@ $(STATIC_LIBS)

%.o : %.c
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ 

%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $< -o $@ 

install:
	tsxs -i -o $(TARGET)

clean:
	rm -rf $(TARGET)
	rm -rf $(OBJS)


