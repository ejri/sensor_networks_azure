
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

float accAngleX1, accAngleY1, gyroAngleX1, gyroAngleY1, gyroAngleZ1;
float roll1, pitch1, yaw1;
float AccErrorX1, AccErrorY1, GyroErrorX1, GyroErrorY1, GyroErrorZ1;
float accAngleX2, accAngleY2, gyroAngleX2, gyroAngleY2, gyroAngleZ2;
float roll2, pitch2, yaw2;
float AccErrorX2, AccErrorY2, GyroErrorX2, GyroErrorY2, GyroErrorZ2;
float accAngleX3, accAngleY3, gyroAngleX3, gyroAngleY3, gyroAngleZ3;
float roll3, pitch3, yaw3;
float AccErrorX3, AccErrorY3, GyroErrorX3, GyroErrorY3, GyroErrorZ3;
float accAngleX4, accAngleY4, gyroAngleX4, gyroAngleY4, gyroAngleZ4;
float roll4, pitch4, yaw4;
float AccErrorX4, AccErrorY4, GyroErrorX4, GyroErrorY4, GyroErrorZ4;
float accAngleX5, accAngleY5, gyroAngleX5, gyroAngleY5, gyroAngleZ5;
float roll5, pitch5, yaw5;
float AccErrorX5, AccErrorY5, GyroErrorX5, GyroErrorY5, GyroErrorZ5;

float elapsedTime1, currentTime1, previousTime1;
int c1 = 0;
float elapsedTime2, currentTime2, previousTime2;
int c2 = 0;
float elapsedTime3, currentTime3, previousTime3;
int c3 = 0;
float elapsedTime4, currentTime4, previousTime4;
int c4 = 0;
float elapsedTime5, currentTime5, previousTime5;
int c5 = 0;




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

// calculate reading errors
  calculate_IMU_error();
  delay(20);
}
  
