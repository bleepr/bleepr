

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
#include <Adafruit_NeoPixel.h>

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
#define speaker 2
#define LEDStrip A6

RFID rfid(SS_PIN,RST_PIN);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
SoftwareSerial bluetooth(BT_TX, BT_RX);

//LEDs setup
Adafruit_NeoPixel strip = Adafruit_NeoPixel(30, LEDStrip, NEO_GRB + NEO_KHZ800);


int serNum[5];
int test[][5] = {
  {131,222,128,0,221}
};

bool access = false;

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
    tft.setRotation(3);               // sets screen orientation to landscape with pins on left
    tft.setTextSize(2);
    loginDisplay();
    
    //LED strip initialise
    strip.begin();
    strip.show();
    
    colorWipe(strip.Color(0, 0, 100), 10); //Blue init
    

    pinMode(ext_led, OUTPUT);
    digitalWrite(ext_led, LOW);
    pinMode(speaker, OUTPUT);

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
            tone(speaker, 800);
            delay(120);
            tone(speaker, 1000);
            delay(40);
            noTone(speaker);
            tft.fillScreen(ILI9341_BLACK);
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
            
            for(int x = 0; x < sizeof(test); x++){
              for(int i = 0; i < sizeof(rfid.serNum); i++ ){
                  if(rfid.serNum[i] != test[x][i]) {
                      access = false;
                      break;
                  } else {
                      access = true;
                  }
              }
              if(access) break;
            }           
        }

       if(access){
          tft.setCursor(0, 75);
          tft.setTextColor(ILI9341_WHITE);
          tft.setTextSize(4);
          Serial.println("Welcome TEST!");
          tft.println("Hey TEST!");
          colorWipe(strip.Color(0, 100, 0), 10);
          access = false;
          homeDisplay();
        } else {
           Serial.println("Not allowed!");
           tft.println("ACCESS DENIED.");
           colorWipe(strip.Color(255, 0, 0), 10);
           tone(speaker, 400);
           delay(150);
           noTone(speaker);
           delay(100);
           tone(speaker, 400);
           delay(150);
           noTone(speaker);
           delay(100);
           tone(speaker, 400);
           delay(150);
           noTone(speaker);
           delay(100);
           colorWipe(strip.Color(0, 0, 100), 10);
           loginDisplay();
       }        
    }
    
    
    
    rfid.halt();

}

void loginDisplay(){
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setCursor(5,20);
  tft.println("Login by placing your card on the screen.");
}

void homeDisplay(){
  tft.fillRect(0,0,160,120,ILI9341_LIGHTGREY);
  tft.fillRect(160,0,160,120,ILI9341_DARKGREY);
  tft.fillRect(0,120,160,120,ILI9341_DARKGREY);
  tft.fillRect(160,120,160,120,ILI9341_LIGHTGREY);
  tft.setTextSize(2);
  tft.setCursor(5,60);
  tft.println("Call waiter");
  tft.setCursor(165,60);
  tft.println("Ask for bill");
  tft.setCursor(5,180);
  tft.println("Food status");
  tft.setCursor(165,180);
  tft.println("Leave table");
  
}



//LED Functions

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

