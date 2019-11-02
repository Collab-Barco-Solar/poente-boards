// bibliotecas

#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <Adafruit_ADS1015.h>
#include <multiplexer.h>


//pinos

#define SCK     5     // GPIO5  -- LoRa SX1278's SCK
#define MISO    19    // GPIO19 -- LoRa SX1278's MISnO
#define MOSI    27    // GPIO27 -- LoRa SX1278's MOSI
#define SS      18    // GPIO18 -- LoRa SX1278's CS
#define RST     14    // GPIO14 -- LoRa SX1278's RESET
#define DI0     26    // GPIO26 -- LoRa SX1278's IRQ(Interrupt Request)

#define RXPin    15  // Neo6M RX pin
#define TXPin    12  // Neo6M TX pin
#define GPSBaud 9600 // Neo6M Baud rate

// Mux defines

#define S0 32 //MUX control pins
#define S1 14
#define S2 13
#define S3 0
#define MUX_SIG 4 //MUX input Pin

// polynomial coefficients

const float p5 = 17.5938359490609;
const float p4 = 1.21697808640912;
const float p3 = -0.000168313273959029;
const float p2 =  7.69758075646963e-08;
const float p1 = -1.21956845031952e-11;

// MUX input pins

 const int DMSMux = 0;
 const int ReverseMux = 1;
 const int OnOffMux = 2;
 const int CruiseMux = 3;
 const int PotMux = 4;
 const int CBMux = 5;
 const int BBMux = 6;
 const int BEMux = 7;
 const int CEMux = 8;
 const int ACS1Mux = 9;
 const int BatAMux = 10;
 const int PhotoMux = 11;
 const int BatBankMux = 12;
 const int ACS2Mux = 13;

//#define PIN_ACS 33  //??
//#define PIN_BAT_AUX 13
//#define PIN_POTENCIOMETER 14
//#define PIN_BAT_BANK_V 2
//#define PIN_BAT_BANK_C 21
//#define PIN_MOTOR 22


//defines

#define FREQ  915E6   // Operating LoRa frequency
#define SF      7     // Operating LoRa Spread Factor
#define BAND  125E3   // Operating LoRa Bandwidth
#define TXPOWER 20    // Operating LoRa Transmition Power
#define BAUD 2000000  // BAUD serial rate


#define MOTOR    0x48  // Address of the ADC measuring the motor current
#define BATTERY  0x49  // Address of the ADC on the solar panel modules


//Conversion Ratios (Voltage Divider)

const float DT1_RATIO = 19.333333333f; // atualizar com valores medidos dos resistores
const float DT2_RATIO = 4.970588235f; // atualizar com valores medidos dos resistores
const float DT3_RATIO = 28.5f; // atualizar com valores medidos dos resistores
const float DT4_RATIO = 1.564102564f; // atualizar com valores medidos de resistores
const float DT5_RATIO = 1.552971576f; // VOLTAGE DIVIDER FOR POTENTIOMETER

//objetos

String rssi = "RSSI --";
String packSize = "--";
String packet;

TinyGPSPlus gps;      // GPS object
HardwareSerial ss(2); // Hardware Serial comunication object

Adafruit_ADS1115 ads_motor(MOTOR);
Adafruit_ADS1115 ads_battery(BATTERY);

  //   Cheatsheet                                                     ADS1115
  //                                                                  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit =   0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit =   0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit =   0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit =   0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit =   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit =   0.0078125mV

//functions

float polyfit(float value)
{
  float fittedReading;  
  fittedReading = (value*value*value*value*p1)+(value*value*value*p2)+(value*value*p3) + (value*p4) + p5;
    
  return fittedReading;
}

void SetMuxChannel ( int channel ) {
  
  digitalWrite(S0, channel>>0&1);
  digitalWrite(S1, channel>>1&1);
  digitalWrite(S2, channel>>2&1);
  digitalWrite(S3, channel>>3&1); 
  delay(10);

}

double LatitudeGPS( ){

  return gps.location.lat();
}

double LongitudeGPS( ){
  
  return gps.location.lng();
}

char CSV_Separator(){
  return (' ; ');
}

float MotorCurrentRead(){
  ads_motor.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit =   0.0078125mV
  
  return ads_motor.readADC_Differential_0_1();
}

float BatteryCurrentRead(){
  ads_battery.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit =   0.125mV
  
  return ads_battery.readADC_Differential_0_1();
}