//============================================================================= 
void loop()
{
  // get raw accelerometer data /////
      TCA9548A(3);
      Wire.beginTransmission(MPU);
      Wire.write(0x3B);
      Wire.endTransmission(false);
      Wire.requestFrom(MPU, 6, true);
      aX1 = Wire.read() << 8 | Wire.read() /16384 ;
      aY1 = Wire.read() << 8 | Wire.read() /16384 ;
      aZ1 = Wire.read() << 8 | Wire.read() /16384 ;

// Calculating Roll and Pitch from the accelerometer data
accAngleX1 = (atan(aY1 / sqrt(pow(aX1, 2) + pow(aZ1, 2))) * 180 / PI) - 0.58; // AccErrorX ~(0.58) See the calculate_IMU_error()custom function for more details
accAngleY1 = (atan(-1 * aX1 / sqrt(pow(aY1, 2) + pow(aZ1, 2))) * 180 / PI) + 1.58; // AccErrorY ~(-1.58)

// === Read gyroscope data === //
  previousTime1 = currentTime1;        // Previous time is stored before the actual time read
  currentTime1 = millis();            // Current time actual time read
  elapsedTime1 = (currentTime1 - previousTime1) / 1000; // Divide by 1000 to get seconds
  TCA9548A(3);
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  gX1 = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  gY1 = (Wire.read() << 8 | Wire.read()) / 131.0;
  gZ1 = (Wire.read() << 8 | Wire.read()) / 131.0;
  // Correct the outputs with the calculated error values
  gX1 = gX1 + 0.56; // GyroErrorX ~(-0.56)
  gY1 = gY1 - 2; // GyroErrorY ~(2)
  gZ1 = gZ1 + 0.79; // GyroErrorZ ~ (-0.8)
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  gyroAngleX1 = gyroAngleX1 + gX1 * elapsedTime1; // deg/s * s = deg
  gyroAngleY1 = gyroAngleY1 + gY1 * elapsedTime1;
  yaw1 =  yaw1 + gZ1 * elapsedTime1;
  // Complementary filter - combine acceleromter and gyro angle values
  roll1 = 0.96 * gyroAngleX1 + 0.04 * accAngleX1;
  pitch1 = 0.96 * gyroAngleY1 + 0.04 * accAngleY1;
  
  // Print the values on the serial monitor
  Serial.print("MPU 1: roll: ");Serial.print(roll1);
  Serial.print("/ pitch: ");
  Serial.print(pitch1);
  Serial.print("/ yaw: ");
  Serial.println(yaw1);
    
    delay(1000);

  // get raw accelerometer data /////
      TCA9548A(4);
      Wire.beginTransmission(MPU);
      Wire.write(0x3B);
      Wire.endTransmission(false);
      Wire.requestFrom(MPU, 6, true);
      aX2 = Wire.read() << 8 | Wire.read() /16384 ;
      aY2 = Wire.read() << 8 | Wire.read() /16384 ;
      aZ2 = Wire.read() << 8 | Wire.read() /16384 ;

// Calculating Roll and Pitch from the accelerometer data
accAngleX2 = (atan(aY2 / sqrt(pow(aX2, 2) + pow(aZ2, 2))) * 180 / PI) - 0.58; // AccErrorX ~(0.58) See the calculate_IMU_error()custom function for more details
accAngleY2 = (atan(-1 * aX2 / sqrt(pow(aY2, 2) + pow(aZ2, 2))) * 180 / PI) + 1.58; // AccErrorY ~(-1.58)

// === Read gyroscope data === //
  previousTime2 = currentTime2;        // Previous time is stored before the actual time read
  currentTime2 = millis();            // Current time actual time read
  elapsedTime2 = (currentTime2 - previousTime2) / 1000; // Divide by 1000 to get seconds
  TCA9548A(4);
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  gX2 = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  gY2 = (Wire.read() << 8 | Wire.read()) / 131.0;
  gZ2 = (Wire.read() << 8 | Wire.read()) / 131.0;
  // Correct the outputs with the calculated error values
  gX2 = gX2 + 0.56; // GyroErrorX ~(-0.56)
  gY2 = gY2 - 2; // GyroErrorY ~(2)
  gZ2 = gZ2 + 0.79; // GyroErrorZ ~ (-0.8)
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  gyroAngleX2 = gyroAngleX2 + gX2 * elapsedTime2; // deg/s * s = deg
  gyroAngleY2 = gyroAngleY2 + gY2 * elapsedTime2;
  yaw2 =  yaw2 + gZ2 * elapsedTime2;
  // Complementary filter - combine acceleromter and gyro angle values
  roll2 = 0.96 * gyroAngleX2 + 0.04 * accAngleX2;
  pitch2 = 0.96 * gyroAngleY2 + 0.04 * accAngleY2;
  
  // Print the values on the serial monitor
  Serial.print("MPU 2: roll: ");Serial.print(roll2);
  Serial.print("/ pitch: ");
  Serial.print(pitch2);
  Serial.print("/ yaw: ");
  Serial.println(yaw2);
    
    delay(1000);




   // get raw accelerometer data /////
      TCA9548A(5);
      Wire.beginTransmission(MPU);
      Wire.write(0x3B);
      Wire.endTransmission(false);
      Wire.requestFrom(MPU, 6, true);
      aX3 = Wire.read() << 8 | Wire.read() /16384 ;
      aY3 = Wire.read() << 8 | Wire.read() /16384 ;
      aZ3 = Wire.read() << 8 | Wire.read() /16384 ;

// Calculating Roll and Pitch from the accelerometer data
accAngleX3 = (atan(aY3 / sqrt(pow(aX3, 2) + pow(aZ3, 2))) * 180 / PI) - 0.58; // AccErrorX ~(0.58) See the calculate_IMU_error()custom function for more details
accAngleY3 = (atan(-1 * aX3 / sqrt(pow(aY3, 2) + pow(aZ3, 2))) * 180 / PI) + 1.58; // AccErrorY ~(-1.58)

// === Read gyroscope data === //
  previousTime3 = currentTime3;        // Previous time is stored before the actual time read
  currentTime3 = millis();            // Current time actual time read
  elapsedTime3 = (currentTime3 - previousTime3) / 1000; // Divide by 1000 to get seconds
  TCA9548A(5);
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  gX3 = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  gY3 = (Wire.read() << 8 | Wire.read()) / 131.0;
  gZ3 = (Wire.read() << 8 | Wire.read()) / 131.0;
  // Correct the outputs with the calculated error values
  gX3 = gX3 + 0.56; // GyroErrorX ~(-0.56)
  gY3 = gY3 - 2; // GyroErrorY ~(2)
  gZ3 = gZ3 + 0.79; // GyroErrorZ ~ (-0.8)
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  gyroAngleX3 = gyroAngleX3 + gX3 * elapsedTime3; // deg/s * s = deg
  gyroAngleY3 = gyroAngleY3 + gY3 * elapsedTime3;
  yaw3 =  yaw3 + gZ3 * elapsedTime3;
  // Complementary filter - combine acceleromter and gyro angle values
  roll3 = 0.96 * gyroAngleX3 + 0.04 * accAngleX3;
  pitch3 = 0.96 * gyroAngleY3 + 0.04 * accAngleY3;
  
  // Print the values on the serial monitor
  Serial.print("MPU 3: roll: ");Serial.print(roll3);
  Serial.print("/ pitch: ");
  Serial.print(pitch3);
  Serial.print("/ yaw: ");
  Serial.println(yaw3);
    
    delay(1000);

 // get raw accelerometer data /////
      TCA9548A(6);
      Wire.beginTransmission(MPU);
      Wire.write(0x3B);
      Wire.endTransmission(false);
      Wire.requestFrom(MPU, 6, true);
      aX4 = Wire.read() << 8 | Wire.read() /16384 ;
      aY4 = Wire.read() << 8 | Wire.read() /16384 ;
      aZ4 = Wire.read() << 8 | Wire.read() /16384 ;

// Calculating Roll and Pitch from the accelerometer data
accAngleX4 = (atan(aY4 / sqrt(pow(aX4, 2) + pow(aZ4, 2))) * 180 / PI) - 0.58; // AccErrorX ~(0.58) See the calculate_IMU_error()custom function for more details
accAngleY4 = (atan(-1 * aX4 / sqrt(pow(aY4, 2) + pow(aZ4, 2))) * 180 / PI) + 1.58; // AccErrorY ~(-1.58)

// === Read gyroscope data === //
  previousTime4 = currentTime4;        // Previous time is stored before the actual time read
  currentTime4 = millis();            // Current time actual time read
  elapsedTime4 = (currentTime4 - previousTime4) / 1000; // Divide by 1000 to get seconds
  TCA9548A(6);
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  gX4 = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  gY4 = (Wire.read() << 8 | Wire.read()) / 131.0;
  gZ4 = (Wire.read() << 8 | Wire.read()) / 131.0;
  // Correct the outputs with the calculated error values
  gX4 = gX4 + 0.56; // GyroErrorX ~(-0.56)
  gY4 = gY4 - 2; // GyroErrorY ~(2)
  gZ4 = gZ4 + 0.79; // GyroErrorZ ~ (-0.8)
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  gyroAngleX4 = gyroAngleX4 + gX4 * elapsedTime4; // deg/s * s = deg
  gyroAngleY4 = gyroAngleY4 + gY4 * elapsedTime4;
  yaw4 =  yaw4 + gZ4 * elapsedTime4;
  // Complementary filter - combine acceleromter and gyro angle values
  roll4 = 0.96 * gyroAngleX4 + 0.04 * accAngleX4;
  pitch4 = 0.96 * gyroAngleY4 + 0.04 * accAngleY4;
  
  // Print the values on the serial monitor
  Serial.print("MPU 4: roll: ");Serial.print(roll4);
  Serial.print("/ pitch: ");
  Serial.print(pitch4);
  Serial.print("/ yaw: ");
  Serial.println(yaw4);
    
    delay(1000);

   // get raw accelerometer data /////
      TCA9548A(7);
      Wire.beginTransmission(MPU);
      Wire.write(0x3B);
      Wire.endTransmission(false);
      Wire.requestFrom(MPU, 6, true);
      aX5 = Wire.read() << 8 | Wire.read() /16384 ;
      aY5 = Wire.read() << 8 | Wire.read() /16384 ;
      aZ5 = Wire.read() << 8 | Wire.read() /16384 ;

// Calculating Roll and Pitch from the accelerometer data
accAngleX5 = (atan(aY5 / sqrt(pow(aX5, 2) + pow(aZ5, 2))) * 180 / PI) - 0.58; // AccErrorX ~(0.58) See the calculate_IMU_error()custom function for more details
accAngleY5 = (atan(-1 * aX5 / sqrt(pow(aY5, 2) + pow(aZ5, 2))) * 180 / PI) + 1.58; // AccErrorY ~(-1.58)

// === Read gyroscope data === //
  previousTime5 = currentTime5;        // Previous time is stored before the actual time read
  currentTime5 = millis();            // Current time actual time read
  elapsedTime5 = (currentTime5 - previousTime5) / 1000; // Divide by 1000 to get seconds
  TCA9548A(7);
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  gX5 = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  gY5 = (Wire.read() << 8 | Wire.read()) / 131.0;
  gZ5 = (Wire.read() << 8 | Wire.read()) / 131.0;
  // Correct the outputs with the calculated error values
  gX5 = gX5 + 0.56; // GyroErrorX ~(-0.56)
  gY5 = gY5 - 2; // GyroErrorY ~(2)
  gZ5 = gZ5 + 0.79; // GyroErrorZ ~ (-0.8)
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  gyroAngleX5 = gyroAngleX5 + gX5 * elapsedTime5; // deg/s * s = deg
  gyroAngleY5 = gyroAngleY5 + gY5 * elapsedTime5;
  yaw5 =  yaw5 + gZ5 * elapsedTime5;
  // Complementary filter - combine acceleromter and gyro angle values
  roll5 = 0.96 * gyroAngleX5 + 0.04 * accAngleX5;
  pitch5 = 0.96 * gyroAngleY5 + 0.04 * accAngleY5;
  
  // Print the values on the serial monitor
  Serial.print("MPU 5: roll: ");Serial.print(roll5);
  Serial.print("/ pitch: ");
  Serial.print(pitch5);
  Serial.print("/ yaw: ");
  Serial.println(yaw5);
    
    delay(1000);
}

