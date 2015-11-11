#include <RFID.h>

/*
* Read a card using a mfrc522 reader on your SPI interface
* Pin layout should be as follows (on Arduino Uno):
* MOSI: Pin 11 / ICSP-4
* MISO: Pin 12 / ICSP-1
* SCK: Pin 13 / ISCP-3
* SS/SDA: Pin 10
* RST: Pin 9
*/

#include <SPI.h>
#include <RFID.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SoftwareSerial.h>

#define SS_PIN 10
#define RST_PIN 9
#define TFT_DC 5
#define TFT_CS 15
#define TFT_MOSI 4
#define TFT_CLK 3
#define TFT_RST 6
#define TFT_MISO 15
#define BT_TX 7       //Arduino D7 -> BT TX
#define BT_RX 8       //Arduino D8 -> BT RX
#define led 13
#define ext_led 14

RFID rfid(SS_PIN,RST_PIN);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
SoftwareSerial bluetooth(BT_TX, BT_RX);


int serNum[5];
int cards[][5] = {
  {131,222,128,0,221},
  {131,222,128,0,222},
  {131,222,128,0,223},
  {131,222,128,0,224}
  
};
int theo[][5] = {
  {86,94,109,143,234}
};
int scott[][5] = {
  {156,65,247,175,133}
};

bool access = false;
bool theo_access = false;
bool scott_access = false;

// Buffer for reading in bluetooth text
String bt_buffer;

void setup(){

    Serial.begin(9600);               // starts serial monitor at 9600bps
    SPI.begin();                      // starts SPI
    rfid.init();                      // initialises RFID library
    bluetooth.begin(115200);          // starts bluetooth at 115200
    delay(100);                       // wait for bluetooth module to sed back command
    bluetooth.println("U,9600,N");    // temporarily change the bps to 9600, no parity
    bluetooth.begin(9600);            // starts bluetooth at 9600bps
    tft.begin();                      // starts TFT screen
    tft.fillScreen(ILI9341_BLACK);    // fills screen with black
    tft.setRotation(3);               // sets screen orientation to landscape with pins on left
    tft.setCursor(0, 75);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(4);

    pinMode(ext_led, OUTPUT);
    digitalWrite(ext_led, LOW);

}

void loop(){

    // Read bluetooth text and print it to screen
    
    if(bluetooth.available())  // If the bluetooth sent any characters
    {
      char char_in = (char)bluetooth.read();
      if(char_in != '~'){ // Before end char
        bt_buffer = bt_buffer + char_in;
      } else { // Print to screen at end char
        tft.setCursor(5,75);
        tft.fillScreen(ILI9341_BLACK);
        tft.println(bt_buffer);
        bt_buffer = "";
      }
      
    }

    // RFID reading
    if(rfid.isCard()){
        if(rfid.readCardSerial()){
            tft.fillScreen(ILI9341_BLACK);
            tft.setCursor(0, 75);
            tft.setTextColor(ILI9341_WHITE);
            tft.setTextSize(4);
            Serial.print(rfid.serNum[0]);
            Serial.print(" ");
            Serial.print(rfid.serNum[1]);
            Serial.print(" ");
            Serial.print(rfid.serNum[2]);
            Serial.print(" ");
            Serial.print(rfid.serNum[3]);
            Serial.print(" ");
            Serial.print(rfid.serNum[4]);
            Serial.println("");
            tft.print(rfid.serNum[0]);
            tft.print(" ");
            tft.print(rfid.serNum[1]);
            tft.print(" ");
            tft.print(rfid.serNum[2]);
            tft.print(" ");
            tft.print(rfid.serNum[3]);
            tft.print(" ");
            tft.print(rfid.serNum[4]);
            tft.println("");
            
            for(int x = 0; x < sizeof(theo); x++){
              for(int i = 0; i < sizeof(rfid.serNum); i++ ){
                  if(rfid.serNum[i] != theo[x][i]) {
                      theo_access = false;
                      break;
                  } else {
                      theo_access = true;
                  }
              }
              if(theo_access) break;
            }
            for(int x = 0; x < sizeof(scott); x++){
              for(int i = 0; i < sizeof(rfid.serNum); i++ ){
                  if(rfid.serNum[i] != scott[x][i]) {
                      scott_access = false;
                      break;
                  } else {
                      scott_access = true;
                  }
              }
              if(scott_access) break;
            }
           
        }

       if(theo_access){
          Serial.println("Welcome Theo!");
          tft.println("Hey Theo!");
          theo_access = false;
           digitalWrite(ext_led, HIGH); 
           delay(1000);
           digitalWrite(ext_led, LOW);           
      } else if(scott_access) {
          Serial.println("Welcome Scott!");
          tft.println("Hey Scott!");
          scott_access = false;
           digitalWrite(ext_led, HIGH); 
           delay(1000);
           digitalWrite(ext_led, LOW);
      } else {
           Serial.println("Not allowed!");
           tft.println("ACCESS DENIED.");
           digitalWrite(ext_led, HIGH);
           delay(500);
           digitalWrite(ext_led, LOW); 
           delay(500);
           digitalWrite(ext_led, HIGH);
           delay(500);
           digitalWrite(ext_led, LOW);         
       }        
    }
    
    
    
    rfid.halt();

}


