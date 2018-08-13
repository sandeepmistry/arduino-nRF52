/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>

Servo myservo;   // create servo object to control a servo
Servo myservo1;  // create servo object to control a servo
Servo myservo2;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int i;

static const int start = 600;
static const int end = 2400;

void setup() {
  myservo.attach(3, 600, 2400);
  myservo1.attach(4, 600, 2400);
  myservo2.attach(8, 600, 2400);
  myservo.write(0);
  myservo1.write(0);
  myservo2.write(0);
}

void loop() {
  delay(3000);

  for (i = 0; i <= (end - start); i += 100) {
    myservo.write(start + i);
    myservo1.write(start + i);
    myservo2.write(end - i);
    delay(1000);
  }
  delay(3000);
  for (i = 0; i <= (end - start); i += 100) {
    myservo2.write(start + i);
    myservo1.write(end - i);
    myservo.write(end - i);
    delay(1000);
  }

  for (i = start; i <= end; i += 100) { // goes from 0 degrees to 180 degrees
    myservo.write(i);
    myservo1.write(i);
    myservo2.write(i);
    delay(1000);
  }
  delay(3000);
  for (i = 180; i >= 0; i -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(i);
    myservo1.write(i);
    myservo2.write(i);
    delay(50);
  }
}

