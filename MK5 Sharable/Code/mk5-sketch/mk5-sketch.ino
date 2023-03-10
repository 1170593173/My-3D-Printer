#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN 150   // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX 450   // This is the 'maximum' pulse length count (out of 4096)
#define USMIN 600      // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX 2400     // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50  // Analog servos run at ~50 Hz updates

uint8_t servonum = 0;
int buttonPin = 2;
int ledPin = 6;
int buttonState = 0;
int globalPos = 1;
unsigned long previousMillis = 0;
unsigned long interval = 5000;
int a = 60;

int mainServoL = 0;
int mainServoR = 1;
int mainServoClosed = 5;
int mainServoOpen = 90;
int servoBrowCenter = 4;
int servoBrowCenterOpen = 40;
int servoBrowCenterClosed = 120;


void setup() {
  Serial.begin(9600);
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);
  pinMode(buttonPin, INPUT_PULLUP);
  delay(50);
  pwm.sleep();
}

/*
node:
1、arduino的map是映射的意思：arduino map(num，旧区间初值，旧区间终值，新区间初值，新区间终值)；意思就是把num这个数从旧区间映射到新区间，就是高中数学知识那个映射，而且是最简单的线性映射。
2、pwm.wakeup和pwm.sleep估计分别是唤醒和休眠的意思，应该不会是阻塞程序
3、digitalRead：读取对于pin脚的高低电平，赋值给全局变量globalPos，为0的时候需要打开面罩
4、pwm.setPWM：通过调制pwm的占空比来调整舵机角度，然后如果是需要镜像的舵机，则采用的占空比值互补的格式也就是
      for (uint16_t pulselen = 30; pulselen <= 90; pulselen++) {
        pwm.setPWM(2, 0, getAngleToPulse(pulselen));
        pwm.setPWM(3, 0, getAngleToPulse(30 + 90 - pulselen));
      }
5、pwm.writeMicroseconds，应该也是设置电机角度的一种api
*/

int getAngleToPulse(int angle) {
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}
int getAngleToMilli(int angle) {
  return map(angle, 0, 180, USMIN, USMAX);
}

void loop() {

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    Serial.println("Keeping");
    Serial.println(previousMillis);
  }

  buttonState = digitalRead(buttonPin);
  if (buttonState == 0) {
    Serial.println("Wake up");
    pwm.wakeup();

    if (globalPos > 0) {
      Serial.println("Opening");
      
      for (uint16_t pulselen = 90; pulselen >= 20; pulselen--) {
        pwm.setPWM(9, 0, getAngleToPulse(90 + 20 - pulselen));
        pwm.setPWM(8, 0, getAngleToPulse(pulselen));
      }
      delay(100);
      for (uint16_t pulselen = 90; pulselen >= 20; pulselen--) {
        pwm.setPWM(6, 0, getAngleToPulse(90 + 20 - pulselen));
        pwm.setPWM(7, 0, getAngleToPulse(pulselen));
      }
      delay(100);
      
      //BROW Center
      for (uint16_t pulselen = servoBrowCenterClosed; pulselen >= servoBrowCenterOpen; pulselen--) {
        pwm.setPWM(servoBrowCenter, 0, getAngleToPulse(pulselen));
      }
      delay(100);
      // BROW Side
      for (uint16_t pulselen = 30; pulselen <= 90; pulselen++) {
        pwm.setPWM(2, 0, getAngleToPulse(pulselen));
        pwm.setPWM(3, 0, getAngleToPulse(30 + 90 - pulselen));
      }
      delay(100);
      
      for (uint16_t pulselen = 90; pulselen >= 2; pulselen--) {
        pwm.setPWM(5, 0, getAngleToPulse(pulselen));
      }
      delay(100);
      
      analogWrite(ledPin, 0);
      

      //MAIN SERVOS
      for (uint16_t microsec = 750; microsec < 1600; microsec +=3) {
        pwm.writeMicroseconds(mainServoL, microsec);
        pwm.writeMicroseconds(mainServoR, (1600+750-microsec));
      }
      globalPos = 0;
    } else {
      Serial.println("Closing");
      //MAIN SERVOS
      for (uint16_t microsec = 1600; microsec > 750; microsec-=3) {
        pwm.writeMicroseconds(mainServoL, microsec);
        pwm.writeMicroseconds(mainServoR, (1600+750-microsec));
      }
      delay(100);
      
      for (uint16_t pulselen = 2; pulselen <= 90; pulselen++) {
        pwm.setPWM(5, 0, getAngleToPulse(pulselen));
      }
      
      delay(100);
      
      // BROW Side
      for (uint16_t pulselen = 90; pulselen >= 30; pulselen--) {
        pwm.setPWM(2, 0, getAngleToPulse(pulselen));
        pwm.setPWM(3, 0, getAngleToPulse(30 + 90 - pulselen));
      }
      delay(100);
      analogWrite(ledPin, 255);
      
      for (uint16_t pulselen = servoBrowCenterOpen; pulselen <= servoBrowCenterClosed; pulselen++) {
        pwm.setPWM(servoBrowCenter, 0, getAngleToPulse(pulselen));
      }
      delay(100);

      for (uint16_t pulselen = 20; pulselen <= 90; pulselen++) {
        pwm.setPWM(6, 0, getAngleToPulse(90 + 20 - pulselen));
        pwm.setPWM(7, 0, getAngleToPulse(pulselen));
      }
      delay(100);
      for (uint16_t pulselen = 20; pulselen <= 90; pulselen++) {
        pwm.setPWM(9, 0, getAngleToPulse(90 + 20 - pulselen));
        pwm.setPWM(8, 0, getAngleToPulse(pulselen));
      }
      delay(100);

      globalPos = 1;
    }
    delay(500);
    Serial.println("Sleep");
    pwm.sleep();
  }
  delay(100);
}
