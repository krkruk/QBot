#!/bin/sh

protoc --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` *.proto
protoc --cpp_out=. *.proto
python -m grpc_tools.protoc -I./ --python_out=. --grpc_python_out=. *.proto
