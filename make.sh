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
export COMPILE_TYPE="exe"    #exe/shared/static
GDB=gdb
pwd=$PWD

ARG1=$1

cd ./network/test
make clean
make
RET=$?
check_sub()
{
    if [ $1 -eq 0 ]
    then
        echo -e "\nMake success !\n"
    else
        echo -e "\nMake error!\n"
        exit 1
    fi
    return 0
}
check_sub $RET

cd $pwd

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
if [ $COMPILE_TYPE = "shared" ] 
then
   mv $TARGET "$TARGET.so"
   exit 0
fi
mv $TARGET "$TARGET.elf"

EXEC=$pwd/"$TARGET.elf"
$EXEC $ARG1;
RET=$?

check_execut()
{
    if [ $1 -ne 0 ]
    then
        echo -e "\nExecute Program Failed !\n"
        $GDB $EXEC $ARG1
    else
        echo -e "\nExecute Program Success !\n"
    fi
    return 0
}
check_execut $RET
echo -e "\n--------------------------------------------\n"



