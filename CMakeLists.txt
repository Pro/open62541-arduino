cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_TOOLCHAIN_FILE cmake/ArduinoToolchain.cmake) # Arduino Toolchain


# Set a variable for commands below
set(PROJECT_NAME arduino_opcua)

# Define your project and language
project(${PROJECT_NAME})

# Set special compilers for esp32
set (CMAKE_CXX_STANDARD 11)

SET(CMAKE_C_COMPILER "${ARDUINO_SDK_PATH}/hardware/espressif/esp32/tools/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc")
SET(CMAKE_C_COMPILER_WORKS 1)
SET(CMAKE_CXX_COMPILER "${ARDUINO_SDK_PATH}/hardware/espressif/esp32/tools/xtensa-esp32-elf/bin/xtensa-esp32-elf-g++")
SET(CMAKE_CXX_COMPILER_WORKS 1)
SET(CMAKE_AR "${ARDUINO_SDK_PATH}/hardware/espressif/esp32/tools/xtensa-esp32-elf/bin/xtensa-esp32-elf-ar" CACHE STRING "" FORCE)
SET(CMAKE_RANLIB "${ARDUINO_SDK_PATH}/hardware/espressif/esp32/tools/xtensa-esp32-elf/bin/xtensa-esp32-elf-ranlib")

set(${PROJECT_NAME}_BOARD featheresp32)

# Define the source code
set(${PROJECT_NAME}_SRCS src/arduino_opcua.cpp src/aes.c src/Nodeset.cpp src/Gripper.cpp)

# Define the port for uploading code to the Arduino
# See following URL on how to assign persistend device names on Linux:
# http://hintshop.ludvig.co.nz/show/persistent-names-usb-serial-devices/
set(${PROJECT_NAME}_PORT /dev/ttyUSB9)

# Select a specific upload speed for the board
set(${${PROJECT_NAME}_BOARD}.upload.speed 921600)

# Select a specific flash frequence for the board
set(${${PROJECT_NAME}_BOARD}.build.flash_freq 80m)


# -----------------------------------
# open62541 specific settings - BEGIN
# -----------------------------------
set(UA_ARCHITECTURE "arduino" CACHE STRING "" FORCE)
set(UA_ENABLE_DISCOVERY_SEMAPHORE OFF CACHE BOOL "" FORCE)
set(UA_FILE_NS0 "${PROJECT_SOURCE_DIR}/nodeset/Opc.Ua.NodeSet2.Custom.xml")
set(UA_ENABLE_SUBSCRIPTIONS ON)
set(UA_ENABLE_DISCOVERY ON)
set(UA_ENABLE_ENCRYPTION OFF)
set(UA_ENABLE_STATUSCODE_DESCRIPTIONS ON)
set(UA_ENABLE_TYPENAMES ON)
set(UA_LOGLEVEL 300)
add_subdirectory(lib/open62541)
# Define the source code
set(${PROJECT_NAME}_SRCS ${${PROJECT_NAME}_SRCS} "${PROJECT_BINARY_DIR}/lib/open62541/open62541.c")
include_directories("${PROJECT_BINARY_DIR}/lib/open62541/")

# -----------------------------------
# open62541 specific settings - END
# -----------------------------------


set(${PROJECT_NAME}_SRCS ${${PROJECT_NAME}_SRCS} "${CMAKE_CURRENT_LIST_DIR}/lib/NTPClient/NTPClient.cpp")
include_directories(${CMAKE_CURRENT_LIST_DIR}/lib/NTPClient/)

set(${PROJECT_NAME}_ARDLIBS WiFi NTPClient)
set(${PROJECT_NAME}_ARDLIBS_PATH ${ESP32_LIBRARIES_PATH})
set(${PROJECT_NAME}_LIBS   esp32   wpa_supplicant lwip c)

# Command to generate code arduino firmware (.hex file)
generate_arduino_firmware(${PROJECT_NAME})
add_dependencies(${PROJECT_NAME} open62541-amalgamation-source open62541-amalgamation-header)
