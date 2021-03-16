// This code: connecting an accelerometer to esp32. The raw accelerometer data is sent to IoT Hub.
// IoT Hub connection, and device provisioning, etc. :

// SimpleMQTT_esp32_azure_iothub.ino
// downloads/ubc_5G/sensor_networks_azure/

// IoT Hub has to be Standard Tier, Basic Tier creates a lot of issues

// Azure ESP32 IoT DevKit Get Started
// https://docs.microsoft.com/en-us/samples/azure-samples/esp32-iot-devkit-get-started/sample/

/**
   A simple Azure IoT example for sending telemetry to Iot Hub.

  original code from:
  https://github.com/VSChina/ESP32_AzureIoT_Arduino
  se
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



//////////////// LTE-M connection ////////////////
#define TINY_GSM_MODEM_SIM7000

#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb
#define DUMP_AT_COMMANDS
#include "TinyGsmClient.h"

#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 60       /* ESP32 should sleep more seconds  (note SIM7000 needs ~20sec to turn off if sleep is activated) */
RTC_DATA_ATTR int bootCount = 0;


HardwareSerial serialGsm(1);
#define SerialAT serialGsm
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

// rogers
const char apn[] = "ltemobile.apn";
const char nbiot_apn[] = "iot";

// 1nce
// const char apn[] = "iot.1nce.net";
// const char nbiot_apn[] = "iot.1nce.net";

#define isNBIOT false

const char user[] = "";
const char pass[] = "";

// TTGO T-SIM pin definitions
#define MODEM_RST 5
#define MODEM_PWKEY 4
#define MODEM_DTR 25

#define MODEM_TX 26
#define MODEM_RX 27

//#define I2C_SDA 21
//#define I2C_SCL 22

#define reading_samles 100

#ifdef DUMP_AT_COMMANDS
#include "StreamDebugger.h"
StreamDebugger debugger(serialGsm, Serial);
TinyGsm modem(debugger);
#else
TinyGsm modem(serialGsm);
#endif


// Initialize GSM client
TinyGsmClient client(modem);


// Set to true, if modem is connected
bool modemConnected = false;

void shutdown();
void wait_till_ready();
void modem_off();

void print_wakeup_reason()
{
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0:
      Serial.println("Wakeup caused by external signal using RTC_IO");
      break;
    case ESP_SLEEP_WAKEUP_EXT1:
      Serial.println("Wakeup caused by external signal using RTC_CNTL");
      break;
    case ESP_SLEEP_WAKEUP_TIMER:
      Serial.println("Wakeup caused by timer");
      break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
      Serial.println("Wakeup caused by touchpad");
      break;
    case ESP_SLEEP_WAKEUP_ULP:
      Serial.println("Wakeup caused by ULP program");
      break;
    default:
      Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
      break;
  }
}

void modem_reset()
{
  Serial.println("Modem hardware reset");
  pinMode(MODEM_RST, OUTPUT);
  digitalWrite(MODEM_RST, LOW);
  delay(260); //Treset 252ms
  digitalWrite(MODEM_RST, HIGH);
  delay(4000); //Modem takes longer to get ready and reply after this kind of reset vs power on

  //modem.factoryDefault();
  //modem.restart(); //this results in +CGREG: 0,0
}

void modem_on()
{
  // Set-up modem  power pin
  pinMode(MODEM_PWKEY, OUTPUT);
  digitalWrite(MODEM_PWKEY, HIGH);
  delay(10);
  digitalWrite(MODEM_PWKEY, LOW);
  delay(1010); //Ton 1sec
  digitalWrite(MODEM_PWKEY, HIGH);

  //wait_till_ready();
  Serial.println("Waiting till modem ready...");
  delay(4510); //Ton uart 4.5sec but seems to need ~7sec after hard (button) reset
  //On soft-reset serial replies immediately.

}

void modem_off()
{
  //if you turn modem off while activating the fancy sleep modes it takes ~20sec, else its immediate
  Serial.println("Going to sleep now with modem turned off");
  //modem.gprsDisconnect();
  //modem.radioOff();
  modem.sleepEnable(false); // required in case sleep was activated and will apply after reboot
  modem.poweroff();
}

// fancy low power mode - while connected
void modem_sleep() // will have an effect after reboot and will replace normal power down
{
  Serial.println("Going to sleep now with modem in power save mode");
  // needs reboot to activa and takes ~20sec to sleep
  //modem.PSM_mode();    //if network supports will enter a low power sleep PCM (9uA)
  //modem.eDRX_mode14(); // https://github.com/botletics/SIM7000-LTE-Shield/wiki/Current-Consumption#e-drx-mode
  modem.sleepEnable(); //will sleep (1.7mA), needs DTR or PWRKEY to wake
  pinMode(MODEM_DTR, OUTPUT);
  digitalWrite(MODEM_DTR, HIGH);
}

void modem_wake()
{
  Serial.println("Wake up modem from sleep");
  // DTR low to wake serial
  pinMode(MODEM_DTR, OUTPUT);
  digitalWrite(MODEM_DTR, LOW);
  delay(50);
  //wait_till_ready();
}

