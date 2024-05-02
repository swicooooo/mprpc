#!/bin/bash

set -e  # 打印信息

if [ ! -d `pwd`/build ];then
	mkdir `pwd`/build
fi

rm -rf `pwd`/build/*

cd `pwd`/build && 
	cmake .. &&
	make -j8

#回到根目录
cd ..

#把头文件拷贝到/usr/local/include/mprpc so库到/usr/local/lib
#环境变量不用设置
if [ ! -d /usr/local/include/mprpc ];then
	mkdir /usr/local/include/mprpc
fi

for header in `ls ./include/*.h`
do
	cp $header /usr/local/include/mprpc
done
cp ./include/header.proto /usr/local/include/mprpc

cp `pwd`/lib/libmprpc.so /usr/local/lib

ldconfig