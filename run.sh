#!/bin/sh

root_path=`pwd`
export LD_LIBRARY_PATH=/usr/local/lib
#echo library path: ${root_path}/protobuflib/linux

./blade-bin/routersvr/routersvr -D --config_file ./conf/routersvr/routersvr.conf 
echo 'starting routersvr..........'



