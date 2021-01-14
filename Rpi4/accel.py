# requirements:
# smbus
# mpu6050-raspberrypi
# azure-iot-device

# connecting the accelerometer GY-521 (MPU6050) to RPi
# i2cdetect -y 1 to double check if i2c on the pi is confgirured
# all dep. libraries are in requirements.txt
# pip3 install -r requirements.txt
# https://github.com/Arijit1080/mpu6050-with-Raspberry-Pi/blob/master/mpu_6050.py

import smbus
from mpu6050 import mpu6050
import time
import asyncio
from azure.iot.device.aio import IoTHubDeviceClient
import json
import RPi.GPIO as GPIO

i2c_address = 0x68
mpu = mpu6050(i2c_address)

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

    conn_str = "HostName=SensorDataRpi4.azure-devices.net;DeviceId=rpi4Tree;SharedAccessKey=cBq5yS4ygdwP4OirmjF49ajrQwjpcCMEmbysIobvk60="
    device_client = IoTHubDeviceClient.create_from_connection_string(conn_str)
    await device_client.connect()

    while True:
        print("Temp : " + str(mpu.get_temp()))
        temp = "{0:0.1f}".format(mpu.get_temp())
        print()
        # last_temp= "{0:0.1f}".format(mpu.get_temp())

        accel_data = mpu.get_accel_data()
        print("Acc X : " + str(accel_data["x"]))
        print("Acc Y : " + str(accel_data["y"]))
        print("Acc Z : " + str(accel_data["z"]))
        print()

        gyro_data = mpu.get_gyro_data()
        print("Gyro X : " + str(gyro_data["x"]))
        print("Gyro Y : " + str(gyro_data["y"]))
        print("Gyro Z : " + str(gyro_data["z"]))
        print()
        print("-------------------------------")

        # reduce the number of messages sent, by only sendint out values if
        # the last value is different eg. temp
        # if temp != last_temp:
        # last_temp= temp
        # json file

        data = {}
        data["deviceID"] = "RaspberryPi4_Tree"
        data["Acc_X"] = "{0:0.1f}".format(accel_data["x"])
        data["Acc_Y"] = "{0:0.1f}".format(accel_data["y"])
        data["Acc_Z"] = "{0:0.1f}".format(accel_data["z"])
        data["Gyro_X"] = "{0:0.1f}".format(gyro_data["x"])
        data["Gyro_Y"] = "{0:0.1f}".format(gyro_data["y"])
        data["Gyro_Z"] = "{0:0.1f}".format(gyro_data["z"])
        data["Temprature"] = temp
        json_body = json.dumps(data)
        print("Sending message: ", json_body)
        await device_client.send_message(json_body)

        # get device twin status and command
        twin = await device_client.get_twin()
        handle_twin(twin)

        time.sleep(60)

    await device_client.disconnect()


if __name__ == "__main__":
    asyncio.run(main())
