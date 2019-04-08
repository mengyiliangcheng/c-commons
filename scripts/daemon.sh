#!/bin/sh
#/**************************************************
#*file name         :daemon.sh
#*descrption        :
#*author            :pengyicheng
#*date              :20190406
#*version           :1.0
# this file is created by tool:newfile.sh VER:1.0 automatic
#***************************************************/
echo filename: daemon.sh

while((1==1))
do
PID=`ps -elf |grep ./hello | grep -v grep | awk '{print $4}'`
echo $PID

if [ "$PID" == "" ]
then
    nohup ./hello > hello.log 2>&1 &
    echo start ok !
else
    echo runing
fi
sleep 5
done
