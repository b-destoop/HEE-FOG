import os
import queue
import re
import threading
import time
from sys import platform

import matplotlib.animation as animation
import matplotlib.pyplot as plt
import pandas as pd
import serial.tools.list_ports

# serial info https://espressif-docs.readthedocs-hosted.com/projects/esp-idf/en/v3.3.5/get-started/establish-serial-connection.html
# https://pyserial.readthedocs.io/en/latest/pyserial_api.html
# -- CONFIG --
PORT = ""
BAUD_RATE = 115200
MS_BETWEEN_READS = 200
PLOTTING_FRAMES_WDW = 20

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
            lines_queue.put(buffer)
            print(buffer)
            buffer = ""
            ser.flushInput()

        time.sleep(MS_BETWEEN_READS / 1000)

    ser.close()


def str_to_float_list(string: str):
    string = string[1:-1]
    list_str = string.split(",")
    list_float = []
    for word in list_str:
        if word == ' ':
            continue
        list_float.append(float(word))
    return list_float


def animate(i, axs):
    global dataframe_raw, dataframe_FFT
    # Read all data from queue
    data_txt = lines_queue.get()
    df_dict = serial_txt_to_dict(data_txt)

    # Update the dataframe where data is stored
    dataframe_in = pd.DataFrame(df_dict, index=[0])
    dataframe_raw = pd.concat([dataframe_raw, dataframe_in], ignore_index=True, join="inner")
    # print(dataframe)

    if "FFT_array_in" in dataframe_in and "FFT_array_der" in dataframe_in:
        dataframe_FFT = pd.concat(
            [dataframe_FFT,
             dataframe_in[["FFT_array_in", "FFT_array_der", "FFT_max_freq"]]
             ]
        )
        fft_arr_in_latest_str = dataframe_FFT.tail(1)["FFT_array_in"][0]
        fft_arr_in_latest = str_to_float_list(fft_arr_in_latest_str)

        ax: plt.Axes = axs[0][1]
        ax.clear()
        ax.set_title("FFT")
        ax.bar(range(len(fft_arr_in_latest)), fft_arr_in_latest)

    # Limit x and y lists to items
    xs = dataframe_raw.tail(PLOTTING_FRAMES_WDW)["ts"]  # the last PLOTTING_FRAMES_WDW rows
    xs = xs / 1000  # ms => s

    # Draw plots
    draw_plot_for_data(axs, (0, 0), 'raw XYZ accelerometer data', xs, ["X_raw", "Y_raw", "Z_raw"])
    draw_plot_for_data(axs, (1, 0), 'adjusted XYZ accelerometer data', xs,
                       ["X_acc_der", "Y_acc_der", "Z_acc_der"])
    draw_plot_for_data(axs, (1, 1), 'gyro data', xs, ["X_gyr_der", "Y_gyr_der", "Z_gyr_der"])


def draw_plot_for_data(axs, subplot_coord: (int, int), subplot_title: str, x_contents, plot_contents: list[str]):
    global dataframe_raw

    ax: plt.Axes = axs[subplot_coord[0]][subplot_coord[1]]
    ax.clear()
    for data_title in plot_contents:
        ax.plot(x_contents, dataframe_raw.tail(PLOTTING_FRAMES_WDW)[data_title], label=data_title)
    ax.set_title(subplot_title)
    ax.legend(loc='upper left')


def serial_txt_to_dict(data_txt):
    # Extracting variable names and values using regular expressions
    matches = re.findall(r'(\w+)=([^;]+);', data_txt)
    # adding variables and values in their respective columns in a pandas dataframe
    df_dict = {}
    for match in matches:
        variable_name, value = match
        try:
            df_dict[variable_name] = float(value) if '[' not in value else value
        except ValueError as e:
            print(e)
            df_dict[variable_name] = 0

    # print(df_dict)
    return df_dict


def handle_data():
    fig, axs = plt.subplots(2, 2)

    # Set up plot to call animate() function periodically
    # interval = 0 because blocking queue.get in animate function
    ani = animation.FuncAnimation(fig, animate, fargs=(axs,), interval=1)
    plt.show()

    while True:
        continue


thread_serial = threading.Thread(target=get_serial_data, name="serial thread", daemon=True)
thread_serial.start()

dataframe_raw = pd.DataFrame(serial_txt_to_dict(lines_queue.get()), index=[0])
dataframe_FFT = pd.DataFrame([])
handle_data()

thread_serial.join()
