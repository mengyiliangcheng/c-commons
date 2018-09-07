#!/bin/sh
#/**************************************************
#*file name         :makefile
#*descrption        : 
#*author            :pengyicheng
#*date              :20161125
#*version           :1.0
#***************************************************/ 

export TARGET='c-commons'
echo $TARGET
export INC="" 
export SYS_INC=""
export CC=gcc
pwd=$PWD

ARG1=$1

clear
echo "Clean ..."
make clean
echo -e "\nMaking ..."
make
RET=$?

check()
{
    if [ $1 -eq 0 ]
    then
        echo -e "\nMake success !\nExecute the program !\n"
        echo -e "\n--------------------------------------------\n"
    else
        echo -e "\nMake error!\n"
        exit 1
    fi
    return 0
}
check $RET
mv $TARGET "$TARGET.elf"

EXEC=$pwd/"$TARGET.elf"
$EXEC $ARG1;
echo -e "\n--------------------------------------------\n"



