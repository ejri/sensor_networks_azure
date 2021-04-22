//ejri ->  GPL-3.0 License

///// WIFI (esp32 wover)

// This code: connecting an accelerometer to esp32. The yaw,ptich,roll IMU data is sent to InfluxDB.

/**
 * A simple Azure IoT example for sending telemetry to Iot Hub.
 
 credits:
 https://github.com/tobiasschuerg/InfluxDB-Client-for-Arduino
 
 https://gist.github.com/debsahu/77fb50a36c0d48a39db3937885990f0c
 
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
//#include <esp_wpa2.h>   WPA2 wifi connection
//#include <esp_wifi.h>   WPA2 wifi connection
#include <time.h>
#include <InfluxDbClient.h> // https://github.com/tobiasschuerg/InfluxDB-Client-for-Arduino

//////////////////////////// Accelerometer ////////////////////
#include <Wire.h>
// Amazing MPU6050 library by rfetick.
#include <MPU6050_light.h>
MPU6050 mpu(Wire);

const int MPU = 0x68;
int16_t roll1, pitch1, yaw1, roll2, pitch2, yaw2, roll3, pitch3, yaw3, roll4, pitch4, yaw4, roll5, pitch5, yaw5;
//////////////////////////// Accelerometer ////////////////////

void TCA9548A(uint8_t bus)
{
    Wire.beginTransmission(0x70);
    Wire.write(1 << bus);
    Wire.endTransmission();
}
//////////////////////////// Accelerometer ////////////////////

////// tags in influxDB///
#define HOSTNAME "ESP32 IMU data"
#define LOCATION "FairView- Cedar"
////// tags in influxDB///

//// wifi credentials
const char *ssid = "";
const char *password = "";
static bool hasWifi = false;

//// wifi credentials

/*    WPA2 ssid and password
//Check instructions at https://github.com/debsahu/Esp32_EduWiFi to connect to WPA2-Enterprise WiFi
//Register your device using MAC address in MSetup
const char *ssid = "eduroam";             // Eduroam // MWireless seized all 2.4GHz SSID on 2/25/2020
#define EAP_IDENTITY "username@umich.edu" //enter full umich email address
#define EAP_PASSWORD "umich_password"     //your umich password

static const char incommon_ca[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
-----END CERTIFICATE-----
)EOF";
// Obtain the latest from :
// https://documentation.its.umich.edu/content/wifi-manually-configuring-wpa2-enterprise-other-wifi-enabled-devices-unsupported-devices
// Use the "Intermediate CA" file: http://www.itcom.itd.umich.edu/downloads/wifi/incommon_ras_server_ca.cer ^^^^^^^^^^^^^^^^^^^^^^^^^^^
// NO need for "Root CA: UserTrust RSA Cerification Authority" file
*/

// InfluxDB  server url. Don't use localhost, always server name or ip address.
// E.g. http://192.168.1.48:8086 (In InfluxDB 2 UI -> Load Data -> Client Libraries),
#define INFLUXDB_URL "http://10.0.0.93:8086"
// InfluxDB 2 server or cloud API authentication token (Use: InfluxDB UI -> Load Data -> Tokens -> <select token>)
#define INFLUXDB_TOKEN ""
// InfluxDB 2 organization id (Use: InfluxDB UI -> Settings -> Profile -> <name under tile> )
#define INFLUXDB_ORG "UBC"
// InfluxDB 2 bucket name (Use: InfluxDB UI -> Load Data -> Buckets)
#define INFLUXDB_BUCKET "air_quality_custom"

// InfluxDB client instance
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN);

// Data point
Point sensor("tree_dynamics");

const char *ntpServer = "pool.ntp.org";
const char *timezoneEST = "EST5EDT,M3.2.0/2,M11.1.0";