void shutdown()
{

  //modem_sleep();
  modem_off();

  delay(1000);
  Serial.flush();
  esp_deep_sleep_start();
  Serial.println("Going to sleep now");
  delay(1000);
  Serial.flush();
  esp_deep_sleep_start();
}

void wait_till_ready() // NOT WORKING - Attempt to minimize waiting time
{

  for (int8_t i = 0; i < 100; i++) //timeout 100 x 100ms = 10sec
  {
    if (modem.testAT())
    {
      //Serial.println("Wait time:%F sec\n", i/10));
      Serial.printf("Wait time: %d\n", i);
      break;
    }
    delay(100);
  }
}

//////////////// LTE-M connection ////////////////


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
  
  //// start count/// --> moved to after modemOn() function
  //send_interval_ms = millis();

  delay(10);
  Serial.println(F("Started"));
  Serial.println("ESP32 Device");

  /*
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
  */


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


  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  /*
    First we configure the wake up source
    We set our ESP32 to wake up every 5 seconds
  */
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
                 " Seconds");

  // if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER)
  // {
  //   modem_wake();
  // }
  // else
  {
    modem_on();
    //modem_wake();
    //modem_reset();
  }


  // Set GSM module baud rate and UART pins
  SerialAT.begin(9600, SERIAL_8N1, MODEM_TX, MODEM_RX); //reversing them
  String modemInfo = modem.getModemInfo();
  Serial.print(F("Modem: "));
  Serial.println(modemInfo);

  if (!modemConnected)
  {

    //SIM7000

    if (isNBIOT)
    {
      Serial.println("configuring NBIoT mode");
      modem.setPreferredMode(38);
      //modem.setPreferredLTEMode(2);
      //modem.setOperatingBand(20); // Required for cosmote Greece

      Serial.print(F("Waiting for network..."));
      if (!modem.waitForNetwork(60000L))
      {
        Serial.println(" fail");
        modem_reset();
        shutdown();
      }
      Serial.println("Connected to LTE Mode Only (channel--> 38).... OK");

      Serial.print("Signal quality:");
      Serial.println(modem.getSignalQuality());

      Serial.print(F("Connecting to apn:"));
      Serial.println(nbiot_apn);
      if (!modem.gprsConnect(nbiot_apn, user, pass))
      {
        Serial.println(" failed");
        modem_reset();
        shutdown();
      }

      modemConnected = true;
      Serial.println("Modem Connected (channel--> 38). OK");
    }
    else
    {
      Serial.println("configuring LTE, Cat-M mode"); // AUTO or GSM ONLY

      //2 Auto
      // 13 GSM only
      // 38 LTE only
      // 51 GSM and LTE only
      modem.setNetworkMode(38);
      //modem.setOperatingBand("CAT-M",12)
      delay(500);

      /*
          1 CAT-M
          2 NB-Iot
          3 CAT-M and NB-IoT
        * * */

      modem.setPreferredMode(1);
      delay(500);

      Serial.print(F("Waiting for network..."));
      if (!modem.waitForNetwork(60000L))
      {
        Serial.println(" fail");
        modem_reset();
        shutdown();
      }
      Serial.println(" OK");

      Serial.print("Signal quality:");
      Serial.println(modem.getSignalQuality());

      Serial.print(F("Connecting to Rogers APN at LTE Mode Only (channel--> 38): "));
      Serial.print(apn);
      if (!modem.gprsConnect(apn, user, pass))
      {
        Serial.println(" failed");
        modem_reset();
        shutdown();
      }

      modemConnected = true;
      Serial.println("Modem Connected to Rogers' LTE (channel--> 38) CAT-M (preferred network). TLE CAT-M OK");
    }
  }

  Serial.println(" > IoT Hub");
  if (!Esp32MQTTClient_Init((const uint8_t *)connectionString, true))
  {
    hasIoTHub = false;
    Serial.println("Initializing IoT hub failed.");
    modem_reset();
    shutdown();
    return;
  }
  hasIoTHub = true;
  Esp32MQTTClient_SetSendConfirmationCallback(SendConfirmationCallback);
  Esp32MQTTClient_SetMessageCallback(MessageCallback);
  Esp32MQTTClient_SetDeviceTwinCallback(DeviceTwinCallback);
  Esp32MQTTClient_SetDeviceMethodCallback(DeviceMethodCallback);
  Serial.println("Start sending events.");
  randomSeed(analogRead(0));
  send_interval_ms = millis();   // moved to after adding the accelerometer


  if (modemConnected && hasIoTHub)
  {
    if (messageSending &&
        (int)(millis() - send_interval_ms) >= INTERVAL)
    {

      read_MPU_1();
      read_MPU_2();
      read_MPU_3();
      read_MPU_4();
      read_MPU_5();
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


  //delay(1000); // required - else will shutdown too early and miss last value
  shutdown();


}

void loop()
{

}




void read_MPU_1()
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
}

void read_MPU_2()
{
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
  }

 void read_MPU_3()
{
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
}

void read_MPU_4()
{
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
}

void read_MPU_5()
{
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
    Serial.print(","); Serial.print(aY5);
    Serial.print(","); Serial.print(aZ5); 
    Serial.print(","); Serial.print(gX5);
    Serial.print(","); Serial.print(gY5);
    Serial.print(","); Serial.println(gZ5);
    
    delay(2000);
}
