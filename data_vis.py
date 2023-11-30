import os
import re
import threading
import time

import pandas
import serial.tools.list_ports
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import datetime as dt
import queue
import pandas as pd

# serial info https://espressif-docs.readthedocs-hosted.com/projects/esp-idf/en/v3.3.5/get-started/establish-serial-connection.html
# https://pyserial.readthedocs.io/en/latest/pyserial_api.html
# -- CONFIG --
PORT = ""
BAUD_RATE = 115200

MS_BETWEEN_READS = 50

lines_queue = queue.Queue()


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

        if buffer[0:2] != "ts":
            buffer = ""
            continue

        if buffer[-1] == "\n":
            buffer = buffer[0:-2]  # trim off the \r and \n at the end of each line
            # print(buffer)
            lines_queue.put(buffer)
            buffer = ""

        time.sleep(MS_BETWEEN_READS / 1000)

    ser.close()


dataframe = pd.DataFrame([])


def animate(i, ax):
    global dataframe
    # Read all data from queue
    data_txt = lines_queue.get()

    df_dict = serial_txt_to_dict(data_txt)

    # pd.concat([dataframe, pd.DataFrame(df_dict, index=[0])])
    dataframe = pd.concat([dataframe, pd.DataFrame(df_dict, index=[0])], ignore_index=True)
    # print(dataframe)

    # Limit x and y lists to items
    xs = dataframe.tail(60)["ts"] # the last 60 rows
    ys = dataframe.tail(60)["X_raw"]

    # Draw x and y lists
    ax.clear()
    ax.plot(xs, ys)

    # Format plot
    plt.xticks(rotation=45, ha='right')
    plt.subplots_adjust(bottom=0.30)
    plt.title('NOT TMP102 Temperature over Time')
    plt.ylabel('NOT Temperature (deg C)')


def serial_txt_to_dict(data_txt):
    # Extracting variable names and values using regular expressions
    matches = re.findall(r'(\w+)=([^;]+);', data_txt)
    # adding variables and values in their respective columns in a pandas dataframe
    df_dict = {}
    for match in matches:
        variable_name, value = match
        try:
            df_dict[variable_name] = float(value) if '[' not in value else print(value)
        except ValueError as e:
            print(e)
            df_dict[variable_name] = 0

    # print(df_dict)
    return df_dict


def handle_data():
    fig = plt.figure()
    ax = fig.add_subplot(1, 1, 1)

    # Set up plot to call animate() function periodically
    # interval = 0 because blocking queue.get in animate function
    ani = animation.FuncAnimation(fig, animate, fargs=(ax,), interval=0)
    plt.show()

    while True:
        continue


thread_serial = threading.Thread(target=get_serial_data, name="serial thread", daemon=True)
thread_serial.start()
handle_data()
thread_serial.join()