void calculate_IMU_error() {
  // We can call this funtion in the setup section to calculate the accelerometer and gyro data error. From here we will get the error values used in the above equations printed on the Serial Monitor.
  // Note that we should place the IMU flat in order to get the proper values, so that we then can the correct values
  // Read accelerometer values 200 times
  while (c1 < 200) {
    TCA9548A(3);
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    aX1 = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    aY1 = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    aZ1 = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    // Sum all readings
    AccErrorX1 = AccErrorX1 + ((atan((aY1) / sqrt(pow((aX1), 2) + pow((aZ1), 2))) * 180 / PI));
    AccErrorY1 = AccErrorY1 + ((atan(-1 * (aX1) / sqrt(pow((aY1), 2) + pow((aZ1), 2))) * 180 / PI));
    c1++;
  }
  //Divide the sum by 200 to get the error value
  AccErrorX1 = AccErrorX1 / 200;
  AccErrorY1 = AccErrorY1 / 200;
  c1 = 0;
  // Read gyro values 200 times
  while (c1 < 200) {
    TCA9548A(3);
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    gX1 = Wire.read() << 8 | Wire.read();
    gY1 = Wire.read() << 8 | Wire.read();
    gZ1 = Wire.read() << 8 | Wire.read();
    // Sum all readings
    GyroErrorX1 = GyroErrorX1 + (gX1 / 131.0);
    GyroErrorY1 = GyroErrorY1 + (gY1 / 131.0);
    GyroErrorZ1 = GyroErrorZ1 + (gZ1 / 131.0);
    c1++;
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX1 = GyroErrorX1 / 200;
  GyroErrorY1 = GyroErrorY1 / 200;
  GyroErrorZ1 = GyroErrorZ1 / 200;
  // Print the error values on the Serial Monitor
  Serial.print("AccErrorX1: ");
  Serial.println(AccErrorX1);
  Serial.print("AccErrorY1: ");
  Serial.println(AccErrorY1);
  Serial.print("GyroErrorX1: ");
  Serial.println(GyroErrorX1);
  Serial.print("GyroErrorY1: ");
  Serial.println(GyroErrorY1);
  Serial.print("GyroErrorZ1: ");
  Serial.println(GyroErrorZ1);

while (c1 < 200) {
    TCA9548A(4);
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    aX2 = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    aY2 = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    aZ2 = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    // Sum all readings
    AccErrorX2 = AccErrorX2 + ((atan((aY2) / sqrt(pow((aX2), 2) + pow((aZ2), 2))) * 180 / PI));
    AccErrorY2 = AccErrorY2 + ((atan(-1 * (aX2) / sqrt(pow((aY2), 2) + pow((aZ2), 2))) * 180 / PI));
    c2++;
  }
  //Divide the sum by 200 to get the error value
  AccErrorX2 = AccErrorX2 / 200;
  AccErrorY2 = AccErrorY2 / 200;
  c2 = 0;
  // Read gyro values 200 times
  while (c2 < 200) {
    TCA9548A(4);
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    gX2 = Wire.read() << 8 | Wire.read();
    gY2 = Wire.read() << 8 | Wire.read();
    gZ2 = Wire.read() << 8 | Wire.read();
    // Sum all readings
    GyroErrorX2 = GyroErrorX2 + (gX2 / 131.0);
    GyroErrorY2 = GyroErrorY2 + (gY2 / 131.0);
    GyroErrorZ2 = GyroErrorZ2 + (gZ2 / 131.0);
    c2++;
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX2 = GyroErrorX2 / 200;
  GyroErrorY2 = GyroErrorY2 / 200;
  GyroErrorZ2 = GyroErrorZ2 / 200;
  // Print the error values on the Serial Monitor
  Serial.print("AccErrorX2: ");
  Serial.println(AccErrorX2);
  Serial.print("AccErrorY2: ");
  Serial.println(AccErrorY2);
  Serial.print("GyroErrorX2: ");
  Serial.println(GyroErrorX2);
  Serial.print("GyroErrorY2: ");
  Serial.println(GyroErrorY2);
  Serial.print("GyroErrorZ2: ");
  Serial.println(GyroErrorZ2);


while (c3 < 200) {
    TCA9548A(5);
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    aX3 = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    aY3 = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    aZ3 = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    // Sum all readings
    AccErrorX3 = AccErrorX3 + ((atan((aY3) / sqrt(pow((aX3), 2) + pow((aZ3), 2))) * 180 / PI));
    AccErrorY3 = AccErrorY3 + ((atan(-1 * (aX3) / sqrt(pow((aY3), 2) + pow((aZ3), 2))) * 180 / PI));
    c3++;
  }
  //Divide the sum by 200 to get the error value
  AccErrorX3 = AccErrorX3 / 200;
  AccErrorY3 = AccErrorY3 / 200;
  c3 = 0;
  // Read gyro values 200 times
  while (c3 < 200) {
    TCA9548A(5);
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    gX3 = Wire.read() << 8 | Wire.read();
    gY3 = Wire.read() << 8 | Wire.read();
    gZ3 = Wire.read() << 8 | Wire.read();
    // Sum all readings
    GyroErrorX3 = GyroErrorX3 + (gX3 / 131.0);
    GyroErrorY3 = GyroErrorY3 + (gY3 / 131.0);
    GyroErrorZ3 = GyroErrorZ3 + (gZ3 / 131.0);
    c3++;
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX3 = GyroErrorX3 / 200;
  GyroErrorY3 = GyroErrorY3 / 200;
  GyroErrorZ3 = GyroErrorZ3 / 200;
  // Print the error values on the Serial Monitor
  Serial.print("AccErrorX3: ");
  Serial.println(AccErrorX3);
  Serial.print("AccErrorY3: ");
  Serial.println(AccErrorY3);
  Serial.print("GyroErrorX3: ");
  Serial.println(GyroErrorX3);
  Serial.print("GyroErrorY3: ");
  Serial.println(GyroErrorY3);
  Serial.print("GyroErrorZ3: ");
  Serial.println(GyroErrorZ3);

while (c4 < 200) {
    TCA9548A(6);
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    aX4 = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    aY4 = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    aZ4 = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    // Sum all readings
    AccErrorX4 = AccErrorX4 + ((atan((aY4) / sqrt(pow((aX4), 2) + pow((aZ4), 2))) * 180 / PI));
    AccErrorY4 = AccErrorY4 + ((atan(-1 * (aX4) / sqrt(pow((aY4), 2) + pow((aZ4), 2))) * 180 / PI));
    c4++;
  }
  //Divide the sum by 200 to get the error value
  AccErrorX4 = AccErrorX4 / 200;
  AccErrorY4 = AccErrorY4 / 200;
  c4 = 0;
  // Read gyro values 200 times
  while (c4 < 200) {
    TCA9548A(6);
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    gX4 = Wire.read() << 8 | Wire.read();
    gY4 = Wire.read() << 8 | Wire.read();
    gZ4 = Wire.read() << 8 | Wire.read();
    // Sum all readings
    GyroErrorX4 = GyroErrorX4 + (gX4 / 131.0);
    GyroErrorY4 = GyroErrorY4 + (gY4 / 131.0);
    GyroErrorZ4 = GyroErrorZ4 + (gZ4 / 131.0);
    c4++;
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX4 = GyroErrorX4 / 200;
  GyroErrorY4 = GyroErrorY4 / 200;
  GyroErrorZ4 = GyroErrorZ4 / 200;
  // Print the error values on the Serial Monitor
  Serial.print("AccErrorX4: ");
  Serial.println(AccErrorX4);
  Serial.print("AccErrorY4: ");
  Serial.println(AccErrorY4);
  Serial.print("GyroErrorX4: ");
  Serial.println(GyroErrorX4);
  Serial.print("GyroErrorY4: ");
  Serial.println(GyroErrorY4);
  Serial.print("GyroErrorZ4: ");
  Serial.println(GyroErrorZ4);

while (c5 < 200) {
    TCA9548A(7);
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    aX5 = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    aY5 = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    aZ5 = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    // Sum all readings
    AccErrorX5 = AccErrorX5 + ((atan((aY5) / sqrt(pow((aX5), 2) + pow((aZ5), 2))) * 180 / PI));
    AccErrorY5 = AccErrorY5 + ((atan(-1 * (aX5) / sqrt(pow((aY5), 2) + pow((aZ5), 2))) * 180 / PI));
    c5++;
  }
  //Divide the sum by 200 to get the error value
  AccErrorX5 = AccErrorX5 / 200;
  AccErrorY5 = AccErrorY5 / 200;
  c5 = 0;
  // Read gyro values 200 times
  while (c5 < 200) {
    TCA9548A(7);
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    gX5 = Wire.read() << 8 | Wire.read();
    gY5 = Wire.read() << 8 | Wire.read();
    gZ5 = Wire.read() << 8 | Wire.read();
    // Sum all readings
    GyroErrorX5 = GyroErrorX5 + (gX5 / 131.0);
    GyroErrorY5 = GyroErrorY5 + (gY5 / 131.0);
    GyroErrorZ5 = GyroErrorZ5 + (gZ5 / 131.0);
    c5++;
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX5 = GyroErrorX5 / 200;
  GyroErrorY5 = GyroErrorY5 / 200;
  GyroErrorZ5 = GyroErrorZ5 / 200;
  // Print the error values on the Serial Monitor
  Serial.print("AccErrorX5: ");
  Serial.println(AccErrorX5);
  Serial.print("AccErrorY5: ");
  Serial.println(AccErrorY5);
  Serial.print("GyroErrorX5: ");
  Serial.println(GyroErrorX5);
  Serial.print("GyroErrorY5: ");
  Serial.println(GyroErrorY5);
  Serial.print("GyroErrorZ5: ");
  Serial.println(GyroErrorZ5);
  
}
