// This example modifies the Ethernet Client example to use
// a randomly generated MAC address
//
// This has the advantage that no MAC addresses will clash, even if
// every device uses exactly the same code on its Arduino.
//

// This example is identical to the Ethernet/Client example
// with three changes to utilise TrueRandom.

#include <Ethernet.h>

//
// Change 1/3: Include TrueRandom library
#include <TrueRandom.h>
//
//

//
// Change 2/3: Include TrueRandom library
// TrueRandom sets this, so no default is needed here.
byte mac[6];
//
//

byte ip[] = { 10, 0, 0, 177 };
byte server[] = { 64, 233, 187, 99 }; // Google

Client client(server, 80);

void setup()
{
  //
  // Change 3/3: Set a random MAC address using TrueRandom
  TrueRandom.mac(mac);
  //
  //
  
  
  Ethernet.begin(mac, ip);
  Serial.begin(9600);
  
  delay(1000);
  
  Serial.println("connecting...");
  
  if (client.connect()) {
    Serial.println("connected");
    client.println("GET /search?q=arduino HTTP/1.0");
    client.println();
  } else {
    Serial.println("connection failed");
  }
}

void loop()
{
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    for(;;)
      ;
  }
}