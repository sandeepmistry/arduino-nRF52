#include <TrueRandom.h>

unsigned long startTime;
int i;

void setup() {
  Serial.begin(9600);
  
  Serial.println("TrueRandom benchmark");
  Serial.println("--------------------");
  Serial.println();

  Serial.print("Arduino clock speed: ");
  Serial.print(F_CPU/1000000);
  Serial.println("MHz");

  Serial.print("randomBit(): ");
  startTime = millis();
  TrueRandom.randomBit();
  Serial.print(millis() - startTime);
  Serial.println("ms");
  
  Serial.print("randomByte(): ");
  startTime = millis();
  TrueRandom.randomByte();
  Serial.print(millis() - startTime);
  Serial.println("ms");
  
  Serial.print("random(100): ");
  startTime = millis();
  TrueRandom.random(100);
  Serial.print(millis() - startTime);
  Serial.println("ms");
  
  Serial.print("random(65536): ");
  startTime = millis();
  TrueRandom.random(65536);
  Serial.print(millis() - startTime);
  Serial.println("ms");
  
  Serial.print("random(65537): ");
  startTime = millis();
  TrueRandom.random(65537);
  Serial.print(millis() - startTime);
  Serial.println("ms");
}
void loop() {
}