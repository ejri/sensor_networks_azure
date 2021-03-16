
//////// reading raw data. To calculate G-force, divide raw data
/// by (16384), for sentivity of +/-2g
//// 2g   : 16384
//// 4g   : 8192
//// 8g   : 4096
//// 16g  : 2048
/// 
//// example: 
//// aX1 = aX1/16384;

// TCA9548A I2C Switch between: 
/// 5 MPU 6050
// GY521-MPU 6050 at address, on TCA9548's 
// at channels 3,4,5,6,7
// TCA9548 module I2C address:  0x70
// 

//======================================================
//I2C Mutiplexing using TCA9548A at I2C address 0x70
//======================================================
#include <Wire.h>


const int MPU = 0x68;
int16_t aX1, aY1, aZ1, gX1, gY1, gZ1, aX2, aY2, aZ2, gX2, gY2, gZ2,aX3, aY3, aZ3, gX3, gY3, gZ3, aX4, aY4, aZ4, gX4, gY4, gZ4, aX5, aY5, aZ5, gX5, gY5, gZ5;

void TCA9548A(uint8_t bus)
{
  Wire.beginTransmission(0x70);
  Wire.write(1 << bus);
  Wire.endTransmission();
}





//=============================================================================
void setup()
{
  Wire.begin();
  
  // first MPU6050 at channel 3
  TCA9548A(3);
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  delay(1000);

  TCA9548A(4);
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  delay(1000);

  TCA9548A(5);
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  delay(1000);

  TCA9548A(6);
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  delay(1000);

  TCA9548A(7);
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
    Serial.print(","); Serial.print(aY5);
    Serial.print(","); Serial.print(aZ5); 
    Serial.print(","); Serial.print(gX5);
    Serial.print(","); Serial.print(gY5);
    Serial.print(","); Serial.println(gZ5);
    
    delay(1000);
}
