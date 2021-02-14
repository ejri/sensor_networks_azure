
# Use with other I2C sensors would be similar.
import adafruit_lsm9ds1
import adafruit_tca9548a
import asyncio
import board
import busio
import json
import RPi.GPIO as GPIO
import smbus
import time
from mpu6050 import mpu6050
from azure.iot.device.aio import IoTHubDeviceClient


####### LSM9DS1 IMU on the TCA9548A multiplexer #####
# Create I2C bus as normal
i2c = busio.I2C(board.SCL, board.SDA)
# Create the TCA9548A object and give it the I2C bus
tca = adafruit_tca9548a.TCA9548A(i2c)
# For each sensor, create it using the TCA9548A channel instead of the I2C object
accel_lsm9ds1 = adafruit_lsm9ds1.LSM9DS1_I2C(tca[0])
####### LSM9DS1 IMU on the TCA9548A multiplexer #####

####### MPU6050 IMU on the TCA9548A multiplexer, redefine the mpu6050
# to the address of the multiplexer at 0x70 #####
i2c_address = 0x70
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

    conn_str = 
    device_client = IoTHubDeviceClient.create_from_connection_string(conn_str)
    await device_client.connect()

    while True:
        print("Temp : " + str(mpu.get_temp()))
    temp = "{0:0.2f}".format(mpu.get_temp())
    print()
    # last_temp= "{0:0.1f}".format(mpu.get_temp())
    accel_data = mpu.get_accel_data()
    print("Acc X : " + str("{0:0.2f}".format(accel_data["x"])))
    print("Acc Y : " + str("{0:0.2f}".format(accel_data["y"])))
    print("Acc Z : " + str("{0:0.2f}".format(accel_data["z"])))
    print()

    gyro_data = mpu.get_gyro_data()
    print("Gyro X : " + str("{0:0.2f}".format(gyro_data["x"])))
    print("Gyro Y : " + str("{0:0.2f}".format(gyro_data["y"])))
    print("Gyro Z : " + str("{0:0.2f}".format(gyro_data["z"])))
    print()
    print("-------------------------------")
    time.sleep(3)
    accel_x, accel_y, accel_z = accel_lsm9ds1.acceleration
    mag_x, mag_y, mag_z = accel_lsm9ds1.magnetic
    gyro_x, gyro_y, gyro_z = accel_lsm9ds1.gyro
    temp = accel_lsm9ds1.temperature

    print(
        "Acceleration (m/s^2): ({0:0.3f},{1:0.3f},{2:0.3f})".format(
            accel_x, accel_y, accel_z
        )
    )
    print(
        "Magnetometer (gauss): ({0:0.3f},{1:0.3f},{2:0.3f})".format(mag_x, mag_y, mag_z)
    )
    print(
        "Gyroscope (degrees/sec): ({0:0.3f},{1:0.3f},{2:0.3f})".format(
            gyro_x, gyro_y, gyro_z
        )
    )
    print("Temperature: {0:0.3f}C".format(temp))
    # Delay for a second.
    time.sleep(3)

        # reduce the number of messages sent, by only sendint out values if
        # the last value is different eg. temp
        # if temp != last_temp:
        # last_temp= temp
        # json file

        data = {}
        data["deviceID"] = "RaspberryPi4_Tree"
        data["Acc_X"] = "{0:0.2f}".format(accel_data["x"])
        data["Acc_Y"] = "{0:0.2f}".format(accel_data["y"])
        data["Acc_Z"] = "{0:0.2f}".format(accel_data["z"])
        data["Gyro_X"] = "{0:0.2f}".format(gyro_data["x"])
        data["Gyro_Y"] = "{0:0.2f}".format(gyro_data["y"])
        data["Gyro_Z"] = "{0:0.2f}".format(gyro_data["z"])
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



