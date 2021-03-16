


// TCA9548A I2C Switch between: 
// adafruit LSM9DS1 at address, on TCA9548's channel 0
// GY521-MPU 6050 at address, on TCA9548's channel 1
// TCA9548 module I2C address:  0x70
// 

//======================================================
//I2C Mutiplexing using TCA9548A at I2C address 0x70
//======================================================
#include <Wire.h>
#include <Adafruit_LSM9DS1.h>
#include <Adafruit_Sensor.h>  // not used in this demo but required!


Adafruit_LSM9DS1 lsm_1 = Adafruit_LSM9DS1(1);

#define LSM9DS1_SCK A5
#define LSM9DS1_MISO 12
#define LSM9DS1_MOSI A4
#define LSM9DS1_XGCS 6
#define LSM9DS1_MCS 5

const int MPU = 0x68;
int16_t aX, aY, aZ, gX, gY, gZ;

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
void setup()
{
  Wire.begin();
  

  /* Initialise the 1st sensor */
  Serial.println("LSM9DS1 data read demo");
  
  // Try to initialise and warn if we couldn't detect the chip
  TCA9548A(0);
  if (!lsm_1.begin())
  {
    Serial.println("Oops ... unable to initialize the LSM9DS1. Check your wiring!");
    while (1);
  }
  Serial.println("Found LSM9DS1 9DOF");

  // helper to just set the default scaling we want, see above!
  TCA9548A(0);
  setupSensor();

  // helper to just set the default scaling we want, see above!
  TCA9548A(1);
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  delay(1000);

  Serial.begin(115200);
  
}
  
//============================================================================= 
void loop()
{
  TCA9548A(0);
lsm_1.read();  /* ask it to read in the data */ 

  /* Get a new sensor event */ 
  TCA9548A(0);
  sensors_event_t a, m, g, temp;
  TCA9548A(0);
  lsm_1.getEvent(&a, &m, &g, &temp); 

  Serial.print("Accel X: "); Serial.print(a.acceleration.x); Serial.print(" m/s^2");
  Serial.print("\tY: "); Serial.print(a.acceleration.y);     Serial.print(" m/s^2 ");
  Serial.print("\tZ: "); Serial.print(a.acceleration.z);     Serial.println(" m/s^2 ");

  Serial.print("Mag X: "); Serial.print(m.magnetic.x);   Serial.print(" uT");
  Serial.print("\tY: "); Serial.print(m.magnetic.y);     Serial.print(" uT");
  Serial.print("\tZ: "); Serial.print(m.magnetic.z);     Serial.println(" uT");

  Serial.print("Gyro X: "); Serial.print(g.gyro.x);   Serial.print(" rad/s");
  Serial.print("\tY: "); Serial.print(g.gyro.y);      Serial.print(" rad/s");
  Serial.print("\tZ: "); Serial.print(g.gyro.z);      Serial.println(" rad/s");

  Serial.println();
  delay(1000);

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

   // Serial print acceleromter data
    Serial.print(aX);
    Serial.print(","); Serial.print(aY);
    Serial.print(","); Serial.print(aZ); 
    Serial.print(","); Serial.print(gX);
    Serial.print(","); Serial.print(gY);
    Serial.print(","); Serial.println(gZ);
    
    delay(1000);
}
