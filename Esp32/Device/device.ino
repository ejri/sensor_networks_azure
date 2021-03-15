
// This code: connecting an accelerometer to esp32. The raw accelerometer data is sent to IoT Hub.
// IoT Hub connection, and device provisioning, etc. :

// SimpleMQTT_esp32_azure_iothub.ino
// downloads/ubc_5G/sensor_networks_azure/ 

// Azure ESP32 IoT DevKit Get Started
// https://docs.microsoft.com/en-us/samples/azure-samples/esp32-iot-devkit-get-started/sample/

/**
 * A simple Azure IoT example for sending telemetry to Iot Hub.
 
 original code from:
 https://github.com/VSChina/ESP32_AzureIoT_Arduino

IoT Hub has to be Standard Tier, Basic Tier creates a lot of issues
 
 */

// TCA9548A I2C Switch between:
// adafruit LSM9DS1 at address, on TCA9548's channel 0
// GY521-MPU 6050 at address, on TCA9548's channel 1
// TCA9548 module I2C address:  0x70
//

//======================================================
//I2C Mutiplexing using TCA9548A at I2C address 0x70
//======================================================

// GY-521 / mpu6050 on esp32
// connections: 5v, GND, SDA, SCL, on sensor to 5v, GND, SDA, SCL on esp32 respectively

#include <WiFi.h>
#include "Esp32MQTTClient.h"

//////////////////////////// Accelerometer ////////////////////
// Include Wire Library for I2C
#include <Wire.h>
/*
const int MPU=0x68; // i2c address of the mpu6050, if AD0 pin on accelerometer module is set to HIGH
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;  // for acceleromter, temperature, gyroscope 
*/

//Variables for Gyroscope

const int MPU = 0x68;
int16_t aX, aY, aZ, gX, gY, gZ;
//////////////////////////// Accelerometer ////////////////////

#include <Adafruit_LSM9DS1.h>
#include <Adafruit_Sensor.h> // not used in this demo but required!

//////////////////////////// LSM9DS1 ////////////////////
Adafruit_LSM9DS1 lsm_1 = Adafruit_LSM9DS1(1);

#define LSM9DS1_SCK A5
#define LSM9DS1_MISO 12
#define LSM9DS1_MOSI A4
#define LSM9DS1_XGCS 6
#define LSM9DS1_MCS 5

void TCA9548A(uint8_t bus)
{
  Wire.beginTransmission(0x70);
  Wire.write(1 << bus);
  Wire.endTransmission();
}

void setupSensor()
{
  // 1.) Set the accelerometer range
  lsm_1.setupAccel(lsm_1.LSM9DS1_ACCELRANGE_2G);
  //lsm_1.setupAccel(lsm_1.LSM9DS1_ACCELRANGE_4G);
  //lsm_1.setupAccel(lsm_1.LSM9DS1_ACCELRANGE_8G);
  //lsm_1.setupAccel(lsm_1.LSM9DS1_ACCELRANGE_16G);

  // 2.) Set the magnetometer sensitivity
  lsm_1.setupMag(lsm_1.LSM9DS1_MAGGAIN_4GAUSS);
  //lsm_1.setupMag(lsm_1.LSM9DS1_MAGGAIN_8GAUSS);
  //lsm_1.setupMag(lsm_1.LSM9DS1_MAGGAIN_12GAUSS);
  //lsm_1.setupMag(lsm_1.LSM9DS1_MAGGAIN_16GAUSS);

  // 3.) Setup the gyroscope
  lsm_1.setupGyro(lsm_1.LSM9DS1_GYROSCALE_245DPS);
  //lsm_1.setupGyro(lsm_1.LSM9DS1_GYROSCALE_500DPS);
  //lsm_1.setupGyro(lsm_1.LSM9DS1_GYROSCALE_2000DPS)a
}

//=============================================================================

