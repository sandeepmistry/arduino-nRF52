/*
  Software serial multiple serial test

 Receives from the hardware serial, sends to software serial.
 Receives from software serial, sends to hardware serial.

 The circuit:
 * RX is digital pin 9 (connect to TX of other device)
 * TX is digital pin 10 (connect to RX of other device)
 
 This example code is in the public domain.

 */

#include <SoftwareSerial.h>

SoftwareSerial mySerial(9, 10); // RX, TX


void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  Serial.println("Goodnight moon!");
 
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  mySerial.println("Hello, world?");
}

void loop() // run over and over//
{
  if (mySerial.available()) 
    Serial.write(mySerial.read());

  if (Serial.available())
    mySerial.write(Serial.read());
}
