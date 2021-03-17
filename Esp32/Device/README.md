# description of files in this folder

device.ino --> LilyGo esp32 sim7000G connected to 5 MPU6050. Pitch, roll, yaw from the 5 sensors are uploaded to a SQL database in Azure through IoT Hub. Connection is 5G (LTE Cat-M). 

SimpleMQTT_esp32_azure_iothub --> backup version of device.ino

simpleMQTT_esp32_wifi_IoTHub --> esp32 connected to 5 MPU6050. Pitch, roll, yaw from the 5 sensors are uploaded to a SQL database in Azure through IoT Hub. Connection is wifi.  

multiple_mpu6050_esp32 --> 5 MPU6050 connected to an arduino based board. Pitch, roll, yaw data are calculated, not raw data. 

sim7000g_thingsboard --> example from LilyGo's github repo. esp32 sim7000G sending battery, solar vaues to Thingsboard. 

test_individual_soil --> esp32 connected to a soil sensor. Esp32 intermittantly reads and saves sensor values in memory. 100 data points are  aggregated over 24hrs. Data is uploaded once a day.  


Arduino_NetworkTest_scan_LTE_networs --> general scan for available LTE, GSM, GPRS, networks. 

esp32_multiple_accelerometers_MPU6050_LSM9DS1 --> connecting multiple IMUs (MPU6050 and LSM9DS1) into one board. Raw data. 

esp32_multiple_MPU6050_raw_data---> connecting multiple IMUs (MPU6050) into one board. Raw data.


mpu6050_demo --> 3D Real-Time Visualization of the MPU6050 (Processing software -- https://processing.org/)

## 3D Real-Time Visualization of the MPU6050:
\
![image output](Esp32/Device/3D_mpu6050.jpg)



