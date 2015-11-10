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

#define SS_PIN 10
#define RST_PIN 9
#define TFT_DC 5
#define TFT_CS 7
#define TFT_MOSI 4
#define TFT_CLK 3
#define TFT_RST 6
#define TFT_MISO 2

RFID rfid(SS_PIN,RST_PIN);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);


int led = 7;
int power = 8; 
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

void setup(){

    Serial.begin(9600);
    SPI.begin();
    rfid.init();
    tft.begin();


    pinMode(led, OUTPUT);

    digitalWrite(led, LOW);
   
}

void loop(){
    
    if(rfid.isCard()){
    
        if(rfid.readCardSerial()){
            tft.setRotation(3);
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
//            tft.print(rfid.serNum[0]);
//            tft.print(" ");
//            tft.print(rfid.serNum[1]);
//            tft.print(" ");
//            tft.print(rfid.serNum[2]);
//            tft.print(" ");
//            tft.print(rfid.serNum[3]);
//            tft.print(" ");
//            tft.print(rfid.serNum[4]);
//            tft.println("");
            
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
           digitalWrite(led, HIGH); 
           delay(1000);
           digitalWrite(led, LOW);
           digitalWrite(power, HIGH);
           delay(1000);
           digitalWrite(power, LOW);
           
      } else if(scott_access) {
          Serial.println("Welcome Scott!");
          tft.println("Hey Scott!");
          scott_access = false;
           digitalWrite(led, HIGH); 
           delay(1000);
           digitalWrite(led, LOW);
           digitalWrite(power, HIGH);
           delay(1000);
           digitalWrite(power, LOW);
      } else {
           Serial.println("Not allowed!"); 
           digitalWrite(led, HIGH);
           delay(500);
           digitalWrite(led, LOW); 
           delay(500);
           digitalWrite(led, HIGH);
           delay(500);
           digitalWrite(led, LOW);         
       }        
    }
    
    
    
    rfid.halt();

}


