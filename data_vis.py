import io
import os
import time

import serial
import serial.tools.list_ports
import queue

# serial info https://espressif-docs.readthedocs-hosted.com/projects/esp-idf/en/v3.3.5/get-started/establish-serial-connection.html
# https://pyserial.readthedocs.io/en/latest/pyserial_api.html
# -- CONFIG --
port = ""
BAUD_RATE = 115200

# -- ------ --

if port == "":
    for _port in serial.tools.list_ports.comports():
        if "UART Bridge Controller" in _port.description:
            port = _port

baud = os.getenv('IDF_MONITOR_BAUD') or os.getenv('MONITORBAUD') or BAUD_RATE
# see: esp-idf/tools/idf_py_actions/serial_ext.py

ser = serial.Serial(
    port="/dev/ttyUSB0",
    baudrate=baud,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=0)

sio = io.TextIOWrapper(io.BufferedRWPair(ser, ser))

print("connected to: " + ser.portstr)

buffer = ""

while True:
    bytes = ser.readline()
    bytes = bytes.decode("utf-8")

    if bytes == "":
        continue

    buffer += bytes

    if buffer[-1] == "\n":
        print(buffer[0:-2])
        buffer = ""

ser.close()
