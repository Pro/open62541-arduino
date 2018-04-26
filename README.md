OPC UA for Arduino using open62541
==================================

Example project for compiling [open62541](https://open62541.org) on an Arduino. This specific project uses an Adafruit ESP32 microcontroller with a WLAN module.

[![Build Status](https://img.shields.io/travis/Pro/open62541-arduino/master.svg)](https://travis-ci.org/Pro/open62541-arduino)

## How to build

Check out the file used by travis, to see which dependencies are required: `tools/travis/travis_linux_before_install.sh`

Then you can simply build using something like this. Don't forget to set the environment variable `ARDUINO_SDK_PATH` to the location of the arduino sdk.

```bash
mkdir build && cd build
cmake -DWIFI_SSID="SOME_SSID" -DWIFI_PWD="SOME_PWD" ..
make -j
```

### Increase the stack size

You need to manually increase the stack size of the loopTask in the file:

`arduino-1.5.8/hardware/espressif/esp32/cores/esp32/main.cpp`

and change the line

`xTaskCreatePinnedToCore(loopTask, "loopTask", 8192, NULL, 1, NULL, ARDUINO_RUNNING_CORE);`

to

`xTaskCreatePinnedToCore(loopTask, "loopTask", 16384, NULL, 1, NULL, ARDUINO_RUNNING_CORE);`

Otherwise you will get an error like:
```
Guru Meditation Error: Core  1 panic'ed (Unhandled debug exception)
Debug exception reason: Stack canary watchpoint triggered (loopTask) 
```

## How to debug

It is possible to use an OCD debugger to directly debug your code on the microcontroller using gdb (or any IDE which is based on gdb, like Eclipse, CLion, ...).

Additional info can be found in the Wiki: [Debugging ESP32](https://github.com/Pro/open62541-arduino/wiki/debugging)