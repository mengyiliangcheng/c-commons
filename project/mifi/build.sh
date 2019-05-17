#!/bin/sh
#/**************************************************
#*file name         :build.sh
#*descrption        :
#*author            :pengyicheng
#*date              :2019/05/15
#*version           :1.0
# this file is created by tool:newfile.sh VER:1.0 automatic
#***************************************************/
clear

LOG="/home/pengyicheng/self/commons/c-commons/project/mifi/build.log"
echo filename: build.sh >> $LOG

HOME_PATH="/home/pengyicheng/"
BUILD_CMD=$1

echo "build command:"$BUILD_CMD >> $LOG

DEST_DIRS="mifi 778 work"

LOG=$HOME_PATH"build.log"
echo filename: build.sh >> $LOG

do_check_path()
{
if [ -d $BUILD_PATH ]
then
    echo "build path already exist!!"
else
    rm -rf $HOME_PATH$DEST_DIR"/mdm9607-le-2-0-1_amss_standard_oem_105_1"
    mkdir $HOME_PATH$DEST_DIR
    cd $HOME_PATH$DEST_DIR
    echo "git clone to:"$PWD
    git clone ssh://pengyicheng@192.168.32.200:29418/mdm9607-le-2-0-1_amss_standard_oem_105_1 && scp -p -P 29418 pengyicheng@192.168.32.200:hooks/commit-msg mdm9607-le-2-0-1_amss_standard_oem_105_1/.git/hooks/
    cd $HOME_PATH$DEST_DIR"/mdm9607-le-2-0-1_amss_standard_oem_105_1"
    git checkout -b SLT778_776 origin/SLT778_776
    git pull
fi
}

do_git()
{
echo "start update repository" >> $LOG
git reset --hard HEAD^^ >> $LOG
git pull >> $LOG
echo "update over" >> $LOG
}

do_git_checkout()
{
    cd ../
    git checkout . >> $LOG
    git reset --hard >> $LOG
    git pull >> $LOG
    cd -
}

do_source()
{
    cd $BUILD_PATH
    echo "update env parameter" >> $LOG
    source build/conf/set_bb_env.sh >> $LOG
}

do_clean()
{
    echo "do build,cmd:"$1 >> $LOG
    if [ $1 = "-c" ]
    then
        echo "start buildclean" >> $LOG
        buildclean
    fi
}

do_build()
{
    bitbake -c cleanall cpe
    bitbake -c cleanall linux-quic
    bitbake -c cleanall data
    build_9607_image 2 1 1
}

do_all()
{
do_check_path
do_git
do_source
do_clean $BUILD_CMD
do_build

BUILD_RESULT=$?
echo "build result:"$? >> $LOG

if [ $BUILD_RESULT != "0"  ]
then
    do_git_checkout
    do_source
    do_clean $BUILD_CMD
    do_build
fi
}

for DIR in $DEST_DIRS
do
    echo "now we build:"$DIR >> $LOG
    DEST_DIR=$DIR
    BUILD_PATH=$HOME_PATH$DEST_DIR"/mdm9607-le-2-0-1_amss_standard_oem_105_1/apps_proc/poky"
    cd $BUILD_PATH
    echo $BUILD_PATH
    echo "now we are here::"$PWD >> $LOG
    do_all
done