uint8_t counter = 0;
unsigned long previousMillisWiFi = 0;
char timeStringBuff[50];
//uint8_t masterCustomMac[] = {0x24, 0x0A, 0xC4, 0x9A, 0xA5, 0xA1}; // 24:0a:c4:9a:a5:a1

void printLocalTime(bool printToSerial = false)
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.print(F("NTP sync failed "));
        return;
    }
    strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
    if (printToSerial)
        Serial.println(timeStringBuff);
}

void sendData()
{
    read_MPU_1();
    // Store measured value into point
    sensor.clearFields();

    // Data
    sensor.addField("Roll1", roll1);
    sensor.addField("Pitch1", pitch1);
    sensor.addField("Yaw1", yaw1);
    sensor.addField("Roll1", roll2);
    sensor.addField("Pitch1", pitch2);
    sensor.addField("Yaw1", yaw2);
    sensor.addField("Roll1", roll3);
    sensor.addField("Pitch1", pitch3);
    sensor.addField("Yaw1", yaw3);
    sensor.addField("Roll1", roll4);
    sensor.addField("Pitch1", pitch4);
    sensor.addField("Yaw1", yaw4);
    sensor.addField("Roll1", roll5);
    sensor.addField("Pitch1", pitch5);
    sensor.addField("Yaw1", yaw5);
    // Print what are we exactly writing
    Serial.print("Writing: ");
    Serial.println(client.pointToLineProtocol(sensor));

    // Write point
    if (!client.writePoint(sensor))
    {
        Serial.print("InfluxDB write failed: ");
        Serial.println(client.getLastErrorMessage());
    }
}

void setup()
{
    Serial.begin(115200);
    delay(10);
    Serial.println();

    Wire.begin();
    TCA9548A(3);
    mpu.begin();
    Serial.println(F("Calculating gyro offset, do not move MPU6050"));
    mpu.calcGyroOffsets(); // This does the calibration
    delay(20);

    TCA9548A(4);
    mpu.begin();
    Serial.println(F("Calculating gyro offset, do not move MPU6050"));
    mpu.calcGyroOffsets();
    delay(20);

    TCA9548A(5);
    mpu.begin();
    Serial.println(F("Calculating gyro offset, do not move MPU6050"));
    mpu.calcGyroOffsets();
    delay(20);

    TCA9548A(6);
    mpu.begin();
    Serial.println(F("Calculating gyro offset, do not move MPU6050"));
    mpu.calcGyroOffsets();
    delay(20);

    TCA9548A(7);
    mpu.begin();
    Serial.println(F("Calculating gyro offset, do not move MPU6050"));
    mpu.calcGyroOffsets();
    delay(20);

    WiFi.disconnect(true); //disconnect form wifi to set new wifi connection
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

    /* wifi WPA2 begin 
    WiFi.mode(WIFI_STA);   //init wifi mode
    // esp_wifi_set_mac(WIFI_IF_STA, &masterCustomMac[0]);
    Serial.print("MAC >> ");
    Serial.println(WiFi.macAddress());
    Serial.printf("Connecting to WiFi: %s ", ssid);
    WiFi.setHostname(HOSTNAME);
    esp_wifi_sta_wpa2_ent_set_ca_cert((uint8_t *)incommon_ca, strlen(incommon_ca) + 1);
    esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
    esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT();
    esp_wifi_sta_wpa2_ent_enable(&config);
    WiFi.begin(ssid);

    */
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(F("."));
        counter++;
        if (counter >= 60)
        { //after 30 seconds timeout - reset board
            ESP.restart();
        }
    }
    Serial.println(F(" connected!"));
    Serial.print(F("IP address set: "));
    Serial.println(WiFi.localIP()); //print LAN IP

    //init and get the time
    configTime(0, 0, ntpServer);
    setenv("TZ", timezoneEST, 1);
    time_t now;
    Serial.print("Obtaining NTP time: ");
    printLocalTime();
    while (now < 1510592825)
    {
        Serial.print(F("."));
        delay(500);
        time(&now);
    }
    Serial.print(F(" success!\nGot Time: "));
    printLocalTime(true);
    Serial.print(F("NTP time received!\n"));

    // Add constant tags - only once
    sensor.addTag("device", HOSTNAME);
    sensor.addTag("location", LOCATION);

    // Check Influxdb server connection
    if (client.validateConnection())
    {
        Serial.print("Connected to InfluxDB: ");
        Serial.println(client.getServerUrl());
    }
    else
    {
        Serial.print("InfluxDB connection failed: ");
        Serial.println(client.getLastErrorMessage());
    }
}

