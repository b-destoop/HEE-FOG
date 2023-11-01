# HEE-FOG
An ESP32 microcontroller project for FOG episodes with Parkinson's patients.

## structure of the code

## commands needed
The esp-idf extension offers commands that are integrated in the command palette of VSCode. You can run VSCode commands by going to 
> view > command palette 

or by pressing 

> ctrl + shift + P

The steps in putting code on the ESP32 microcontroller are:
1. Writing your code (duh)

The code driving the project can be found in the ./main directory.

2. Configuring the microcontroller (e.g. UART, SPI, ...)

open the configuration menu with command
> ESP-IDF: SDK Configuration editor (menuconfig)

3. Building the project.

build the project with command
> ESP-IDF: Build your project

4. Selecting the right USB port.
5. Flashing the project

## Common problems

### LINUX - TTY permission error
```shell
A fatal error occurred: Could not open /dev/ttyUSB0, the port is busy or doesn't exist.
([Errno 13] could not open port /dev/ttyUSB0: [Errno 13] Permission denied: '/dev/ttyUSB0')
```

https://github.com/esp8266/source-code-examples/issues/26
add the current user to the tty user group:
```shell
# all current groups:
groups
# available groups:
compgen -g
# add yourself to the tty and dialout group for tty (usb) access
sudo usermod -a -G tty [username] 
sudo usermod -a -G dialout [username]
```