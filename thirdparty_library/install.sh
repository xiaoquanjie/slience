#!/bin/sh

exec ./link.sh
cp -rf ./protobuf/linux/* /usr/local/lib
cp -rf ./mysql/linux_lib64/* /usr/local/lib
