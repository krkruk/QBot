#!/usr/bin/env sh

ip=${1:-localhost}
port=${2:-6000}

nc $ip $port | mplayer -fps 300 -demuxer h264es -
