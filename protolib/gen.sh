#!/bin/sh

./protoc  --proto_path=../protolib/proto --cpp_out=../protolib/src/ routersvr_config.proto
