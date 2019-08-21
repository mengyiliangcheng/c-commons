#!/bin/sh
#/**************************************************
#*file name         :compiler.sh
#*descrption        :
#*author            :pengyicheng
#*date              :2019/08/19
#*version           :1.0
# this file is created by tool:newfile.sh VER:1.0 automatic
#***************************************************/
ARGS=$*
echo filename: compiler.sh

make clean
make
echo "output***************"
./netdetect $ARGS 
