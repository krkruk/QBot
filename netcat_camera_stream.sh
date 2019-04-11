#!/bin/bash

port=$1
raspivid -n -t 0 -w 800 -h 600 -fps 30 -o - | netcat -k -l $port
