import os
import threading

import serial.tools.list_ports
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import datetime as dt

# serial info https://espressif-docs.readthedocs-hosted.com/projects/esp-idf/en/v3.3.5/get-started/establish-serial-connection.html
# https://pyserial.readthedocs.io/en/latest/pyserial_api.html
# -- CONFIG --
PORT = ""
BAUD_RATE = 115200


def get_serial_data():
    if PORT == "":
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


def animate(i, ax, xs, ys):
    # Read temperature (Celsius) from TMP102
    value = 10

    # Add x and y to lists
    xs.append(dt.datetime.now().strftime('%H:%M:%S.%f'))
    ys.append(value)

    # Limit x and y lists to 20 items
    xs = xs[-100:]
    ys = ys[-100:]

    # Draw x and y lists
    ax.clear()
    ax.plot(xs, ys)

    # Format plot
    plt.xticks(rotation=45, ha='right')
    plt.subplots_adjust(bottom=0.30)
    plt.title('TMP102 Temperature over Time')
    plt.ylabel('Temperature (deg C)')


def handle_data():
    fig = plt.figure()
    ax = fig.add_subplot(1, 1, 1)

    xs = []
    ys = []

    # Set up plot to call animate() function periodically
    ani = animation.FuncAnimation(fig, animate, fargs=(ax, xs, ys), interval=1000)
    plt.show()
    while True:
        continue


thread_serial = threading.Thread(target=get_serial_data, name="serial thread")
thread_handle_data = threading.Thread(target=handle_data, name="handle data thread")

thread_serial.start()
thread_handle_data.start()

thread_serial.join()
thread_handle_data.join()
