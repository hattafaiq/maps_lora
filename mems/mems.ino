#include <Wire.h>
#include <MPU9250_asukiaaa.h>

float aX, aY, aZ, aSqrt, gX, gY, gZ, mDirection, mX, mY, mZ;
double roll , pitch;

MPU9250_asukiaaa mySensor;

void setup()
{
    Serial.begin(115200);
    Serial.print(" MEMS & TEmp ");
   
  mySensor.beginAccel();
  mySensor.beginGyro();
  mySensor.beginMag();
}

void loop()
{
  if (mySensor.accelUpdate() == 0) {
    aX = mySensor.accelX();
    aY = mySensor.accelY();
    aZ = mySensor.accelZ();
    aSqrt = mySensor.accelSqrt();
//    Serial.println("accelX: " + String(aX));
//    Serial.println("accelY: " + String(aY));
//    Serial.println("accelZ: " + String(aZ));
//    Serial.println("accelSqrt: " + String(aSqrt));
  } else {
    Serial.println("Cannod read accel values");
  }

  if (mySensor.gyroUpdate() == 0) {
    gX = mySensor.gyroX();
    gY = mySensor.gyroY();
    gZ = mySensor.gyroZ();
    Serial.println("gyroX: " + String(abs(gX)));
    Serial.println("gyroY: " + String(abs(gY)));
//    Serial.println("gyroZ: " + String(gZ));
  } else {
    Serial.println("Cannot read gyro values");
  }
    aX = mySensor.accelX();
    aY = mySensor.accelY();
    aZ = mySensor.accelZ();
    mX = mySensor.magX();
    mY = mySensor.magY();
    mZ = mySensor.magZ();
    aSqrt = mySensor.accelSqrt();
   pitch = atan2 (aY ,( sqrt ((aX * aX) + (aZ * aZ))));
   roll = atan2(-aX ,( sqrt((aY * aY) + (aZ * aZ))));
//  Serial.println("pitch"  + String( pitch) );
//  Serial.println("roll" + String( roll));
 delay(200);
}
