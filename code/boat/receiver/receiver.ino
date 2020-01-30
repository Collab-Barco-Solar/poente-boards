#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>  

#define SCK     5     // GPIO5  -- SX1278's SCK
#define MISO    19    // GPIO19 -- SX1278's MISO
#define MOSI    27    // GPIO27 -- SX1278's MOSI
#define SS      18    // GPIO18 -- SX1278's CS
#define RST     14    // GPIO14 -- SX1278's RESET
#define DI0     26    // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define FREQ  915E6   // Operating LoRa frequency
#define SF      7     // Operating LoRa Spread Factor
#define SYNCWORD 0x2D // LoRa Sync Word (default is 0x12)
#define BAND  125E3   // Operating LoRa Bandwidth
#define BAUD 2000000  // BAUD serial rate

String rssi = "RSSI --";
String packSize = "--";
String packet ;


void cbk(int packetSize) {
  packet ="";
  for (int i = 0; i < packetSize; i++) { packet += (char) LoRa.read(); }
  rssi = String(LoRa.packetRssi(), DEC) ;
  
  Serial.print("Sinal RSSI: " + rssi );
  Serial.println(" Mensagem: " + packet);
}

void setup() {
  
  Serial.begin(BAUD);
  while (!Serial);
  Serial.println();
  Serial.println("LoRa Receiver Callback");
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);
  LoRa.setSyncWord(SYNCWORD);  
  if (!LoRa.begin(FREQ)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  LoRa.setSpreadingFactor(SF);
  LoRa.setSignalBandwidth(BAND);
  LoRa.enableCrc();
  
  //LoRa.onReceive(cbk);
  LoRa.receive();
  Serial.println("init ok");

  delay(500);

  Serial.print("Latitude; Longitude ; Speed(knots); DMS ; Reverse ; Motor ; Cruise ; Battery +Current ; Potentiometer ; PV Modules ; Aux Battery ; Main Battery ; LC ; RC ; Left Pump ; Right Pump ; Motor Current");
  
}

void loop() {
  
  int packetSize = LoRa.parsePacket();
  
  if (packetSize) { 
    cbk(packetSize);
    
    //Serial.println(  );
  }

  delay(10);
  
}
