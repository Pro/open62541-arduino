#!/bin/bash
set -ev

# Get arduino sdk
wget https://downloads.arduino.cc/arduino-1.8.6-linux64.tar.xz
tar xf arduino-1.8.6-linux64.tar.xz
cd arduino-1.8.6/hardware

# Additionally get the ESP toolchain
mkdir espressif && cd espressif

git clone --depth 1 https://github.com/espressif/arduino-esp32 esp32

cd esp32

git submodule update --init --recursive
cd tools && python get.py
cd ..

chmod +x tools/esptool/esptool.py

# Increase the stack size
sed -i 's/xTaskCreatePinnedToCore(loopTask, "loopTask", 8192, NULL, 1, NULL, ARDUINO_RUNNING_CORE);/xTaskCreatePinnedToCore(loopTask, "loopTask", 16384, NULL, 1, NULL, ARDUINO_RUNNING_CORE);/g' cores/esp32/main.cpp