#define INTERVAL 15000 //10seconds intervals sending messages
#define MESSAGE_MAX_LEN 256
// Please input the SSID and password of WiFi
const char *ssid = "Liverpool FC";
const char *password = "6042140214";

/// Primaty Connection String///
/*String containing Hostname, Device Id & Device Key in the format:                         */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"                */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessSignature=<device_sas_token>"    */
static const char *connectionString = "HostName=GreenSpaces.azure-devices.net;DeviceId=GreenSpaceTree1;SharedAccessKey=Y4IN6au/gQSVpObThi7uJ1m+y4b823YgadkXYbK6vx0=";
//const char *messageData = "{\"messageId\":%d, \"Temperature\":%f, \"Humidity\":%f}";
const char *DeviceID = "GreenSpaceTree1";
const char *messageData = "{\"DeviceID\":\"GreenSpaceTree1\" ,\"messageId\":%d, \"aX\":%d, \"aY\":%d, \"aZ\":%d, \"gX\":%d, \"gY\":%d, \"gZ\":%d}";
static bool hasIoTHub = false;
static bool hasWifi = false;
int messageCount = 1;
static bool messageSending = true;
static uint64_t send_interval_ms;

static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result)
{
  if (result == IOTHUB_CLIENT_CONFIRMATION_OK)
  {
    Serial.println("Send Confirmation Callback finished.");
  }
}

static void MessageCallback(const char *payLoad, int size)
{
  Serial.println("Message callback:");
  Serial.println(payLoad);
}

static void DeviceTwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payLoad, int size)
{
  char *temp = (char *)malloc(size + 1);
  if (temp == NULL)
  {
    return;
  }
  memcpy(temp, payLoad, size);
  temp[size] = '\0';
  // Display Twin message.
  Serial.println(temp);
  free(temp);
}

static int DeviceMethodCallback(const char *methodName, const unsigned char *payload, int size, unsigned char **response, int *response_size)
{
  LogInfo("Try to invoke method %s", methodName);
  const char *responseMessage = "\"Successfully invoke device method\"";
  int result = 200;

  if (strcmp(methodName, "start") == 0)
  {
    LogInfo("Start sending temperature and humidity data");
    messageSending = true;
  }
  else if (strcmp(methodName, "stop") == 0)
  {
    LogInfo("Stop sending temperature and humidity data");
    messageSending = false;
  }
  else
  {
    LogInfo("No method %s found", methodName);
    responseMessage = "\"No method found\"";
    result = 404;
  }

  *response_size = strlen(responseMessage) + 1;
  *response = (unsigned char *)strdup(responseMessage);

  return result;
}

void setup()
{
  Serial.begin(115200);
  Serial.println("ESP32 Device");
  Serial.println("Initializing...");
  Serial.println(" > WiFi");
  Serial.println("Starting connecting WiFi.");

  delay(10);
  WiFi.mode(WIFI_AP);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    hasWifi = false;
  }
  hasWifi = true;

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(" > IoT Hub");
  if (!Esp32MQTTClient_Init((const uint8_t *)connectionString, true))
  {
    hasIoTHub = false;
    Serial.println("Initializing IoT hub failed.");
    return;
  }
  hasIoTHub = true;
  Esp32MQTTClient_SetSendConfirmationCallback(SendConfirmationCallback);
  Esp32MQTTClient_SetMessageCallback(MessageCallback);
  Esp32MQTTClient_SetDeviceTwinCallback(DeviceTwinCallback);
  Esp32MQTTClient_SetDeviceMethodCallback(DeviceMethodCallback);
  Serial.println("Start sending events.");
  randomSeed(analogRead(0));
  //send_interval_ms = millis();   // moved to after adding the accelerometer

  Wire.begin();

  /* Initialise the 1st sensor */
  Serial.println("LSM9DS1 data read demo");

  // lsm9ds1 i2c at the multiplexer channel 1. check if sensor is connected
  TCA9548A(0);
  if (!lsm_1.begin())
  {
    Serial.println("Oops ... unable to initialize the LSM9DS1. Check your wiring!");
    while (1)
      ;
  }
  Serial.println("Found LSM9DS1 9DOF");

  // sensor connected. initiate sensor
  TCA9548A(0);
  setupSensor();

  // mpu6050 i2c at the multiplexer channel 1.
  TCA9548A(1);
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  delay(10);

  //// start count///
  send_interval_ms = millis();
}

