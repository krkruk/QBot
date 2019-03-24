#!/bin/sh

if [[ $1 == 'clean' ]]; then
    to_delete='*.grpc.pb.cc *.grpc.pb.h *_pb2_grpc.py *_pb2.py *.pb.cc *.pb.h'
    for ending in $to_delete; do
        rm $ending
    done
else
    protoc --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` *.proto
    protoc --cpp_out=. *.proto
    python -m grpc_tools.protoc -I./ --python_out=. --grpc_python_out=. *.proto
fi
