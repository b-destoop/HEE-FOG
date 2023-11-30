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
from sys import platform

# serial info https://espressif-docs.readthedocs-hosted.com/projects/esp-idf/en/v3.3.5/get-started/establish-serial-connection.html
# https://pyserial.readthedocs.io/en/latest/pyserial_api.html
# -- CONFIG --
PORT = ""
BAUD_RATE = 115200

MS_BETWEEN_READS = 50

lines_queue = queue.Queue()


def get_serial_data():
    port_name = PORT
    if PORT == "":
        for _port in serial.tools.list_ports.comports():
            if "UART Bridge Controller" in _port.description:
                port = _port

    baud = os.getenv('IDF_MONITOR_BAUD') or os.getenv('MONITORBAUD') or BAUD_RATE
    # see: esp-idf/tools/idf_py_actions/serial_ext.py

    if platform == "linux" or platform == "linux2":
        port_name = "/dev/" + port.name or PORT
    elif platform == "darwin":
        pass
        # OS X
    elif platform == "win32":
        port_name = port.name

    ser = serial.Serial(
            port=port_name,
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
            print(lines_queue.qsize())
            buffer = ""

        time.sleep(MS_BETWEEN_READS / 1000)

    ser.close()


dataframe = pd.DataFrame([])


def animate(i, axs):
    global dataframe
    # Read all data from queue
    data_txt = lines_queue.get()

    df_dict = serial_txt_to_dict(data_txt)

    # pd.concat([dataframe, pd.DataFrame(df_dict, index=[0])])
    dataframe = pd.concat([dataframe, pd.DataFrame(df_dict, index=[0])], ignore_index=True)
    # print(dataframe)

    # Draw plots
    draw_plot_for_data(axs, (0, 0), 'raw XYZ accelerometer data', ["X_raw", "Y_raw", "Z_raw"])
    draw_plot_for_data(axs, (1, 0), 'adjusted XYZ accelerometer data', ["X_acc_der", "Y_acc_der", "Z_acc_der"])
    draw_plot_for_data(axs, (1, 1), 'gyro data', ["X_gyr_der", "Y_gyr_der", "Z_gyr_der"])


def draw_plot_for_data(axs, subplot_coord: (int, int), subplot_title: str, plot_contents: list[str]):
    global dataframe

    # Limit x and y lists to items
    xs = dataframe.tail(60)["ts"]  # the last 60 rows
    xs = xs / 1000  # ms => s

    xyz_raw: plt.Axes = axs[subplot_coord[0]][subplot_coord[1]]
    xyz_raw.clear()
    for data_title in plot_contents:
        xyz_raw.plot(xs, dataframe.tail(60)[data_title], label=data_title)
    xyz_raw.set_title(subplot_title)
    xyz_raw.legend(loc='upper left')


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
    fig, axs = plt.subplots(2, 2)

    # Set up plot to call animate() function periodically
    # interval = 0 because blocking queue.get in animate function
    ani = animation.FuncAnimation(fig, animate, fargs=(axs,), interval=0)
    plt.show()

    while True:
        continue


thread_serial = threading.Thread(target=get_serial_data, name="serial thread", daemon=True)
thread_serial.start()
handle_data()
thread_serial.join()
