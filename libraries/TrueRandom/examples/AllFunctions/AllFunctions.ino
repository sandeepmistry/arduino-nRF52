/*
 * A magic 8 ball.
 *
 * Press the reset button to see into the future.
 *
 * View the answer to your question in the Serial Monitor, at 19200 baud.
 *
 * Press the Arduino reset button to ask another question.
 *
 */

#include <TrueRandom.h>

char array[10];
int arrayLength = 10;

uint8_t macAddress[6]; // MAC addresses are always 6 bytes long
uint8_t uuidNumber[16]; // UUIDs are always 16 bytes long

void printHex(char number) {
  // Print high hex digit
  Serial.print( "0123456789ABCDEF"[number / 16] );
  // Low hex digit
  Serial.print( "0123456789ABCDEF"[number & 15] );
}

void printMac(uint8_t* macAddress) {
  // Print a MAC address in the form
  //   12:23:34:45:56:67
  int i;
  for (i=0; i<6; i++) {
    printHex(macAddress[i]);
    if (i<5) Serial.print(":");
  }
}
void printUuid(uint8_t* uuidNumber) {
  // Print a UUID in the form
  //   12345678-1234-1234-1234-123456789ABC
  int i;
  for (i=0; i<16; i++) {
    if (i==4) Serial.print("-");
    if (i==6) Serial.print("-");
    if (i==8) Serial.print("-");
    if (i==10) Serial.print("-");
    printHex(uuidNumber[i]);
  }
}

void setup() {
  Serial.begin(9600);
  
  // TrueRandom.rand() returns a positive integer
  // in the range 0..32767
  
  Serial.print("A random int: ");
  Serial.println(TrueRandom.rand());
  
  // TrueRandom.random() returns a positive long integer
  // in the range 0..2,147,483,647

  Serial.print("A random long int: ");
  Serial.println(TrueRandom.random());

  // TrueRandom.random(n) returns a positive long integer
  // in the range 0 .. (n-1)
  
  Serial.print("A random number between 0 and 999: ");
  Serial.println(TrueRandom.random(1000));

  // TrueRandom.random(a,b) returns a positive long integer
  // in the range a .. (b-1).
  // b must be larger than a.

  Serial.print("A random number between 1000 and 9999: ");
  Serial.println(TrueRandom.random(1000,10000));

  // TrueRandom.randomBit() returns a single random bit
  // It returns either 1 or 0.
  Serial.print("A random bit: ");
  Serial.println(TrueRandom.randomBit());
  
  // TrueRandom.randomByte() returns a single random byte
  // It returns an 8 bit char (byte) in the range -128 to 127.
  Serial.print("A random byte: ");
  Serial.println(TrueRandom.randomByte(),DEC);

  // Zero an array
  for(int i=0; i<arrayLength; i++) array[i]=0;

  Serial.print("An empty array of bytes: ");
  for(int i=0; i<arrayLength; i++) {
    Serial.print(array[i],DEC);
  }
  Serial.println();
  
  // Now randomise the array
  // TrueRandom.memfill(startAddress,length) fills memory with 
  // random bytes.
  TrueRandom.memfill(array,arrayLength);
  
  Serial.print("A randomised array of bytes: ");
  for(int i=0; i<arrayLength; i++) {
    Serial.print(array[i],DEC);
  }
  Serial.println();
  
  // TrueRandom.mac(location) fills a 6 byte buffer with a
  // random MAC address.
  TrueRandom.mac(macAddress);
  Serial.print("Here is a random MAC address: ");
  printMac(macAddress);
  Serial.println();
  
  // TrueRandom.uuid(location) fills a 16 byte buffer with a
  // random UUID.
  TrueRandom.uuid(uuidNumber);
  Serial.print("Here is a random UUID: ");
  printUuid(uuidNumber);
  Serial.println();

}

void loop() {
  
}
