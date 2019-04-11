#!/usr/bin/env sh

ip=$1
port=$2
nc $ip $port | mplayer -fps 300 -demuxer h264es -
