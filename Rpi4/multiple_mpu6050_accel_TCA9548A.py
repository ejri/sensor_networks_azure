# Use with other I2C sensors would be similar.
# import adafruit_lsm9ds1
import adafruit_tca9548a
import board
import busio
import numpy as np
import smbus
import time
import adafruit_mpu6050


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

while True:

    print("MPU 1")
    print("Acceleration: X:%.2f, Y: %.2f, Z: %.2f m/s^2" % (mpu1.acceleration))
    print("Gyro X:%.2f, Y: %.2f, Z: %.2f degrees/s" % (mpu1.gyro))
    print("Temperature: %.2f C" % mpu1.temperature)
    print("----------------------------------------------------")
    time.sleep(3)
    print("MPU 2")
    print("Acceleration: X:%.2f, Y: %.2f, Z: %.2f m/s^2" % (mpu2.acceleration))
    print("Gyro X:%.2f, Y: %.2f, Z: %.2f degrees/s" % (mpu2.gyro))
    print("Temperature: %.2f C" % mpu2.temperature)
    print("----------------------------------------------------")
    time.sleep(3)
    print("MPU 3")
    print("Acceleration: X:%.2f, Y: %.2f, Z: %.2f m/s^2" % (mpu3.acceleration))
    print("Gyro X:%.2f, Y: %.2f, Z: %.2f degrees/s" % (mpu3.gyro))
    print("Temperature: %.2f C" % mpu3.temperature)
    print("----------------------------------------------------")
    time.sleep(3)
    print("MPU 4")
    print("Acceleration: X:%.2f, Y: %.2f, Z: %.2f m/s^2" % (mpu4.acceleration))
    print("Gyro X:%.2f, Y: %.2f, Z: %.2f degrees/s" % (mpu4.gyro))
    print("Temperature: %.2f C" % mpu4.temperature)
    print("----------------------------------------------------")
    time.sleep(3)
    print("MPU 5")
    print("Acceleration: X:%.2f, Y: %.2f, Z: %.2f m/s^2" % (mpu5.acceleration))
    print("Gyro X:%.2f, Y: %.2f, Z: %.2f degrees/s" % (mpu5.gyro))
    print("Temperature: %.2f C" % mpu5.temperature)
    print("----------------------------------------------------")
    time.sleep(3)
    print("MPU 5")
    print(ax1, ay1, a1z, gx1, gy1, gz1, temp1)
    print("----------------------------------------------------")
    time.sleep(1)