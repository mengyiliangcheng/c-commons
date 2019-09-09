#/**************************************************
#*file name         :Makefile.mk
#*descrption        :
#*author            :pengyicheng
#*date              :2019/08/29
#*version           :1.0
# this file is created by tool:newfile.sh VER:1.0 automatic
#***************************************************/

CC=gcc
CXX=g++
LINK=g++
TARGET=udptrans
##librarys
DYNAMIC_LIBS=
STATIC_LIBS=

INCLUDES= -I. 	  -I../


C_SOURCES= main.c \
		   udp_trans.c \
		   udp_connection.c \
	       list.c

CXX_SOURCES=

CFLAGS=-Wall -fPIC -g $(INCLUDES)
CXXFLAGS=$(CFLAGS) -std=c++11

OBJS = $(C_SOURCES:.c=.o) $(CXX_SOURCES:.cpp=.o) 

all:$(TARGET)

$(TARGET):$(OBJS)
	$(LINK) $^ $(DYNAMIC_LIBS) $(CFLAGS) -o $@ $(STATIC_LIBS)

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@ 

%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@ 

install:
	tsxs -i -o $(TARGET)

clean:
	rm -rf $(TARGET)
	rm -rf $(OBJS)

