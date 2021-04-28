#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const long frequency = 920E6;  // LoRa Frequency

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;        // LoRa radio reset
const int irqPin = 2;          // change for your board; must be a hardware interrupt pin

// mems
#include <MPU9250_asukiaaa.h>
MPU9250_asukiaaa mySensor;

//mems

//gps

#include <TinyGPS++.h>
#include <SoftwareSerial.h>

//-------gps<
int RXPin = 0;
int TXPin = 1;
int GPSBaud = 9600;

String la="";
String lo="";
String sped="";

        
TinyGPSPlus gps;
SoftwareSerial gpsSerial(RXPin, TXPin);

int id_user = 1;
int id_car= 2;
int btn_empty=0;
int btn_filled= 0;
int btn_loading= 0;
int btn_trash= 0;
double latitude= -7.381100;
double longitude= 109.92456;
int xgyro;
int ygyro;
int speeds= 40;
int temp= 0;
int cycle= 1;


void setup() {
//  Serial.begin(9600);                   // initialize serial
   Serial.begin(115200);
    gpsSerial.begin(GPSBaud); //gps
  while (!Serial);
  mySensor.beginGyro();
  LoRa.setPins(csPin, resetPin, irqPin);

  if (!LoRa.begin(frequency)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
 if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
    }
     LoRa.setSpreadingFactor(12);           // ranges from 6-12,default 7 see API docs
  // LoRa.setTxPower(20);
   LoRa.setSignalBandwidth(200E3);
   LoRa.setCodingRate4(5);
  Serial.println("LoRa init succeeded.");
  Serial.println();
  Serial.println("LoRa SIAP");
  Serial.println();

  LoRa.onReceive(onReceive);
  LoRa.onTxDone(onTxDone);
  LoRa_rxMode();
}

void loop() {

  String message;
  message = String(id_user)
      + "," + String(id_car) 
      + "," +String(btn_empty)
      + "," +String(btn_filled)
      + "," +String(btn_loading)
      + "," +String(btn_trash)
      + "," +la
      + "," +lo
      + "," +String(xgyro)
      + "," +String(ygyro)
      + "," +sped
      + "," +String(temp)
      + "," +String(cycle);    

//data mems
if (mySensor.gyroUpdate() == 0) {
    xgyro = abs(mySensor.gyroX());
    ygyro = abs(mySensor.gyroY());
    //  Serial.println("gyroZ: " + String(gZ));
  } 
//  =====================data gps
  while (gpsSerial.available() > 0){
    if (gps.encode(gpsSerial.read())){
      if (gps.location.isValid())
      { 
        la = String(gps.location.lat(),6);
        lo = String(gps.location.lng(),6);
        sped = String(gps.speed.kmph(),3);
        Serial.println(message);
      }
    }
  }     

//  =====================send message
 if (runEvery(600)) { // repeat every 1000 millis
//    message += millis();

    LoRa_sendMessage(message); // send a message
    
    display.clearDisplay();
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0,0);             // Start at top-left corne
    display.println(message);
    display.display();
//  delay(100);
//    Serial.print(message);
//    Serial.println(" -->Send Message!");
//    
  }
}

void LoRa_rxMode(){
  LoRa.enableInvertIQ();                // active invert I and Q signals
  LoRa.receive();                       // set receive mode
}

void LoRa_txMode(){
  LoRa.idle();                          // set standby mode
  LoRa.disableInvertIQ();               // normal mode
}

void LoRa_sendMessage(String message) {
  LoRa_txMode();                        // set tx mode
  LoRa.beginPacket();                   // start packet
  LoRa.print(message);                  // add payload
  LoRa.endPacket(true);                 // finish packet and send it
}

void onReceive(int packetSize) {
  String message = "";

  while (LoRa.available()) {
    message += (char)LoRa.read();
  }

  Serial.print("Node Receive: ");
  Serial.println(message);
}

void onTxDone() {
  Serial.println("TxDone");
  LoRa_rxMode();
}

boolean runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}