float PotentiometerRead(){
  SetMuxChannel(PotMux);
  float readVoltage = (polyfit(analogRead(MUX_SIG)) * 3.3) / 4095;  //if analog read == 4095, it is reading 3.3V, so convert the reading from bits to Voltage
  
  return readVoltage * DT5_RATIO * 2; //Multiply by the ratio of the voltage divider to find the true voltage value
}

boolean DmsRead(){
  multiplexer.setControlPortStatus(DMSMux); 
  return (!(analogRead(multiplexer.getSignalPort) < 300)); //If it's less than 300 bits, then consider the button as closed and dms on
}

boolean ButtonReverseRead(){
  multiplexer.setControlPortStatus(ReverseMux);
  return (!(analogRead(multiplexer.getSignalPort) < 300)); //If it's less than 300 bits, then consider the button as closed
}

boolean ButtonMotorRead(){
  multiplexer.setControlPortStatus(OnOffMux);
  return (!(analogRead(MUX_SIG) < 300)); //If it's less than 300 bits, then consider the button as closed
}

boolean ButtonCruiseRead(){
  multiplexer.setControlPortStatus(CruiseMux);
  return (!(analogRead(MUX_SIG) < 300)); //If it's less than 300 bits, then consider the button as closed
}

float CoolerLeftRead(){
  SetMuxChannel(CBMux);
  float readVoltage = (analogRead(MUX_SIG) * 3.3) / 4095;  //if analog read == 4095, it is reading 3.3V, so convert the reading from bits to Voltage
  
  return readVoltage * DT2_RATIO; //Multiply by the ratio of the voltage divider to find the true voltage value
}

float CoolerRightRead(){
  SetMuxChannel(CEMux);
  float readVoltage = (analogRead(MUX_SIG) * 3.3) / 4095;  //if analog read == 4095, it is reading 3.3V, so convert the reading from bits to Voltage
  
  return readVoltage * DT2_RATIO; //Multiply by the ratio of the voltage divider to find the true voltage value
}
float AuxiliaryBatteryRead(){
  SetMuxChannel(BatAMux);
  float readVoltage = (polyfit(analogRead(MUX_SIG)) * 3.3) / 4095;  //if analog read == 4095, it is reading 3.3V, so convert the reading from bits to Voltage
  
  return readVoltage * DT2_RATIO; //Multiply by the ratio of the voltage divider to find the true voltage value
}
float AuxiliaryBatteryCurrentRead(){
  SetMuxChannel(ACS1Mux);
  float readVoltage = (polyfit(analogRead(MUX_SIG)) * 3.3) / 4095;  //if analog read == 4095, it is reading 3.3V, so convert the reading from bits to Voltage
  
  return readVoltage * DT4_RATIO; //Multiply by the ratio of the voltage divider to find the true voltage value
}

float PhotovoltaicModulesRead(){
  SetMuxChannel(PhotoMux);
  float readVoltage = (polyfit(analogRead(MUX_SIG)) * 3.3) / 4095;  //if analog read == 4095, it is reading 3.3V, so convert the reading from bits to Voltage
  
  return readVoltage * DT3_RATIO; //Multiply by the ratio of the voltage divider to find the true voltage value
}

float BatteryBankRead(){
  SetMuxChannel(BatBankMux);
  float readVoltage = (polyfit(analogRead(MUX_SIG)) * 3.3) / 4095;  //if analog read == 4095, it is reading 3.3V, so convert the reading from bits to Voltage
  
  return readVoltage * DT1_RATIO; //Multiply by the ratio of the voltage divider to find the true voltage value
}

boolean LeftPumpRead(){
  SetMuxChannel(BBMux);
  
  return (!(analogRead(MUX_SIG) < 300)); //If it's less than 300 bits, then consider the button as closed
}

boolean RightPumpRead(){
  SetMuxChannel(BEMux);
    
  return (!(analogRead(MUX_SIG) < 300)); //If it's less than 300 bits, then consider the button as closed
}


//setup

