# ejri

# Soil sensing device
# Fair View Grove
# Tree ID:
# RPi 4 connected to Teros 12 sensors to measure soil temperature, volumetric soil water content,
# dielectric constant at a range of frequencies, and soil electical conductivity

## connections: (Meter Group's) Teros 12 sensor conencted using Dr. John Lui's sdi-12 adapter (https://liudr.wordpress.com/gadget/sdi-12-usb-adapter/)
# parts of the code adapted from Dr. John Lui's code (sdi_12_reading_v1_0.py)


# Data is uploaded to Azure IoT Hub, UDL (InfluxDB), and saved locally (InfluxDB, localhost)


# Notes:
# conencting the devices through a port by id is recommended to prevent connections to wrong ports (GSM ports, etc.)
# each sensor needs to have its own address, individually changed using:
# ################### defining each sensor to its own unique address. default is 0, so change the other connected ones.
# ser.write(sdi_12_address + b"A1!") # Data logger command.
# Request to the sensor to change its address from 1 to a new address of 0.
# sdi_12_line = ser.readline()
# sdi_12_line = sdi_12_line[:-2]  # remove \r and \n
# teros_address_info_1="Sensor address converted to 1:"+str(sdi_12_line.decode('UTF-8'))
# print(teros_address_info_1)
# read sensor individually


################
###############
## find the specific port a device is connected to:
# $ ls -l /dev/serial/by-id
###############
###############
###############
###############


#!/usr/local/opt/python-3.5.1/bin/python3.5
# Simple SDI-12 Sensor Reader Copyright Dr. John Liu
# https://liudr.files.wordpress.com/2021/02/sdi-12-sensors-usb-aadapter-user-manual.pdf

import serial.tools.list_ports
import serial
import time
import re
import datetime
import psutil
import schedule
import asyncio
import json
import numpy as np
import time

# Azure
from azure.iot.device.aio import IoTHubDeviceClient

# influxDB at UDL
from influxdb_client import InfluxDBClient, Point, WriteOptions

# influxDB at Localhost
from influxdb import InfluxDBClient as iflux_localhost


# define twin  function handle
def handle_twin(twin):
    print("Twin received", twin)
    if "desired" in twin:
        desired = twin["desired"]


