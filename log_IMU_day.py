# ejri

# Tree dynamics sensing device. Collecting data all day, and saving data into
# a csv locally.
# the same csv file is uploaded into
#    influxDB at local host
#    influxDB at UDL
#    Azure blob storage

# file is 1.2gb at the end of each day, for 60 samples a minute
# so delete it afterwards?

import serial.tools.list_ports
import serial
import re
import subprocess
import datetime
import psutil
import schedule


# Use with other I2C sensors would be similar.
import adafruit_tca9548a
import adafruit_mpu6050
import asyncio
import board
import busio
import json
import RPi.GPIO as GPIO
import numpy as np
import smbus
import time


import csv
import concurrent.futures
import threading
from timeit import default_timer as timer
import os
import pandas as pd

# Azure
from azure.iot.device.aio import IoTHubDeviceClient

# influxDB at UDL
from influxdb_client import InfluxDBClient, Point, WriteOptions

# influxDB at Localhost
from influxdb import InfluxDBClient as iflux_localhost

# # Azure
# from azure.iot.device.aio import IoTHubDeviceClient

# # influxDB at UDL
# from influxdb_client import InfluxDBClient, Point, WriteOptions

# # influxDB at Localhost
# from influxdb import InfluxDBClient as iflux_localhost


# save csv file daily
path = "/home/pi/sensorsnetwork/data/IMU_data_logger/"
filename = "PinOak_IMU_" + time.strftime("%Y_%m_%d") + ".csv"
path_filname = os.path.join(path, filename)


####### LSM9DS1 IMU on the TCA9548A multiplexer #####
# Create I2C bus as normal
i2c = busio.I2C(board.SCL, board.SDA)

# Create the TCA9548A object and give it the I2C bus
tca = adafruit_tca9548a.TCA9548A(i2c)

# For each sensor, read using the TCA9548A channel instead of the I2C object

mpu1 = adafruit_mpu6050.MPU6050(tca[2])
mpu2 = adafruit_mpu6050.MPU6050(tca[3])
mpu3 = adafruit_mpu6050.MPU6050(tca[4])
mpu4 = adafruit_mpu6050.MPU6050(tca[5])
mpu5 = adafruit_mpu6050.MPU6050(tca[6])

# Setting pin #24 as output. Example: in
# setting up device twin, connect an indicator LED at pin24
GPIO.setmode(GPIO.BCM)
LED_pin = 24
GPIO.setup(LED_pin, GPIO.OUT)


