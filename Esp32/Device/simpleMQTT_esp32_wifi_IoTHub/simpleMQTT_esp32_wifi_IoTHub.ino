///// WIFI

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
// 
// 5 of GY521-MPU 6050 at address, on TCA9548's channels 3-7
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
int16_t aX1, aY1, aZ1, gX1, gY1, gZ1, aX2, aY2, aZ2, gX2, gY2, gZ2,aX3, aY3, aZ3, gX3, gY3, gZ3, aX4, aY4, aZ4, gX4, gY4, gZ4, aX5, aY5, aZ5, gX5, gY5, gZ5;
//////////////////////////// Accelerometer ////////////////////

void TCA9548A(uint8_t bus)
{
  Wire.beginTransmission(0x70);
  Wire.write(1 << bus);
  Wire.endTransmission();
}

//=============================================================================

#define INTERVAL 15000 //10seconds intervals sending messages
#define MESSAGE_MAX_LEN 500
// Please input the SSID and password of WiFi
const char *ssid = "";
const char *password = "";

/// Primaty Connection String///
/*String containing Hostname, Device Id & Device Key in the format:                         */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"                */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessSignature=<device_sas_token>"    */
static const char *connectionString = "";
//const char *messageData = "{\"messageId\":%d, \"Temperature\":%f, \"Humidity\":%f}";
const char *DeviceID = "GreenSpaceTree1";
const char *messageData = "{\"DeviceID\":\"GreenSpaceTree1\",\"TreeType\":\"Cedar\" ,\"messageId\":%d, \"aX1\":%d, \"aY1\":%d, \"aZ1\":%d, \"gX1\":%d, \"gY1\":%d, \"gZ1\":%d, \"aX2\":%d, \"aY2\":%d, \"aZ2\":%d, \"gX2\":%d, \"gY2\":%d, \"gZ2\":%d, \"aX3\":%d, \"aY3\":%d, \"aZ3\":%d, \"gX3\":%d, \"gY3\":%d, \"gZ3\":%d, \"aX4\":%d, \"aY4\":%d, \"aZ4\":%d, \"gX4\":%d, \"gY4\":%d, \"gZ4\":%d, \"aX5\":%d, \"aY5\":%d, \"aZ5\":%d, \"gX5\":%d, \"gY5\":%d, \"gZ5\":%d}";
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

  // mpu6050 i2c at the multiplexer channel 1.
  TCA9548A(3);
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  delay(10);

  TCA9548A(4);
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  delay(10);

  TCA9548A(5);
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  delay(10);

  TCA9548A(6);
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  delay(10);

  TCA9548A(7);
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

      

      // get raw accelerometer data /////
      TCA9548A(3);
      Wire.beginTransmission(MPU);
      Wire.write(0x3B);
      Wire.endTransmission(false);
      Wire.requestFrom(MPU, 12, true);
      aX1 = Wire.read() << 8 | Wire.read();
      aY1 = Wire.read() << 8 | Wire.read();
      aZ1 = Wire.read() << 8 | Wire.read();
      gX1 = Wire.read() << 8 | Wire.read();
      gY1 = Wire.read() << 8 | Wire.read();
      gZ1 = Wire.read() << 8 | Wire.read();

   // Serial print acceleromter data
    Serial.print("MPU 1:");Serial.print(aX1);
    Serial.print(","); Serial.print(aY1);
    Serial.print(","); Serial.print(aZ1); 
    Serial.print(","); Serial.print(gX1);
    Serial.print(","); Serial.print(gY1);
    Serial.print(","); Serial.println(gZ1);
    
    delay(1000);

    // get raw accelerometer data /////
      TCA9548A(4);
      Wire.beginTransmission(MPU);
      Wire.write(0x3B);
      Wire.endTransmission(false);
      Wire.requestFrom(MPU, 12, true);
      aX2 = Wire.read() << 8 | Wire.read();
      aY2 = Wire.read() << 8 | Wire.read();
      aZ2 = Wire.read() << 8 | Wire.read();
      gX2 = Wire.read() << 8 | Wire.read();
      gY2 = Wire.read() << 8 | Wire.read();
      gZ2 = Wire.read() << 8 | Wire.read();

   // Serial print acceleromter data
    Serial.print("MPU 2:");Serial.print(aX2);
    Serial.print(","); Serial.print(aY2);
    Serial.print(","); Serial.print(aZ2); 
    Serial.print(","); Serial.print(gX2);
    Serial.print(","); Serial.print(gY2);
    Serial.print(","); Serial.println(gZ2);
    
    delay(1000);

    // get raw accelerometer data /////
      TCA9548A(5);
      Wire.beginTransmission(MPU);
      Wire.write(0x3B);
      Wire.endTransmission(false);
      Wire.requestFrom(MPU, 12, true);
      aX3 = Wire.read() << 8 | Wire.read();
      aY3 = Wire.read() << 8 | Wire.read();
      aZ3 = Wire.read() << 8 | Wire.read();
      gX3 = Wire.read() << 8 | Wire.read();
      gY3 = Wire.read() << 8 | Wire.read();
      gZ3 = Wire.read() << 8 | Wire.read();

   // Serial print acceleromter data
    Serial.print("MPU 3:");Serial.print(aX3);
    Serial.print(","); Serial.print(aY3);
    Serial.print(","); Serial.print(aZ3); 
    Serial.print(","); Serial.print(gX3);
    Serial.print(","); Serial.print(gY3);
    Serial.print(","); Serial.println(gZ3);
    
    delay(1000);

    // get raw accelerometer data /////
      TCA9548A(6);
      Wire.beginTransmission(MPU);
      Wire.write(0x3B);
      Wire.endTransmission(false);
      Wire.requestFrom(MPU, 12, true);
      aX4 = Wire.read() << 8 | Wire.read();
      aY4 = Wire.read() << 8 | Wire.read();
      aZ4 = Wire.read() << 8 | Wire.read();
      gX4 = Wire.read() << 8 | Wire.read();
      gY4 = Wire.read() << 8 | Wire.read();
      gZ4 = Wire.read() << 8 | Wire.read();

   // Serial print acceleromter data
    Serial.print("MPU 4:");Serial.print(aX4);
    Serial.print(","); Serial.print(aY4);
    Serial.print(","); Serial.print(aZ4); 
    Serial.print(","); Serial.print(gX4);
    Serial.print(","); Serial.print(gY4);
    Serial.print(","); Serial.println(gZ4);
    
    delay(1000);

    // get raw accelerometer data /////
      TCA9548A(7);
      Wire.beginTransmission(MPU);
      Wire.write(0x3B);
      Wire.endTransmission(false);
      Wire.requestFrom(MPU, 12, true);
      aX5 = Wire.read() << 8 | Wire.read();
      aY5 = Wire.read() << 8 | Wire.read();
      aZ5 = Wire.read() << 8 | Wire.read();
      gX5 = Wire.read() << 8 | Wire.read();
      gY5 = Wire.read() << 8 | Wire.read();
      gZ5 = Wire.read() << 8 | Wire.read();

   // Serial print acceleromter data
    Serial.print("MPU 5:");Serial.print(aX5);
    Serial.print(","); Serial.print(aY5);send_interval_ms = millis();
    Serial.print(","); Serial.print(aZ5); 
    Serial.print(","); Serial.print(gX5);
    Serial.print(","); Serial.print(gY5);
    Serial.print(","); Serial.println(gZ5);
    
    delay(2000);
      ////  raw accelerometer data //////


      char messagePayload[MESSAGE_MAX_LEN];

      snprintf(messagePayload, MESSAGE_MAX_LEN, messageData, messageCount++, aX1, aY1, aZ1, gX1, gY1, gZ1, aX2, aY2, aZ2, gX2, gY2, gZ2, aX3, aY3, aZ3, gX3, gY3, gZ3, aX4, aY4, aZ4, gX4, gY4, gZ4, aX5, aY5, aZ5, gX5, gY5, gZ5);
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
