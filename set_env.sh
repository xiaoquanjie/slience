#!/bin/sh

root_path=`pwd`
export LD_LIBRARY_PATH=$root_path/thirdparty_library/mysql/linux_lib64/:$root_path/thirdparty_library/protobuf/linux
