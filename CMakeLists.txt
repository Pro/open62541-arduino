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


option(USE_WIFI "Defines if WIFI should be used, or alternatively the Ehternet feather" ON)
if(USE_WIFI)
    add_definitions(-DUSE_WIFI)
endif()
option(WIFI_SSID "Wifi SSID where the controller should log in")
option(WIFI_PWD "Wifi password where the controller should log in")
if(NOT WIFI_SSID OR NOT WIFI_PWD)
    MESSAGE(FATAL_ERROR "You need to define WIFI_SSID and WIFI_PWD using cmake options")
endif()

# -----------------------------------
# open62541 specific settings - BEGIN
# -----------------------------------
include_directories("${ARDUINO_SDK_PATH}/hardware/espressif/esp32/tools/sdk/include/freertos/freertos")
set(UA_ARCHITECTURE "freertosLWIP" CACHE STRING "" FORCE)
set(UA_ENABLE_DISCOVERY_SEMAPHORE OFF CACHE BOOL "" FORCE)
set(UA_NAMESPACE_ZERO MINIMAL)
set(UA_ENABLE_SUBSCRIPTIONS ON)
set(UA_ENABLE_DISCOVERY ON)
set(UA_ENABLE_ENCRYPTION OFF)
set(UA_ENABLE_STATUSCODE_DESCRIPTIONS ON)
set(UA_ENABLE_TYPENAMES ON)
set(UA_ENABLE_AMALGAMATION ON CACHE BOOL "" FORCE)
set(UA_LOGLEVEL 300)
set(UA_NODESET_ENCODE_BINARY_SIZE 1024)
add_subdirectory(lib/open62541)

add_definitions(-DUA_ARCHITECTURE_FREERTOSLWIP)
set_source_files_properties("${PROJECT_BINARY_DIR}/lib/open62541/open62541.c" PROPERTIES GENERATED TRUE)
set(${PROJECT_NAME}_SRCS ${${PROJECT_NAME}_SRCS} "${PROJECT_BINARY_DIR}/lib/open62541/open62541.c")
include_directories("${PROJECT_BINARY_DIR}/lib/open62541/")
# -----------------------------------
# open62541 specific settings - END
# -----------------------------------


set(${PROJECT_NAME}_SRCS ${${PROJECT_NAME}_SRCS} "${CMAKE_CURRENT_LIST_DIR}/lib/NTPClient/NTPClient.cpp")
include_directories(${CMAKE_CURRENT_LIST_DIR}/lib/NTPClient/)

set(${PROJECT_NAME}_ARDLIBS WiFi NTPClient)
set(${PROJECT_NAME}_ARDLIBS_PATH ${ESP32_LIBRARIES_PATH})
set(${PROJECT_NAME}_LIBS  gcc pp wpa phy wpa_supplicant core tcpip_adapter c_nano rtc wpa2 esp32 coexist app_update nghttp spiffs espnow nvs_flash esp_adc_cal log expat m c stdc++ heap mbedtls lwip net80211 pp wps)

# Command to generate code arduino firmware (.hex file)
generate_arduino_firmware(${PROJECT_NAME})
target_compile_options(${PROJECT_NAME} PRIVATE -fno-exceptions -DWIFI_SSID="${WIFI_SSID}" -DWIFI_PWD="${WIFI_PWD}")
add_dependencies(${PROJECT_NAME} open62541 open62541-amalgamation-source open62541-amalgamation-header)

get_target_property(CUSTOM_FLAGS  ${PROJECT_NAME} "COMPILE_FLAGS")
set_target_properties(open62541-object PROPERTIES COMPILE_FLAGS ${CUSTOM_FLAGS})
