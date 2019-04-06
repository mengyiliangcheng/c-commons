#!/bin/sh
#/**************************************************
#*file name         :newfile.sh
#*descrption        :create a new file for project
#*author            :pengyicheng
#*date              :20190406
#*version           :1.0
#***************************************************/

VERSION=1.0
TOOLNAME=newfile.sh

##
##创建c文件
create_c_file () {
FILE=$1
echo "/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *">$FILE
echo " * this licence is created by tool:"$TOOLNAME" VER:"$VERSION" automatic" >> $FILE
echo "*/" >> $FILE
echo "\n" >> $FILE
echo "\n" >> $FILE
}

##
##创建c主程序文件
create_cmain_file () {
FILE=$1
echo "/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *">$FILE
echo " * this licence is created by tool:"$TOOLNAME" VER:"$VERSION" automatic" >> $FILE
echo "*/" >> $FILE
echo "#include <stdio.h>" >>$FILE
echo "\n" >>$FILE
echo "int main()" >>$FILE
echo "{" >>$FILE
echo '	printf("hello world");' >>$FILE
echo "	return 0;" >>$FILE
echo "}" >>$FILE
echo "\n" >> $FILE
echo "\n" >> $FILE
}

##
##创建c++主程序文件
create_cppmain_file () {
FILE=$1
echo "/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *">$FILE
echo " * this licence is created by tool:"$TOOLNAME" VER:"$VERSION" automatic" >> $FILE
echo "*/" >> $FILE
echo "#include <iostream>" >>$FILE
echo "\n" >>$FILE
echo "using namespace std;" >>$FILE
echo "int main()" >>$FILE
echo "{" >>$FILE
echo '	cout << "hello world" <<endl;' >>$FILE
echo "	return 0;" >>$FILE
echo "}" >>$FILE
echo "\n" >> $FILE
echo "\n" >> $FILE
}

##
##创建c的头文件
create_c_inc_file () {
FILE=$1
NAME=${FILE%.*}
EXTENSION=${FILE##*.}
NAME=$(echo $NAME | tr '[a-z]' '[A-Z]')
MACRO="__"$NAME"_H__"
echo "/*
 * Copyright (c) 2018-2019 Elvis Peng. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *">$FILE
echo " * this licence is created by tool:"$TOOLNAME" VER:"$VERSION" automatic" >> $FILE
echo "*/" >> $FILE
echo "#ifndef "$MACRO >> $FILE
echo "#define "$MACRO >> $FILE
echo "\n" >> $FILE
echo "\n" >> $FILE
echo "#endif" >> $FILE
echo "\n" >> $FILE
echo "\n" >> $FILE
}

##
##创建shell脚本文件
create_shell_file () {
FILE=$1
echo "#!/bin/sh
#/**************************************************" >$FILE
echo "#*file name         :"$FILENAME >> $FILE
echo "#*descrption        :
#*author            :pengyicheng
#*date              :20190406
#*version           :1.0">>$FILE
echo "# this file is created by tool:"$TOOLNAME" VER:"$VERSION" automatic" >> $FILE
echo "#***************************************************/" >> $FILE
echo "echo filename:" $FILENAME >> $FILE
echo "\n" >> $FILE
echo "\n" >> $FILE
}

##
##创建makefile文件
create_makefile_file () {
FILE=$1
echo "#/**************************************************" >$FILE
echo "#*file name         :"$FILENAME >>$FILE
echo "#*descrption        :
#*author            :pengyicheng
#*date              :20190406
#*version           :1.0">>$FILE
echo "# this file is created by tool:"$TOOLNAME" VER:"$VERSION" automatic" >> $FILE
echo "#***************************************************/" >> $FILE
echo "\n" >> $FILE
}

FILETYPE=$1
FILENAME=$2
MAIN_FLAGS=NO
case $FILETYPE in
	-m) MAIN_FLAGS=YES
		;;
	-f) MAIN_FLAGS=NO
		;;
	*) echo "file type is error"
	   echo "[usage] newfile.sh [-m|-f] filename"
	   echo "[usage] -m create a main file including main function"
	   echo "[usage] -f create a simple file excluding main function"
		;;
esac

if [ -n "$FILENAME" ]
then
	echo "you will create file:" $FILENAME "in " $PWD 
else
    echo "error file name"
	exit
fi

FILE_NAME=${FILENAME%.*}
FILE_EXTENSION=${FILENAME##*.}

case $FILE_EXTENSION in
	c) echo "you will create a c file"
		if [ $MAIN_FLAGS = "NO" ]
		then
			create_c_file $FILENAME
		else
			create_cmain_file $FILENAME
		fi
		;;
    cpp) echo "you will create a cpp file"
		if [ $MAIN_FLAGS = "NO" ]
		then
			create_c_file $FILENAME
		else
			create_cppmain_file $FILENAME
		fi
		;;
	h)  echo "you will create a cpp file"
		create_c_inc_file $FILENAME
		;;
	sh) echo "you will create a shell file"
		create_shell_file $FILENAME
		chmod 777 $FILENAME
		;;
	mk) echo "you will create a makefile"
		create_makefile_file $FILENAME
		;;
	*) echo "unknow file type"
	   echo "[usage] newfile xx.xx"
		;;
esac





