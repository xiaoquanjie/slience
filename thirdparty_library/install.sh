#!/bin/sh
cur_path=`pwd`
# exec ./link.sh
source ./link.sh
# cd ${cur_path}
cp -rf ./protobuf/linux/* /usr/local/lib
cp -rf ./mysql/linux_lib64/* /usr/local/lib
