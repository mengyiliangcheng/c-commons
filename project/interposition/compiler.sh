#!/bin/sh
#/**************************************************
#*file name         :compiler.sh
#*descrption        :
#*author            :pengyicheng
#*date              :2019/06/06
#*version           :1.0
# this file is created by tool:newfile.sh VER:1.0 automatic
#***************************************************/
echo filename: compiler.sh


make clean
make
echo "output***************"
LD_PRELOAD="./mymalloc.so"  ./interposition
