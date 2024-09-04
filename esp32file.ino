#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <LoRa.h>

#define LORA_SCK    5
#define LORA_MISO   19
#define LORA_MOSI   27
#define LORA_SS     18
#define LORA_RST    14
#define LORA_DIO0   26

TinyGPSPlus gps;
// for gps
HardwareSerial mySerial(1);

#define DEVICE_ID "Device1"

void setup() {
  Serial.begin(115200);

  //serial communication for GPS
  mySerial.begin(9600, SERIAL_8N1, 16, 17); // RX, TX

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(915E6)) { // Frequency (915 MHz in this case)
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  
  Serial.println("LoRa Initializing OK!");
}

void loop() {
  while (mySerial.available() > 0) {
    gps.encode(mySerial.read());

    if (gps.location.isUpdated()) {
      String lat = String(gps.location.lat(), 6);
      String lng = String(gps.location.lng(), 6);

      String message = String(DEVICE_ID) + ": LAT: " + lat + ", LNG: " + lng;

      LoRa.beginPacket();
      LoRa.print(message);
      LoRa.endPacket();
      
      Serial.println("Sent: " + message);
    }
  }

  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    while (LoRa.available()) {
      String receivedMessage = LoRa.readString();
      Serial.println("Received: " + receivedMessage);
    }
  }
}
