/* Get tilt angles on X and Y, and rotation angle on Z
 * Angles are given in degrees, displays on SSD1306 OLED
 * 
 * License: MIT
 */
#include <Wire.h>
#include <MPU6050_light.h>
float roll, pitch, yaw;


void TCA9548A(uint8_t bus)
{
  Wire.beginTransmission(0x70);
  Wire.write(1 << bus);
  Wire.endTransmission();
}


MPU6050 mpu(Wire);
unsigned long timer = 0;

void setup() {
  Serial.begin(19200);                                                    
  Wire.begin();
  TCA9548A(3);
  mpu.begin();
  Serial.println(F("Calculating gyro offset, do not move MPU6050"));      
  mpu.calcGyroOffsets();                          // This does the calibration

TCA9548A(4);
  mpu.begin();
  Serial.println(F("Calculating gyro offset, do not move MPU6050"));      
  mpu.calcGyroOffsets(); 

TCA9548A(5);
  mpu.begin();
  Serial.println(F("Calculating gyro offset, do not move MPU6050"));      
  mpu.calcGyroOffsets(); 


TCA9548A(6);
  mpu.begin();
  Serial.println(F("Calculating gyro offset, do not move MPU6050"));      
  mpu.calcGyroOffsets(); 

TCA9548A(7);
  mpu.begin();
  Serial.println(F("Calculating gyro offset, do not move MPU6050"));      
  mpu.calcGyroOffsets(); 

}

void loop() {
  TCA9548A(3); 
  mpu.update();  
 
    pitch=mpu.getAngleX(); 
    roll=mpu.getAngleY();
    yaw=mpu.getAngleZ();                                                                    
  Serial.print(roll);
  Serial.print("/");
  Serial.print(pitch);
  Serial.print("/");
  Serial.println(yaw);
    delay(100);

TCA9548A(4); 
  mpu.update();  
    float pitch2=mpu.getAngleX(); 
    float roll2=mpu.getAngleY();
    float yaw2=mpu.getAngleZ();                                                                    
    Serial.print("pitch2: ");
    Serial.println(pitch2);
    Serial.print("roll2: ");
    Serial.println(roll2);
    Serial.print("yaw2: ");
    Serial.println(yaw2);
delay(100);

  TCA9548A(5); 
  mpu.update();  
    float pitch3=mpu.getAngleX(); 
    float roll3=mpu.getAngleY();
    float yaw3=mpu.getAngleZ();                                                                    
    Serial.print("pitch3: ");
    Serial.println(pitch3);
    Serial.print("roll3: ");
    Serial.println(roll3);
    Serial.print("yaw3: ");
    Serial.println(yaw3);
delay(100);

  TCA9548A(6); 
  mpu.update();  
    float pitch4=mpu.getAngleX(); 
    float roll4=mpu.getAngleY();
    float yaw4=mpu.getAngleZ();                                                                    
    Serial.print("pitch4: ");
    Serial.println(pitch4);
    Serial.print("roll4: ");
    Serial.println(roll4);
    Serial.print("yaw4: ");
    Serial.println(yaw4);
delay(100);

TCA9548A(7); 
  mpu.update();  
    float pitch5=mpu.getAngleX(); 
    float roll5=mpu.getAngleY();
    float yaw5=mpu.getAngleZ();                                                                    
    Serial.print("pitch5: ");
    Serial.println(pitch5);
    Serial.print("roll5: ");
    Serial.println(roll5);
    Serial.print("yaw5: ");
    Serial.println(yaw5);



 delay(500);
}
