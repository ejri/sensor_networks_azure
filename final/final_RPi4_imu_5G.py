# ejri

# Tree dynamics sensing device
# Fair View Grove
# Tree ID:
# RPi 4 connected to 5 IMUs, capturing tree dynamics

import serial.tools.list_ports
import serial
import re

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

# Azure
from azure.iot.device.aio import IoTHubDeviceClient

# influxDB at UDL
from influxdb_client import InfluxDBClient, Point, WriteOptions

# influxDB at Localhost
from influxdb import InfluxDBClient as iflux_localhost

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

# manually testing if the LED is connected properly
# GPIO.output(LED_pin, GPIO.HIGH)

# define twin  function handle
# this function controls LED at pin 24, through device twin
def handle_twin(twin):
    print("Twin received", twin)
    if "desired" in twin:
        desired = twin["desired"]
        if "led" in desired:
            GPIO.output(LED_pin, desired["led"])


async def main():

    try:
        conn_str = "HostName=GreenSpaces.azure-devices.net;DeviceId=GreenSpaceTree9;SharedAccessKey=2/xL/obaYf1/GwOhQ1a/J6O9nqyMzAhxQiZq2HvCYdM="
        device_client = IoTHubDeviceClient.create_from_connection_string(conn_str)
        await device_client.connect()
    except:
        print("Error Connecting to Azure")
        pass

    ax1, ay1, az1 = mpu1.acceleration
    gx1, gy1, gz1 = mpu1.gyro
    [ax1, ay1, az1, gx1, gy1, gz1] = np.round([ax1, ay1, az1, gx1, gy1, gz1], 2)
    temp1 = "{0:0.2f}".format(mpu1.temperature)

    ax2, ay2, az2 = mpu2.acceleration
    gx2, gy2, gz2 = mpu2.gyro
    [ax2, ay2, az2, gx2, gy2, gz2] = np.round([ax2, ay2, az2, gx2, gy2, gz2], 2)
    temp2 = "{0:0.2f}".format(mpu2.temperature)

    ax3, ay3, az3 = mpu3.acceleration
    gx3, gy3, gz3 = mpu3.gyro
    [ax3, ay3, az3, gx3, gy3, gz3] = np.round([ax3, ay3, az3, gx3, gy3, gz3], 2)
    temp3 = "{0:0.2f}".format(mpu3.temperature)

    ax4, ay4, az4 = mpu4.acceleration
    gx4, gy4, gz4 = mpu4.gyro
    [ax4, ay4, az4, gx4, gy4, gz4] = np.round([ax4, ay4, az4, gx4, gy4, gz4], 2)
    temp4 = "{0:0.2f}".format(mpu4.temperature)

    ax5, ay5, az5 = mpu5.acceleration
    gx5, gy5, gz5 = mpu5.gyro
    [ax5, ay5, az5, gx5, gy5, gz5] = np.round([ax5, ay5, az5, gx5, gy5, gz5], 2)
    temp5 = "{0:0.2f}".format(mpu5.temperature)

    print("MPU_1")
    print("Acceleration: X:%.2f, Y: %.2f, Z: %.2f m/s^2" % (mpu1.acceleration))
    print("Gyro X:%.2f, Y: %.2f, Z: %.2f degrees/s" % (mpu1.gyro))
    print("Temperature1: %.2f C" % mpu1.temperature)
    print("----------------------------------------------------")
    time.sleep(3)
    print("MPU_2")
    print("Acceleration: X:%.2f, Y: %.2f, Z: %.2f m/s^2" % (mpu2.acceleration))
    print("Gyro X:%.2f, Y: %.2f, Z: %.2f degrees/s" % (mpu2.gyro))
    print("Temperature2: %.2f C" % mpu2.temperature)
    print("----------------------------------------------------")
    time.sleep(3)
    print("MPU_3")
    print("Acceleration: X:%.2f, Y: %.2f, Z: %.2f m/s^2" % (mpu3.acceleration))
    print("Gyro X:%.2f, Y: %.2f, Z: %.2f degrees/s" % (mpu3.gyro))
    print("Temperature3: %.2f C" % mpu3.temperature)
    print("----------------------------------------------------")
    time.sleep(3)
    print("MPU_4")
    print("Acceleration: X:%.2f, Y: %.2f, Z: %.2f m/s^2" % (mpu4.acceleration))
    print("Gyro X:%.2f, Y: %.2f, Z: %.2f degrees/s" % (mpu4.gyro))
    print("Temperature4: %.2f C" % mpu4.temperature)
    print("----------------------------------------------------")
    time.sleep(3)
    print("MPU_5")
    print("Acceleration: X:%.2f, Y: %.2f, Z: %.2f m/s^2" % (mpu5.acceleration))
    print("Gyro X:%.2f, Y: %.2f, Z: %.2f degrees/s" % (mpu5.gyro))
    print("Temperature5: %.2f C" % mpu5.temperature)
    print("----------------------------------------------------")
    time.sleep(3)

    # reduce the number of messages sent, by only sendint out values if
    # the last value is different eg. temp
    # if temp != last_temp:
    # last_temp= temp
    # json file

    data = {}
    data["deviceID"] = "GreenSpaceTree9"
    data["board"] = "RPi4"
    data["gsm_mod"] = "BG96"
    data["location"] = "FairView"
    data["TreeType"] = "Red Oak"
    data["MPU_1_aX"] = ax1
    data["MPU_1_aY"] = ay1
    data["MPU_1_aZ"] = az1
    data["MPU_1_gX"] = gx1
    data["MPU_1_gY"] = gy1
    data["MPU_1_gZ"] = gz1
    data["MPU_1_temp"] = temp1
    data["MPU_2_aX"] = ax2
    data["MPU_2_aY"] = ay2
    data["MPU_2_aZ"] = az2
    data["MPU_2_gX"] = gx2
    data["MPU_2_gY"] = gy2
    data["MPU_2_gZ"] = gz2
    data["MPU_2_temp"] = temp2
    data["MPU_3_aX"] = ax3
    data["MPU_3_aY"] = ay3
    data["MPU_3_aZ"] = az3
    data["MPU_3_gX"] = gx3
    data["MPU_3_gY"] = gy3
    data["MPU_3_gZ"] = gz3
    data["MPU_3_temp"] = temp3
    data["MPU_4_aX"] = ax4
    data["MPU_4_aY"] = ay4
    data["MPU_4_aZ"] = az4
    data["MPU_4_gX"] = gx4
    data["MPU_4_gY"] = gy4
    data["MPU_4_gZ"] = gz4
    data["MPU_4_temp"] = temp4
    data["MPU_5_aX"] = ax5
    data["MPU_5_aY"] = ay5
    data["MPU_5_aZ"] = az5
    data["MPU_5_gX"] = gx5
    data["MPU_5_gY"] = gy5
    data["MPU_5_gZ"] = gz5
    data["MPU_5_temp"] = temp5
    json_body = json.dumps(data)
    print("Sending message: ", json_body)
    try:
        await device_client.send_message(json_body)

        # get device twin status and command
        twin = await device_client.get_twin()
        handle_twin(twin)

        time.sleep(10)

        await device_client.disconnect()
        print("Successfully Uploaded Soil Data to Azure")
    except:
        print("Could not Upload Soil Data to Azure")
        pass

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
                    "measurement": "tree_dynamics",
                    "tags": {
                        "location": "FiarView, Red Oak",
                        "Device ID": "GreenSpaceTree9",
                        "cellular connection": "LTE-M catM",
                        "modem": "BG96",
                    },
                    "fields": {
                        "MPU_1_aX": ax1,
                        "MPU_1_aY": ay1,
                        "MPU_1_aZ": az1,
                        "MPU_1_gX": gx1,
                        "MPU_1_gY": gy1,
                        "MPU_1_gZ": gz1,
                        "MPU_1_temp": temp1,
                        "MPU_2_aX": ax2,
                        "MPU_2_aY": ay2,
                        "MPU_2_aZ": az2,
                        "MPU_2_gX": gx2,
                        "MPU_2_gY": gy2,
                        "MPU_2_gZ": gz2,
                        "MPU_2_temp": temp2,
                        "MPU_3_aX": ax3,
                        "MPU_3_aY": ay3,
                        "MPU_3_aZ": az3,
                        "MPU_3_gX": gx3,
                        "MPU_3_gY": gy3,
                        "MPU_3_gZ": gz3,
                        "MPU_3_temp": temp3,
                        "MPU_4_aX": ax4,
                        "MPU_4_aY": ay4,
                        "MPU_4_aZ": az4,
                        "MPU_4_gX": gx4,
                        "MPU_4_gY": gy4,
                        "MPU_4_gZ": gz4,
                        "MPU_4_temp": temp4,
                        "MPU_5_aX": ax5,
                        "MPU_5_aY": ay5,
                        "MPU_5_aZ": az5,
                        "MPU_5_gX": gx5,
                        "MPU_5_gY": gy5,
                        "MPU_5_gZ": gz5,
                        "MPU_5_temp": temp5,
                    },
                    "time": time_utc,
                },
            )
    print("Successfully Uploaded Tree Dynamics Data to InfluxDB @ UDL")

    # influx configuration - edit these
    ifuser = "ibrahim"
    ifpass = "Dift1234"
    ifdb = "ground_sensors"
    ifhost = "localhost"
    ifport = 8086

    # format the data as a single measurement for influx
    body = [
        {
            "measurement": "Tree_dynamics",
            "tags": {
                "location": "FiarView, Red Oak",
                "Device ID": "GreenSpaceTree9",
                "cellular connection": "LTE-M catM",
                "modem": "BG96",
            },
            "fields": {
                "MPU_1_aX": ax1,
                "MPU_1_aY": ay1,
                "MPU_1_aZ": az1,
                "MPU_1_gX": gx1,
                "MPU_1_gY": gy1,
                "MPU_1_gZ": gz1,
                "MPU_1_temp": temp1,
                "MPU_2_aX": ax2,
                "MPU_2_aY": ay2,
                "MPU_2_aZ": az2,
                "MPU_2_gX": gx2,
                "MPU_2_gY": gy2,
                "MPU_2_gZ": gz2,
                "MPU_2_temp": temp2,
                "MPU_3_aX": ax3,
                "MPU_3_aY": ay3,
                "MPU_3_aZ": az3,
                "MPU_3_gX": gx3,
                "MPU_3_gY": gy3,
                "MPU_3_gZ": gz3,
                "MPU_3_temp": temp3,
                "MPU_4_aX": ax4,
                "MPU_4_aY": ay4,
                "MPU_4_aZ": az4,
                "MPU_4_gX": gx4,
                "MPU_4_gY": gy4,
                "MPU_4_gZ": gz4,
                "MPU_4_temp": temp4,
                "MPU_5_aX": ax5,
                "MPU_5_aY": ay5,
                "MPU_5_aZ": az5,
                "MPU_5_gX": gx5,
                "MPU_5_gY": gy5,
                "MPU_5_gZ": gz5,
                "MPU_5_temp": temp5,
            },
            "time": time_utc,
        }
    ]

    # connect to influx
    ifclient = iflux_localhost(ifhost, ifport, ifuser, ifpass, ifdb)

    # write the measurement
    ifclient.write_points(body)
    await asyncio.sleep(2)
    print("Successfully Uploaded Tree Dynamics Data to InfluxDB @ localhost")
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
            asyncio.run(do_stuff_periodically(60, main))

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
            continue
