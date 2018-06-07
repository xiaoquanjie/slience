#!/bin/sh


cd ./protobuf/linux/
ln -sf libprotobuf.so.15.0.1 libprotobuf.so
ln -sf libprotobuf.so.15.0.1 libprotobuf.so.15
cd -

cd ./mysql/linux_lib64
ln -sf libmysqlcppconn.so.7.1.1.9 libmysqlcppconn.so
ln -sf libmysqlcppconn.so.7.1.1.9 libmysqlcppconn.so.7
cd -

cd ./mysql/linux_lib32
ln -sf libmysqlcppconn.so.7.1.1.9 libmysqlcppconn.so
ln -sf libmysqlcppconn.so.7.1.1.9 libmysqlcppconn.so.7
cd -

