#!/bin/sh

root_path=`pwd`
export LD_LIBRARY_PATH=${root_path}/protobuflib/linux
echo library path: ${root_path}/protobuflib/linux

./blade-bin/routersvr/routersvr -D --config_file ./conf/routersvr/routersvr.conf 
echo 'starting routersvr..........'



