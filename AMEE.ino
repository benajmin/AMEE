#include <Wire.h>
#include <math.h>
#include "kalman.h" 
int ma1 = 8, ma2 = 7, mb1 = 4, mb2 = 5;
int mae = 6, mbe = 3;
int s;
int i = 0;

#define gotta double
#define fastt true

Kalman k;

int gyroY, accX, accZ;
float pTime, rate, ACCangle, kAngle;
double zx;

double speed; //Gotta go fast
float boat; //otherwise it will sink
double fast; // Gotta go faster
gotta goo = fastt;

float pid(float p, float d);
void go(float s);

void setup() {
  // put your setup code here, to run once:
  pinMode(ma1, OUTPUT);
  pinMode(ma2, OUTPUT);
  pinMode(mb1, OUTPUT);
  pinMode(mb2, OUTPUT);

  Serial.begin(9600);

  Wire.begin();
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  pTime = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 12, true);
  accX = Wire.read()<<8;
  accX |= Wire.read();
  Wire.read();
  Wire.read();
  accZ = Wire.read()<<8;
  accZ |= Wire.read();
  Wire.read();
  Wire.read();
  Wire.read();
  Wire.read();
  gyroY = Wire.read()<<8;
  gyroY |= Wire.read();

  rate = map(gyroY, -32768, 32767, -25000, 25000);
  rate /= 100;
  zx = (double)accZ/accX;
  ACCangle = atan(zx)*57.2958;
  kAngle = k.getAngle(ACCangle, rate, ((float)millis()-pTime)/1000);

  float s = pid(kAngle, kAngle + rate*(millis()-pTime)/1000);

  Serial.print(kAngle);
  Serial.print(' ');
  Serial.print(kAngle + rate*(millis()-pTime)/1000);
  Serial.print(' ');
  Serial.println(s);
  
  go(s);
  
  
  pTime = millis();
  //delay(500);
}

void go(float s){
  int en = map(abs(s), -1, 7, 170, 400);

  analogWrite(mae, en);
  analogWrite(mbe, en);
  
  if (s<0){
    digitalWrite(ma1, HIGH);
    digitalWrite(mb1, HIGH);
    digitalWrite(ma2, LOW);
    digitalWrite(mb2, LOW);
  }else if(s>0){
    digitalWrite(ma1, LOW);
    digitalWrite(mb1, LOW);
    digitalWrite(ma2, HIGH);
    digitalWrite(mb2, HIGH);
  }
}

float pid(float p, float d){
  return (p*0.3+d*0.7);
}

