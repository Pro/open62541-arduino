OPC UA for Arduino using open62541
==================================

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