void setup() {
  
  Serial.begin(BAUD);
  ss.begin(GPSBaud, SERIAL_8N1, TXPin, RXPin); // GPS serial communication

  //pinMode(PIN_POTENCIOMETER, INPUT);
  //MUX pins
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(MUX_SIG, INPUT);

  digitalWrite(S0, LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);

  multiplexer.signalPort(MUX_SIG);
  multiplexer.setPinSelectors(S0,S1,S2,S3);

  while (!Serial);
  Serial.println();
  Serial.println("Boat sender");
  
  SPI.begin(SCK,MISO,MOSI,SS); // LoRa SPI communication
  LoRa.setPins(SS,RST,DI0);
  
  if (!LoRa.begin(FREQ)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  LoRa.setSpreadingFactor(SF);
  LoRa.setSignalBandwidth(BAND);
  LoRa.setTxPower(TXPOWER);
  LoRa.enableCrc();

  
  //LoRa.onReceive(cbk);
  //  LoRa.receive();
  Serial.println("Initialization: ok");
 
  delay(1500);
}

void loop() {
  
  // Create and send packet

  // Update GPS
  gps.encode(ss.read());
  
  LoRa.beginPacket();
  // Write GPS Latitude
  LoRa.print("GPS.LAT: "); 
  LoRa.print(LatitudeGPS());
  LoRa.print(CSV_Separator());
  // Write GPS Longitude
  LoRa.print("GPS.LONG: "); 
  LoRa.print(LongitudeGPS());
  LoRa.print(CSV_Separator());

  //MUX readings
    
  //Write DMS reading
  LoRa.print("DMS: ");
  Serial.print("DMS: ");
  Serial.println(DmsRead());
  LoRa.print(DmsRead());
  LoRa.print(CSV_Separator());
  //Write reverse button (ré)
  LoRa.print("REV: ");
  LoRa.print(ButtonReverseRead()); //For all the buttons -> "true" means closed and "false" means open
  LoRa.print(CSV_Separator());
  //Write motor button state (on/off)
  LoRa.print("MOTOR: ");
  LoRa.print(ButtonMotorRead()); //For all the buttons -> "true" means closed and "false" means open
  LoRa.print(CSV_Separator());
  //Write cruise button state
  LoRa.print("Crui: ");
  LoRa.print(ButtonCruiseRead()); //For all the buttons -> "true" means closed and "false" means open
  LoRa.print(CSV_Separator());
  // Write Current input in the batteryes voltage  (using the ADS1115)
  LoRa.print("Bat.C: ");
  LoRa.print(BatteryCurrentRead());
  LoRa.print(CSV_Separator());
  // Write the potentiometer state that controls the motor 
  LoRa.print("Pot: ");
  Serial.print("Pot: ");
  Serial.println(PotentiometerRead());
  LoRa.print(PotentiometerRead());
  LoRa.print(CSV_Separator());
  // Write Solar Modules voltage
  LoRa.print("Mod: ");  
  LoRa.print(PhotovoltaicModulesRead());
  LoRa.print(CSV_Separator());
  // Write Auxiliary Battery voltage 
  LoRa.print("Bat.A: ");   
  LoRa.print(AuxiliaryBatteryRead());
  LoRa.print(CSV_Separator());
  // Write Auxiliary Battery current (using acs712) 
  LoRa.print("Bat.A.C.: "); 
  LoRa.print(AuxiliaryBatteryCurrentRead());
  LoRa.print(CSV_Separator());
  // Write Battery Bank voltage 
  LoRa.print("Bat.B.: ");  
  LoRa.print(BatteryBankRead());
  LoRa.print(CSV_Separator());
  //Write left cooler tension
  LoRa.print("CB: ");  
  LoRa.print(CoolerLeftRead());
  LoRa.print(CSV_Separator());
  //Write right cooler tension
  LoRa.print("CE: ");  
  LoRa.print(CoolerRightRead());
  LoRa.print(CSV_Separator());
  // Write left pump state
  LoRa.print("PB: ");  
  LoRa.print(LeftPumpRead());
  LoRa.print(CSV_Separator());
  // Write right pump state
  LoRa.print("PE: ");  
  LoRa.print(RightPumpRead());
  LoRa.print(CSV_Separator());

  // Write Motor Current reading (motor current)
  LoRa.print("MC: ");
  LoRa.print(MotorCurrentRead());
  LoRa.print(CSV_Separator());
  // Write Battery CurrentIn (probably using the ADS1115)
  LoRa.print("Bat.BC: ");
  LoRa.print(BatteryCurrentRead());
  LoRa.print(CSV_Separator());
    
  //LoRa.print(counter);
  LoRa.endPacket();

  delay(10);
}