def start_timing(time_to_stop):
    stay_connected = True
    start = timer()
    with open(path_filname, mode="a") as log:
        sensor_write = csv.writer(
            log, delimiter=",", quotechar='"', quoting=csv.QUOTE_MINIMAL
        )
        write_to_log = sensor_write.writerow(
            [
                "datetime",
                "DeviceID",
                "BoardType",
                "GSM_module",
                "Location",
                "TreeType",
                "ax1",
                "ay1",
                "az1",
                "gx1",
                "gy1",
                "gz1",
                "temp1",
                "ax2",
                "ay2",
                "az2",
                "gx2",
                "gy2",
                "gz2",
                "temp2",
                "ax3",
                "ay3",
                "az3",
                "gx3",
                "gy3",
                "gz3",
                "temp3",
                "ax4",
                "ay4",
                "az4",
                "gx4",
                "gy4",
                "gz4",
                "temp4",
                "ax5",
                "ay5",
                "az5",
                "gx5",
                "gy5",
                "gz5",
                "temp5",
            ]
        )
    try:
        while stay_connected == True:
            time.sleep(1)
            end = timer()
            time_difference = end - start
            with open(path_filname, mode="a") as log:

                try:
                    ax1, ay1, az1 = mpu1.acceleration
                    gx1, gy1, gz1 = mpu1.gyro
                    [ax1, ay1, az1, gx1, gy1, gz1] = np.round(
                        [ax1, ay1, az1, gx1, gy1, gz1], 2
                    )
                    temp1 = "{0:0.2f}".format(mpu1.temperature)

                    ax2, ay2, az2 = mpu2.acceleration
                    gx2, gy2, gz2 = mpu2.gyro
                    [ax2, ay2, az2, gx2, gy2, gz2] = np.round(
                        [ax2, ay2, az2, gx2, gy2, gz2], 2
                    )
                    temp2 = "{0:0.2f}".format(mpu2.temperature)

                    ax3, ay3, az3 = mpu3.acceleration
                    gx3, gy3, gz3 = mpu3.gyro
                    [ax3, ay3, az3, gx3, gy3, gz3] = np.round(
                        [ax3, ay3, az3, gx3, gy3, gz3], 2
                    )
                    temp3 = "{0:0.2f}".format(mpu3.temperature)

                    ax4, ay4, az4 = mpu4.acceleration
                    gx4, gy4, gz4 = mpu4.gyro
                    [ax4, ay4, az4, gx4, gy4, gz4] = np.round(
                        [ax4, ay4, az4, gx4, gy4, gz4], 2
                    )
                    temp4 = "{0:0.2f}".format(mpu4.temperature)

                    ax5, ay5, az5 = mpu5.acceleration
                    gx5, gy5, gz5 = mpu5.gyro
                    [ax5, ay5, az5, gx5, gy5, gz5] = np.round(
                        [ax5, ay5, az5, gx5, gy5, gz5], 2
                    )
                    temp5 = "{0:0.2f}".format(mpu5.temperature)

                except:
                    i2c = busio.I2C(board.SCL, board.SDA)
                    # Create the TCA9548A object and give it the I2C bus
                    tca = adafruit_tca9548a.TCA9548A(i2c)

                    # For each sensor, read using the TCA9548A channel instead of the I2C object

                    mpu1 = adafruit_mpu6050.MPU6050(tca[2])
                    mpu2 = adafruit_mpu6050.MPU6050(tca[3])
                    mpu3 = adafruit_mpu6050.MPU6050(tca[4])
                    mpu4 = adafruit_mpu6050.MPU6050(tca[5])
                    mpu5 = adafruit_mpu6050.MPU6050(tca[6])
                    ax1, ay1, az1 = mpu1.acceleration
                    gx1, gy1, gz1 = mpu1.gyro
                    [ax1, ay1, az1, gx1, gy1, gz1] = np.round(
                        [ax1, ay1, az1, gx1, gy1, gz1], 2
                    )
                    temp1 = "{0:0.2f}".format(mpu1.temperature)

                    ax2, ay2, az2 = mpu2.acceleration
                    gx2, gy2, gz2 = mpu2.gyro
                    [ax2, ay2, az2, gx2, gy2, gz2] = np.round(
                        [ax2, ay2, az2, gx2, gy2, gz2], 2
                    )
                    temp2 = "{0:0.2f}".format(mpu2.temperature)

                    ax3, ay3, az3 = mpu3.acceleration
                    gx3, gy3, gz3 = mpu3.gyro
                    [ax3, ay3, az3, gx3, gy3, gz3] = np.round(
                        [ax3, ay3, az3, gx3, gy3, gz3], 2
                    )
                    temp3 = "{0:0.2f}".format(mpu3.temperature)

                    ax4, ay4, az4 = mpu4.acceleration
                    gx4, gy4, gz4 = mpu4.gyro
                    [ax4, ay4, az4, gx4, gy4, gz4] = np.round(
                        [ax4, ay4, az4, gx4, gy4, gz4], 2
                    )
                    temp4 = "{0:0.2f}".format(mpu4.temperature)

                    ax5, ay5, az5 = mpu5.acceleration
                    gx5, gy5, gz5 = mpu5.gyro
                    [ax5, ay5, az5, gx5, gy5, gz5] = np.round(
                        [ax5, ay5, az5, gx5, gy5, gz5], 2
                    )
                    temp5 = "{0:0.2f}".format(mpu5.temperature)

                deviceID = "GreenSpaceTree9"
                BoardType = "RPi4"
                gsm_mod = "BG96"
                location = "FairView"
                TreeType = "PinOak"
                sensor_write = csv.writer(
                    log, delimiter=",", quotechar='"', quoting=csv.QUOTE_MINIMAL
                )
                write_to_log = sensor_write.writerow(
                    [
                        datetime.datetime.utcnow().isoformat(),
                        deviceID,
                        BoardType,
                        gsm_mod,
                        location,
                        TreeType,
                        str(ax1),
                        str(ay1),
                        str(az1),
                        str(gx1),
                        str(gy1),
                        str(gz1),
                        str(temp1),
                        str(ax2),
                        str(ay2),
                        str(az2),
                        str(gx2),
                        str(gy2),
                        str(gz2),
                        str(temp2),
                        str(ax3),
                        str(ay3),
                        str(az3),
                        str(gx3),
                        str(gy3),
                        str(gz3),
                        str(temp3),
                        str(ax4),
                        str(ay4),
                        str(az4),
                        str(gx4),
                        str(gy4),
                        str(gz4),
                        str(temp4),
                        str(ax5),
                        str(ay5),
                        str(az5),
                        str(gx5),
                        str(gy5),
                        str(gz5),
                        str(temp5),
                    ]
                )

            if time_difference > time_to_stop:
                print("transferring data to local influxDB")
                # at the end of the day:
                # open the csv file, and trasnsfer it to:
                # influxdb locally
                # influxdb at UDL
                # Azure

                # influx configuration - edit these
                ifuser = ""
                ifpass = ""
                ifdb = "ground_sensors"
                ifhost = "localhost"
                ifport = 8086

                csvRead=pd.read_csv(path_filname,skiprows=[1])

                for row_index, column_value in csvRead.iterrows():
                    # format the data as a single measurement for influx

                    body = [
                        {
                            "measurement": "Tree_dynamics",
                            "tags": {
                                "location": location + " , " + TreeType,
                                "Device ID": deviceID + " , " + BoardType,
                                "cellular connection": "LTE-M catM",
                                "modem": gsm_mod,
                            },
                            "fields": {
                                "MPU_1_aX": (column_value[6]),
                                "MPU_1_aY": (column_value[7]),
                                "MPU_1_aZ": (column_value[8]),
                                "MPU_1_gX": (column_value[9]),
                                "MPU_1_gY": (column_value[10]),
                                "MPU_1_gZ": (column_value[11]),
                                "MPU_1_temp": (column_value[12]),
                                "MPU_2_aX": (column_value[13]),
                                "MPU_2_aY": (column_value[14]),
                                "MPU_2_aZ": (column_value[15]),
                                "MPU_2_gX": (column_value[16]),
                                "MPU_2_gY": (column_value[17]),
                                "MPU_2_gZ": (column_value[18]),
                                "MPU_2_temp": (column_value[19]),
                                "MPU_3_aX": (column_value[20]),
                                "MPU_3_aY": (column_value[21]),
                                "MPU_3_aZ": (column_value[22]),
                                "MPU_3_gX": (column_value[23]),
                                "MPU_3_gY": (column_value[24]),
                                "MPU_3_gZ": (column_value[25]),
                                "MPU_3_temp": (column_value[26]),
                                "MPU_4_aX": (column_value[27]),
                                "MPU_4_aY": (column_value[28]),
                                "MPU_4_aZ": (column_value[29]),
                                "MPU_4_gX": (column_value[30]),
                                "MPU_4_gY": (column_value[31]),
                                "MPU_4_gZ": (column_value[32]),
                                "MPU_4_temp": (column_value[33]),
                                "MPU_5_aX": (column_value[34]),
                                "MPU_5_aY": (column_value[35]),
                                "MPU_5_aZ": (column_value[36]),
                                "MPU_5_gX": (column_value[37]),
                                "MPU_5_gY": (column_value[38]),
                                "MPU_5_gZ": (column_value[39]),
                                "MPU_5_temp": (column_value[40]),
                                "inet6_1": 1,  # ip_addresses[0],
                                "inet6_2": 1,  # ip_addresses[1],
                                "inet6_3": 1,  # ip_addresses[2],
                            },
                            "time": pd.to_datetime(column_value[0]),
                        }
                    ]

                    # connect to influx
                    ifclient = iflux_localhost(ifhost, ifport, ifuser, ifpass, ifdb)

                    # write the measurement
                    ifclient.write_points(body)

                print(
                    "Successfully Uploaded Tree Dynamics Data to InfluxDB @ localhost"
                )
                time.sleep(2)
                time.sleep(0.2)
                break
    except:
        i2c = busio.I2C(board.SCL, board.SDA)
        # Create the TCA9548A object and give it the I2C bus
        tca = adafruit_tca9548a.TCA9548A(i2c)

        # For each sensor, read using the TCA9548A channel instead of the I2C object

        mpu1 = adafruit_mpu6050.MPU6050(tca[2])
        mpu2 = adafruit_mpu6050.MPU6050(tca[3])
        mpu3 = adafruit_mpu6050.MPU6050(tca[4])
        mpu4 = adafruit_mpu6050.MPU6050(tca[5])
        mpu5 = adafruit_mpu6050.MPU6050(tca[6])

        while stay_connected == True:
            end = timer()
            time_difference = end - start
            with open(path_filname, mode="a") as log:

                try:
                    ax1, ay1, az1 = mpu1.acceleration
                    gx1, gy1, gz1 = mpu1.gyro
                    [ax1, ay1, az1, gx1, gy1, gz1] = np.round(
                        [ax1, ay1, az1, gx1, gy1, gz1], 2
                    )
                    temp1 = "{0:0.2f}".format(mpu1.temperature)

                    ax2, ay2, az2 = mpu2.acceleration
                    gx2, gy2, gz2 = mpu2.gyro
                    [ax2, ay2, az2, gx2, gy2, gz2] = np.round(
                        [ax2, ay2, az2, gx2, gy2, gz2], 2
                    )
                    temp2 = "{0:0.2f}".format(mpu2.temperature)

                    ax3, ay3, az3 = mpu3.acceleration
                    gx3, gy3, gz3 = mpu3.gyro
                    [ax3, ay3, az3, gx3, gy3, gz3] = np.round(
                        [ax3, ay3, az3, gx3, gy3, gz3], 2
                    )
                    temp3 = "{0:0.2f}".format(mpu3.temperature)

                    ax4, ay4, az4 = mpu4.acceleration
                    gx4, gy4, gz4 = mpu4.gyro
                    [ax4, ay4, az4, gx4, gy4, gz4] = np.round(
                        [ax4, ay4, az4, gx4, gy4, gz4], 2
                    )
                    temp4 = "{0:0.2f}".format(mpu4.temperature)

                    ax5, ay5, az5 = mpu5.acceleration
                    gx5, gy5, gz5 = mpu5.gyro
                    [ax5, ay5, az5, gx5, gy5, gz5] = np.round(
                        [ax5, ay5, az5, gx5, gy5, gz5], 2
                    )
                    temp5 = "{0:0.2f}".format(mpu5.temperature)

                except:
                    i2c = busio.I2C(board.SCL, board.SDA)
                    # Create the TCA9548A object and give it the I2C bus
                    tca = adafruit_tca9548a.TCA9548A(i2c)

                    # For each sensor, read using the TCA9548A channel instead of the I2C object

                    mpu1 = adafruit_mpu6050.MPU6050(tca[2])
                    mpu2 = adafruit_mpu6050.MPU6050(tca[3])
                    mpu3 = adafruit_mpu6050.MPU6050(tca[4])
                    mpu4 = adafruit_mpu6050.MPU6050(tca[5])
                    mpu5 = adafruit_mpu6050.MPU6050(tca[6])
                    ax1, ay1, az1 = mpu1.acceleration
                    gx1, gy1, gz1 = mpu1.gyro
                    [ax1, ay1, az1, gx1, gy1, gz1] = np.round(
                        [ax1, ay1, az1, gx1, gy1, gz1], 2
                    )
                    temp1 = "{0:0.2f}".format(mpu1.temperature)

                    ax2, ay2, az2 = mpu2.acceleration
                    gx2, gy2, gz2 = mpu2.gyro
                    [ax2, ay2, az2, gx2, gy2, gz2] = np.round(
                        [ax2, ay2, az2, gx2, gy2, gz2], 2
                    )
                    temp2 = "{0:0.2f}".format(mpu2.temperature)

                    ax3, ay3, az3 = mpu3.acceleration
                    gx3, gy3, gz3 = mpu3.gyro
                    [ax3, ay3, az3, gx3, gy3, gz3] = np.round(
                        [ax3, ay3, az3, gx3, gy3, gz3], 2
                    )
                    temp3 = "{0:0.2f}".format(mpu3.temperature)

                    ax4, ay4, az4 = mpu4.acceleration
                    gx4, gy4, gz4 = mpu4.gyro
                    [ax4, ay4, az4, gx4, gy4, gz4] = np.round(
                        [ax4, ay4, az4, gx4, gy4, gz4], 2
                    )
                    temp4 = "{0:0.2f}".format(mpu4.temperature)

                    ax5, ay5, az5 = mpu5.acceleration
                    gx5, gy5, gz5 = mpu5.gyro
                    [ax5, ay5, az5, gx5, gy5, gz5] = np.round(
                        [ax5, ay5, az5, gx5, gy5, gz5], 2
                    )
                    temp5 = "{0:0.2f}".format(mpu5.temperature)

                deviceID = "GreenSpaceTree9"
                BoardType = "RPi4"
                gsm_mod = "BG96"
                location = "FairView"
                TreeType = "PinOak"
                time.sleep(0.5)
                sensor_write = csv.writer(
                    log, delimiter=",", quotechar='"', quoting=csv.QUOTE_MINIMAL
                )
                write_to_log = sensor_write.writerow(
                    [
                        datetime.datetime.utcnow().isoformat(),
                        deviceID,
                        BoardType,
                        gsm_mod,
                        location,
                        TreeType,
                        str(ax1),
                        str(ay1),
                        str(az1),
                        str(gx1),
                        str(gy1),
                        str(gz1),
                        str(temp1),
                        str(ax2),
                        str(ay2),
                        str(az2),
                        str(gx2),
                        str(gy2),
                        str(gz2),
                        str(temp2),
                        str(ax3),
                        str(ay3),
                        str(az3),
                        str(gx3),
                        str(gy3),
                        str(gz3),
                        str(temp3),
                        str(ax4),
                        str(ay4),
                        str(az4),
                        str(gx4),
                        str(gy4),
                        str(gz4),
                        str(temp4),
                        str(ax5),
                        str(ay5),
                        str(az5),
                        str(gx5),
                        str(gy5),
                        str(gz5),
                        str(temp5),
                    ]
                )

            if time_difference > time_to_stop:
                print("transferring data to local influxDB")
                # at the end of the day:
                # open the csv file, and trasnsfer it to:
                # influxdb locally
                # influxdb at UDL
                # Azure

                # influx configuration - edit these
                ifuser = ""
                ifpass = ""
                ifdb = "ground_sensors"
                ifhost = "localhost"
                ifport = 8086

                csvRead=pd.read_csv(path_filname,skiprows=[1])

                for row_index, column_value in csvRead.iterrows():
                    # format the data as a single measurement for influx
                    body = [
                        {
                            "measurement": "Tree_dynamics",
                            "tags": {
                                "location": location + " , " + TreeType,
                                "Device ID": deviceID + " , " + BoardType,
                                "cellular connection": "LTE-M catM",
                                "modem": gsm_mod,
                            },
                            "fields": {
                                "MPU_1_aX": (column_value[6]),
                                "MPU_1_aY": (column_value[7]),
                                "MPU_1_aZ": (column_value[8]),
                                "MPU_1_gX": (column_value[9]),
                                "MPU_1_gY": (column_value[10]),
                                "MPU_1_gZ": (column_value[11]),
                                "MPU_1_temp": (column_value[12]),
                                "MPU_2_aX": (column_value[13]),
                                "MPU_2_aY": (column_value[14]),
                                "MPU_2_aZ": (column_value[15]),
                                "MPU_2_gX": (column_value[16]),
                                "MPU_2_gY": (column_value[17]),
                                "MPU_2_gZ": (column_value[18]),
                                "MPU_2_temp": (column_value[19]),
                                "MPU_3_aX": (column_value[20]),
                                "MPU_3_aY": (column_value[21]),
                                "MPU_3_aZ": (column_value[22]),
                                "MPU_3_gX": (column_value[23]),
                                "MPU_3_gY": (column_value[24]),
                                "MPU_3_gZ": (column_value[25]),
                                "MPU_3_temp": (column_value[26]),
                                "MPU_4_aX": (column_value[27]),
                                "MPU_4_aY": (column_value[28]),
                                "MPU_4_aZ": (column_value[29]),
                                "MPU_4_gX": (column_value[30]),
                                "MPU_4_gY": (column_value[31]),
                                "MPU_4_gZ": (column_value[32]),
                                "MPU_4_temp": (column_value[33]),
                                "MPU_5_aX": (column_value[34]),
                                "MPU_5_aY": (column_value[35]),
                                "MPU_5_aZ": (column_value[36]),
                                "MPU_5_gX": (column_value[37]),
                                "MPU_5_gY": (column_value[38]),
                                "MPU_5_gZ": (column_value[39]),
                                "MPU_5_temp": (column_value[40]),
                                "inet6_1": 1,  # ip_addresses[0],
                                "inet6_2": 1,  # ip_addresses[1],
                                "inet6_3": 1,  # ip_addresses[2],
                            },
                            "time": pd.to_datetime(column_value[0]),
                        }
                    ]

                    # connect to influx
                    ifclient = iflux_localhost(ifhost, ifport, ifuser, ifpass, ifdb)

                    # write the measurement
                    ifclient.write_points(body)

                print(
                    "Successfully Uploaded Tree Dynamics Data to InfluxDB @ localhost"
                )
                time.sleep(2)
                time.sleep(0.2)
                break

    print("Temperature logged")
    return stay_connected


def log_data():
    print("Logging RPi internal temperature")
    stay_connected = True
    print(stay_connected)


with concurrent.futures.ThreadPoolExecutor() as executer:
    f1 = executer.submit(log_data)
    # time_to_stop = 30 seconds
    f2 = executer.submit(start_timing, 60)
    print(f2.result())
