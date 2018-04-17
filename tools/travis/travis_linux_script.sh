#!/bin/bash
set -ev

mkdir build && cd build
cmake -DWIFI_SSID="SOME_SSID" -DWIFI_PWD="SOME_PWD" ..
make -j
if [ $? -ne 0 ] ; then exit 1 ; fi