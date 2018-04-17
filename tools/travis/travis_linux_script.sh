#!/bin/bash
set -ev

mkdir build && cd build
cmake -DARDUINO_SDK_PATH=../arduino-1.8.5 -DWIFI_SSID="SOME_SSID" -DWIFI_PWD="SOME_PWD" ..
make -j
if [ $? -ne 0 ] ; then exit 1 ; fi