void loop()
{
  if (hasWifi && hasIoTHub)
  {
    if (messageSending &&
        (int)(millis() - send_interval_ms) >= INTERVAL)
    {

      TCA9548A(0);
      lsm_1.read(); /* ask it to read in the data */

      /* Get a new sensor event */
      TCA9548A(0);
      sensors_event_t a, m, g, temp;
      TCA9548A(0);
      lsm_1.getEvent(&a, &m, &g, &temp);

      Serial.print("Accel X: ");
      Serial.print(a.acceleration.x);
      Serial.print(" m/s^2");
      Serial.print("\tY: ");
      Serial.print(a.acceleration.y);
      Serial.print(" m/s^2 ");
      Serial.print("\tZ: ");
      Serial.print(a.acceleration.z);
      Serial.println(" m/s^2 ");

      Serial.print("Mag X: ");
      Serial.print(m.magnetic.x);
      Serial.print(" uT");
      Serial.print("\tY: ");
      Serial.print(m.magnetic.y);
      Serial.print(" uT");
      Serial.print("\tZ: ");
      Serial.print(m.magnetic.z);
      Serial.println(" uT");

      Serial.print("Gyro X: ");
      Serial.print(g.gyro.x);
      Serial.print(" rad/s");
      Serial.print("\tY: ");
      Serial.print(g.gyro.y);
      Serial.print(" rad/s");
      Serial.print("\tZ: ");
      Serial.print(g.gyro.z);
      Serial.println(" rad/s");

      Serial.println();
      delay(10);

      // get raw accelerometer data /////
      TCA9548A(1);
      Wire.beginTransmission(MPU);
      Wire.write(0x3B);
      Wire.endTransmission(false);
      Wire.requestFrom(MPU, 12, true);
      aX = Wire.read() << 8 | Wire.read();
      aY = Wire.read() << 8 | Wire.read();
      aZ = Wire.read() << 8 | Wire.read();
      gX = Wire.read() << 8 | Wire.read();
      gY = Wire.read() << 8 | Wire.read();
      gZ = Wire.read() << 8 | Wire.read();

      // simulated acceleromter data
      /*
    aX=(float)random(0, 500) / 10;    
    aY=(float)random(0, 500) / 10;  
    aZ=(float)random(0, 500) / 10;  
    gX=(float)random(0, 500) / 10;  
    gY=(float)random(0, 500) / 10;  
    gZ=(float)random(0, 500) / 10;
    */

      // Serial print acceleromter data
      Serial.print(aX);
      Serial.print(",");
      Serial.print(aY);
      Serial.print(",");
      Serial.print(aZ);
      Serial.print(",");
      Serial.print(gX);
      Serial.print(",");
      Serial.print(gY);
      Serial.print(",");
      Serial.println(gZ);

      delay(2000);
      ////  raw accelerometer data //////

      // generate random temperature, humidity data
      //float temperature = (float)random(0, 500) / 10;
      //float humidity = (float)random(0, 1000) / 10;

      char messagePayload[MESSAGE_MAX_LEN];

      snprintf(messagePayload, MESSAGE_MAX_LEN, messageData, messageCount++, aX, aY, aZ, gX, gY, gZ);
      Serial.println(messagePayload);
      EVENT_INSTANCE *message = Esp32MQTTClient_Event_Generate(messagePayload, MESSAGE);
      Esp32MQTTClient_SendEventInstance(message);
      send_interval_ms = millis();
    }
    else
    {
      Esp32MQTTClient_Check();
    }
  }
  delay(5000);
}