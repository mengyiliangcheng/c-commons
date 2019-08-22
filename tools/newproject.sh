#!/bin/bash
#/**************************************************
#*file name         :newfile.sh
#*descrption        :create a new file for project
#*author            :pengyicheng
#*date              :20190406
#*version           :1.0
# this licence is create by tool:newfile.sh VER:1.0 automatic
#***************************************************/
echo filename: newproject.sh

SCRIPT_DIR=$(cd $(dirname ${BASH_SOURCE[0]}); pwd)

MAKESCIPT_NAME=compiler.sh
MAKEFILE=Makefile.mk
create_make_script()
{
	$SCRIPT_DIR/newfile.sh -f $MAKESCIPT_NAME
	echo "make clean" >>$MAKESCIPT_NAME
	echo "make" >>$MAKESCIPT_NAME
	echo 'echo "output***************"' >>$MAKESCIPT_NAME
	echo "./"$PROJECT_NAME >>$MAKESCIPT_NAME
}

create_makefile()
{
	C_SOURCES=`ls *.c`
	CPP_SOURCES=`ls *.cpp`
	MAKEFILE_NAME=$1
	$SCRIPT_DIR/newfile.sh -f $MAKEFILE_NAME
	echo "CC=gcc
CXX=g++
LINK=g++" >>$MAKEFILE_NAME
echo "TARGET="$PROJECT_NAME >>$MAKEFILE_NAME
echo "##librarys
DYNAMIC_LIBS=
STATIC_LIBS=

INCLUDES= -I. \
	  -I../
">>$MAKEFILE_NAME
echo "">>$MAKEFILE_NAME
echo "C_SOURCES=" $C_SOURCES >>$MAKEFILE_NAME
echo "">>$MAKEFILE_NAME
echo "CXX_SOURCES=" $CPP_SOURCES >>$MAKEFILE_NAME
echo "" >>$MAKEFILE_NAME
echo 'CFLAGS=-Wall -fPIC -g $(INCLUDES)' >>$MAKEFILE_NAME
echo 'CXXFLAGS=$(CFLAGS) -std=c++11

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
' >>$MAKEFILE_NAME
	mv $MAKEFILE_NAME makefile
}

create_c_project()
{
	PROJECT_NAME=$1
	if [ -n "$PROJECT_NAME" ]
	then
		echo "will create project:" $PROJECT_NAME "in " $PWD 
	else
		echo "error project name"
		exit
	fi
	mkdir $PROJECT_NAME
	cd $PROJECT_NAME
	$SCRIPT_DIR/newfile.sh -m main.c
	create_makefile $MAKEFILE
	create_make_script
}

create_cpp_project()
{
	PROJECT_NAME=$1
	if [ -n "$PROJECT_NAME" ]
	then
		echo "will create project:" $PROJECT_NAME "in " $PWD 
	else
		echo "error project name"
		exit
	fi
	mkdir $PROJECT_NAME
	cd $PROJECT_NAME
	$SCRIPT_DIR/newfile.sh -m main.cpp
	create_makefile $MAKEFILE
	create_make_script
}

PROJECT_TYPE=$1
PROJECT_NAME=$2

case $PROJECT_TYPE in
	-c) echo "c project"
		create_c_project $PROJECT_NAME
		;;
	-cpp) echo "cpp project"
		create_cpp_project $PROJECT_NAME
		;;
	*) echo "error project type"
       echo "option:[-c/-cpp]"
		exit
		;;
esac
