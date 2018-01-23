cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
set(PROJECT_NAME arduino_opcua)
project(${PROJECT_NAME})

set(${CMAKE_PROJECT_NAME}_SKETCH arduino_opcua.ino)

SET(CMAKE_C_COMPILER "${ARDUINO_SDK_PATH}/hardware/espressif/esp32/tools/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc")
SET(CMAKE_CXX_COMPILER "${ARDUINO_SDK_PATH}/hardware/espressif/esp32/tools/xtensa-esp32-elf/bin/xtensa-esp32-elf-g++")

register_hardware_platform(${ARDUINO_SDK_PATH}/hardware/espressif/esp32)

#### Uncomment below additional settings as needed.
set(${CMAKE_PROJECT_NAME}_BOARD featheresp32)
# set(${CMAKE_PROJECT_NAME}_PORT /dev/ttyACM0)
# set(mega.build.mcu atmega2560)
# set(mega.upload.protocol wiring)
# set(mega.upload.speed 115200)

generate_arduino_firmware(${CMAKE_PROJECT_NAME})
