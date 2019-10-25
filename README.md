OPC UA for Arduino using open62541
==================================


**NOTE**:
**This project is not maintained anymore due to two reasons:**
1. The arduino-cmake modifications I did are not integrated in the main repo. Keeping that fork up-to-date is too much effort. See also https://github.com/arduino-cmake/arduino-cmake/pull/74

2. In general it's better and easier to use the esp32 framework directly, instead of using the arduino build tools. An example for that can be found here: https://github.com/Pro/open62541-esp32

--------------

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

## Setting a fixed ttyUSB Port

If you plug in your microcontroller via USB, Linux will automatically assign a `/dev/ttyUSB` port, starting with index 0. Depending how many USB devices you have connected, it may happen that every time you plug the ESP32, it gets another `ttyUSB` name.
To statically asign a custom port, follow these steps (for Ubuntu):

1. Plug in the ESP32 and call `udevadm info -a -n /dev/ttyUSB0 | grep -E '({serial}|{idProduct}|{idVendor})' | head -n3` (make sure that `ttyUSB0` is the ESP32)
2. Create a new udev rule with `sudo nano /etc/udev/rules.d/99-usb-serial.rules` and the following content, where you need to replace the values whith the one from previous command.
    ```
    SUBSYSTEM=="tty", ATTRS{idVendor}=="10c4", ATTRS{idProduct}=="ea60", ATTRS{serial}=="01435008", SYMLINK+="ttyUSB9"
    ```
3. Now unplug and plug in ESP32 again, and it will be on `/dev/ttyUSB9`

See also:
http://hintshop.ludvig.co.nz/show/persistent-names-usb-serial-devices/


## How to flash

If the previous step with `make`runs through successfully, you can use the CMAke target to upload/flash the program to your microcontroller:

```
make arduino_opcua-upload

```
If everything is set up correctly, the flashing should run through, otherwise please check the CMake files, you may need to adjust the COM port.

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

Additional info can be found in the Wiki: [Debugging ESP32](https://github.com/Pro/open62541-arduino/wiki/Debugging-ESP32)