async def main():

    conn_str = ""
    device_client = IoTHubDeviceClient.create_from_connection_string(conn_str)
    await device_client.connect()

    # Store the device name to open port with later in the script.
    port_device = "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AC00Z2TL-if00-port0"

    ser = serial.Serial(
        port=port_device,
        baudrate=9600,
        # parity=serial.PARITY_EVEN,
        # stopbits=serial.STOPBITS_ONE,
        timeout=10
        # bytesize=serial.SEVENBITS,
    )
    time.sleep(2.5)  # delay at least 1 second delay of the adapter.

    ser.write(
        b"?!"
    )  # Data logger command. Request for a response from any sensor listening on the data line.
    sdi_12_line = ser.readline()
    sdi_12_line = sdi_12_line[:-2]  # remove \r and \n since [0-9]$ has trouble with \r
    m = re.search(b"[0-9a-zA-Z]$", sdi_12_line)  # having trouble with the \r
    sdi_12_address = m.group(0)  # find address
    teros_address = "Sensor address:" + str(sdi_12_address.decode("UTF-8").strip())
    print(teros_address)
    # print("\nSensor address:", sdi_12_address.decode("utf-8"))

    ################### defining each sensor to its own unique address. default is 0, so change the other connected ones.
    # ser.write(sdi_12_address + b"A1!") # Data logger command.
    # Request to the sensor to change its address from 1 to a new address of 0.
    # sdi_12_line = ser.readline()
    # sdi_12_line = sdi_12_line[:-2]  # remove \r and \n
    # teros_address_info_1="Sensor address converted to 1:"+str(sdi_12_line.decode('UTF-8'))
    # print(teros_address_info_1)
    ################### defining each sensor to its own unique address. default is 0, so change the other connected ones.

    ser.write(b"2I!")  # Data logger command. Request information from sensor 2.
    sdi_12_line_2 = ser.readline()
    sdi_12_line_2 = sdi_12_line_2[:-2]  # remove \r and \n
    teros_address_info_2 = str(sdi_12_line_2.decode("UTF-8"))[17:]
    print(teros_address_info_2)
    # print("Sensor info:", sdi_12_line_2.decode("utf-8"))

    ser.write(b"2M!")  # Data logger command. Request information from sensor 1.
    sdi_12_line_2 = ser.readline()
    sdi_12_line_2 = ser.readline()
    ser.write(b"2D0!")  # Data logger command. Request all data from sensor 1.
    sdi_12_line_2 = ser.readline()
    sdi_12_line_2 = sdi_12_line_2[:-2]  # remove \r and \n
    teros_reading_2 = "Sensor " + str(sdi_12_line_2.decode("UTF-8"))
    print(teros_reading_2)
    k_2 = 1
    for index_2, value_2 in enumerate(teros_reading_2):
        if value_2 == "-":
            negative_value_2 = index_2
            k_2 = 0
    if k_2 == 0:
        teros_reading_2 = (
            teros_reading_2[:negative_value_2]
            + ("+")
            + teros_reading_2[negative_value_2:]
        )
        print(teros_reading_2)

    teros_2_data = [x for x in teros_reading_2.split("+")]
    teros_2_data[0] = teros_address_info_2
    print(teros_2_data)

    ser.write(b"1I!")  # Data logger command. Request information from sensor 1.
    sdi_12_line_1 = ser.readline()
    sdi_12_line_1 = sdi_12_line_1[:-2]  # remove \r and \n
    teros_address_info_1 = str(sdi_12_line_1.decode("UTF-8"))[17:]
    print(teros_address_info_1)
    # print("Sensor info:", sdi_12_line_1.decode("utf-8"))

    ser.write(b"1M!")  # Data logger command. Request information from sensor 1.
    sdi_12_line_1 = ser.readline()
    sdi_12_line_1 = ser.readline()
    ser.write(b"1D0!")  # Data logger command. Request all data from sensor 1.
    sdi_12_line_1 = ser.readline()
    sdi_12_line_1 = sdi_12_line_1[:-2]  # remove \r and \n
    teros_reading_1 = "Sensor " + str(sdi_12_line_1.decode("UTF-8"))
    print(teros_reading_1)
    k_1 = 1
    for index_1, value_1 in enumerate(teros_reading_1):
        if value_1 == "-":
            negative_value_1 = index_1
            k_1 = 0
    if k_1 == 0:
        teros_reading_1 = (
            teros_reading_1[:negative_value_1]
            + ("+")
            + teros_reading_1[negative_value_1:]
        )
        print(teros_reading_1)

    teros_1_data = [x for x in teros_reading_1.split("+")]
    teros_1_data[0] = teros_address_info_1
    print(teros_1_data)

    # print("Sensor reading:", sdi_12_line.decode("utf-8"))

    ser.write(b"0I!")  # Data logger command. Request information from sensor 0.
    sdi_12_line_0 = ser.readline()
    sdi_12_line_0 = sdi_12_line_0[:-2]  # remove \r and \n
    teros_address_info_0 = str(sdi_12_line_0.decode("UTF-8"))[17:]
    print(teros_address_info_0)
    # print("Sensor info:", sdi_12_line_0.decode("utf-8"))

    ser.write(b"0M!")  # Data logger command. Request information from sensor 0.
    sdi_12_line_0 = ser.readline()
    sdi_12_line_0 = ser.readline()
    ser.write(b"0D0!")  # Data logger command. Request all data from sensor 0.
    sdi_12_line_0 = ser.readline()
    sdi_12_line_0 = sdi_12_line_0[:-2]  # remove \r and \n
    teros_reading_0 = "Sensor " + str(sdi_12_line_0.decode("UTF-8"))
    print(teros_reading_0)
    k_0 = 1
    for index_0, value_0 in enumerate(teros_reading_0):
        if value_0 == "-":
            negative_value_0 = index_0
            k_0 = 0
    if k_0 == 0:
        teros_reading_0 = (
            teros_reading_0[:negative_value_0]
            + ("+")
            + teros_reading_0[negative_value_0:]
        )
        print(teros_reading_0)

    teros_0_data = [x for x in teros_reading_0.split("+")]
    teros_0_data[0] = teros_address_info_0
    print(teros_0_data)
    # print("Sensor reading:", sdi_12_line_0.decode("utf-8"))

    ### analog readings

    # ser.write(
    #     b"?!"
    # )  # Data logger command. Request for a response from any sensor listening on the data line.

    # ser.write(b"z!")
    # ser.write(b"zI!")  # Data logger command. Request information from sensor 2.
    # sdi_12_line_analog = ser.readline()
    # sdi_12_line_analog = sdi_12_line_analog[:-2]  # remove \r and \n
    # analog_firmware = str(sdi_12_line_analog.decode("UTF-8"))[17:]
    # print(analog_firmware)
    # # print("Sensor info:", sdi_12_line_analog.decode("utf-8"))

    ser.write(b"zM!")  #
    # Data logger command. Request all data from analog sensors.
    sdi_12_line_analog = ser.readline()
    sdi_12_line_analog = ser.readline()
    ser.write(b"zD0!")
    sdi_12_line_analog = ser.readline()
    sdi_12_line_analog = sdi_12_line_analog[:-2]  # remove \r and \n
    analog_reading = "Analog Sensor: " + str(sdi_12_line_analog.decode("UTF-8"))
    print(analog_reading)

    analog_data = [x for x in analog_reading.split("+")]
    analog_data[0] = "analog_reading"
    print(analog_data)

    ser.close()

    data = {}
    data["deviceID"] = "GreenSpaceTree3"
    data["board"] = "RPi4"
    data["gsm_mod"] = "BG96"
    data["location"] = "FairView"
    data["TreeType"] = "Red Oak"
    data["terros_15_serial"] = teros_0_data[0]
    data["terros_15_VWC"] = float(teros_0_data[1])
    data["terros_15_temp"] = float(teros_0_data[2])
    data["terros_15_EC"] = float(teros_0_data[3])
    data["terros_30_serial"] = teros_1_data[0]
    data["terros_30_VWC"] = float(teros_1_data[1])
    data["terros_30_temp"] = float(teros_1_data[2])
    data["terros_30_EC"] = float(teros_1_data[3])
    data["terros_60_serial"] = 1  # teros_2_data[0]
    data["terros_60_VWC"] = 1  # float(teros_2_data[1])
    data["terros_60_temp"] = 1  # float(teros_2_data[2])
    data["terros_60_EC"] = 1  # float(teros_2_data[3])
    data["commercial_15"] = float(analog_data[1])
    data["commercial_30"] = float(analog_data[2])
    data["commercial_60"] = float(analog_data[3])
    json_body = json.dumps(data)
    print("Sending message: ", json_body)
    await device_client.send_message(json_body)

    # get device twin status and command
    twin = await device_client.get_twin()
    handle_twin(twin)

    time.sleep(10)

    await device_client.disconnect()
    print("Successfully Uploaded Soil Data to Azure")

    # influx configuration - edit these
    _bucket = ""
    _org = ""
    _token = ""
    # Store the URL of your InfluxDB instance
    _url = "http://data.sustain.ubc.ca:8086/"

    # take a timestamp for this measurement
    time_utc = datetime.datetime.utcnow()

    with InfluxDBClient(url=_url, token=_token, org=_org) as _client:

        with _client.write_api(
            write_options=WriteOptions(
                batch_size=1_000,
                flush_interval=10_000,
                jitter_interval=2_000,
                retry_interval=5_000,
                max_retries=5,
                max_retry_delay=30_000,
                exponential_base=2,
            )
        ) as _write_client:

            _write_client.write(
                _bucket,
                _org,
                {
                    "measurement": "Soil",
                    "tags": {
                        "location": "FiarView, Red Oak",
                        "Device ID": "GreenSpaceTree3",
                        "cellular connection": "LTE-M catM",
                        "modem": "BG96",
                    },
                    "fields": {
                        "terros_15_serial": teros_0_data[0],
                        "terros_15_VWC": float(teros_0_data[1]),
                        "terros_15_temp": float(teros_0_data[2]),
                        "terros_15_EC": float(teros_0_data[3]),
                        "terros_30_serial": teros_1_data[0],
                        "terros_30_VWC": float(teros_1_data[1]),
                        "terros_30_temp": float(teros_1_data[2]),
                        "terros_30_EC": float(teros_1_data[3]),
                        "terros_60_serial": teros_2_data[0],
                        "terros_60_VWC": 1,  # float(teros_2_data[1]),
                        "terros_60_temp": 1,  # float(teros_2_data[2]),
                        "terros_60_EC": 1,  # float(teros_2_data[3]),
                        "commercial_15": float(analog_data[1]),
                        "commercial_30": float(analog_data[2]),
                        "commercial_60": float(analog_data[3]),
                    },
                    "time": time_utc,
                },
            )
    print("Successfully Uploaded Soil Data to InfluxDB @ UDL")

    # influx configuration - edit these
    ifuser = ""
    ifpass = ""
    ifdb = ""
    ifhost = "localhost"
    ifport = 8086

    # format the data as a single measurement for influx
    body = [
        {
            "measurement": "Soil",
            "tags": {
                "location": "FiarView, Red Oak",
                "Device ID": "GreenSpaceTree3",
                "cellular connection": "LTE-M catM",
                "modem": "BG96",
            },
            "fields": {
                "terros_15_serial": teros_0_data[0],
                "terros_15_VWC": float(teros_0_data[1]),
                "terros_15_temp": float(teros_0_data[2]),
                "terros_15_EC": float(teros_0_data[3]),
                "terros_30_serial": teros_1_data[0],
                "terros_30_VWC": float(teros_1_data[1]),
                "terros_30_temp": float(teros_1_data[2]),
                "terros_30_EC": float(teros_1_data[3]),
                "terros_60_serial": teros_2_data[0],
                "terros_60_VWC": 1,  # float(teros_2_data[1]),
                "terros_60_temp": 1,  # float(teros_2_data[2]),
                "terros_60_EC": 1,  # float(teros_2_data[3]),
                "commercial_15": float(analog_data[1]),
                "commercial_30": float(analog_data[2]),
                "commercial_60": float(analog_data[3]),
            },
            "time": time_utc,
        }
    ]

    # connect to influx
    ifclient = iflux_localhost(ifhost, ifport, ifuser, ifpass, ifdb)

    # write the measurement
    ifclient.write_points(body)
    await asyncio.sleep(2)
    print("Successfully Uploaded Soil Data to InfluxDB @ localhost")
    time.sleep(2)
    print("Stopping main()... at 120 seconds interval")


async def do_stuff_periodically(interval, periodic_function):
    while True:
        print("Starting main()")
        await asyncio.gather(
            asyncio.sleep(interval),
            periodic_function(),
        )


if __name__ == "__main__":
    while True:
        try:
            asyncio.run(do_stuff_periodically(120, main))

            # run anomaly detection model. if anomaly, collect multiple smaples
            anomaly_detection = 0
            if anomaly_detection == 1:
                print("Anomaly detected.")
                for x in range(0, 10):
                    asyncio.run(do_stuff_periodically(10, main))

        except:

            print(
                "Remote I/O error. Error connecting to device... trying again in 10 seconds."
            )
            time.sleep(10)
            continue