void loop()
{
    unsigned long currentMillis = millis();

    if (WiFi.status() == WL_CONNECTED)
    {                                                        //if we are connected to Eduroam network
        counter = 0;                                         //reset counter
        if (currentMillis - previousMillisWiFi >= 15 * 1000) // send data every 15 sec
        {
            printLocalTime(true);
            sendData();
            previousMillisWiFi = currentMillis;
            Serial.print(F("Wifi is still connected with IP: "));
            Serial.println(WiFi.localIP()); //inform user about his IP address
        }
    }
    else if (WiFi.status() != WL_CONNECTED)
    { //if we lost connection, retry
        WiFi.begin(ssid, password);

        Serial.printf("Connecting to WiFi: %s ", ssid);
        while (WiFi.status() != WL_CONNECTED)
        { //during lost connection, print dots
            delay(500);
            Serial.print(F("."));
            counter++;
            if (counter >= 60)
            { //30 seconds timeout - reset board
                ESP.restart();
            }
        }
        Serial.println(F(" connected!"));
    }
}

void read_MPU_1()
{
    // get pitch, roll, yaw accelerometer data /////
    TCA9548A(3);
    mpu.update();
    pitch1 = mpu.getAngleX();
    roll1 = mpu.getAngleY();
    yaw1 = mpu.getAngleZ();
    Serial.print(roll1);
    Serial.print("/");
    Serial.print(pitch1);
    Serial.print("/");
    Serial.println(yaw1);
    delay(1000);
}

void read_MPU_2()
{
    TCA9548A(4);
    mpu.update();
    pitch2 = mpu.getAngleX();
    roll2 = mpu.getAngleY();
    yaw2 = mpu.getAngleZ();
    Serial.print("pitch2: ");
    Serial.println(pitch2);
    Serial.print("roll2: ");
    Serial.println(roll2);
    Serial.print("yaw2: ");
    Serial.println(yaw2);
    delay(1000);
}

void read_MPU_3()
{
    TCA9548A(5);
    mpu.update();
    pitch3 = mpu.getAngleX();
    roll3 = mpu.getAngleY();
    yaw3 = mpu.getAngleZ();
    Serial.print("pitch3: ");
    Serial.println(pitch3);
    Serial.print("roll3: ");
    Serial.println(roll3);
    Serial.print("yaw3: ");
    Serial.println(yaw3);
    delay(100);
}

void read_MPU_4()
{
    TCA9548A(6);
    mpu.update();
    pitch4 = mpu.getAngleX();
    roll4 = mpu.getAngleY();
    yaw4 = mpu.getAngleZ();
    Serial.print("pitch4: ");
    Serial.println(pitch4);
    Serial.print("roll4: ");
    Serial.println(roll4);
    Serial.print("yaw4: ");
    Serial.println(yaw4);
    delay(100);
}

void read_MPU_5()
{
    TCA9548A(7);
    mpu.update();
    pitch5 = mpu.getAngleX();
    roll5 = mpu.getAngleY();
    yaw5 = mpu.getAngleZ();
    Serial.print("pitch5: ");
    Serial.println(pitch5);
    Serial.print("roll5: ");
    Serial.println(roll5);
    Serial.print("yaw5: ");
    Serial.println(yaw5);

    delay(2000);
}