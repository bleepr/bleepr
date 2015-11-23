

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
#define TFT_DC 4
#define TFT_CS 15
#define TFT_MOSI 5
#define TFT_CLK 6
#define TFT_RST 3
#define TFT_MISO 15
#define BT_TX 8       //Arduino D8 -> BT TX
#define BT_RX 7       //Arduino D7 -> BT RX
#define speaker 2
#define LEDStrip 18
#define BTN0 A0
#define BTN1 A1
#define BTN2 A2
#define BTN3 A3

RFID rfid(SS_PIN,RST_PIN);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
SoftwareSerial bluetooth(BT_TX, BT_RX);

//State to be displayed on screen
int display_state = 0;
//0-Idle
//1-Main Menu

//Helps with the animations
int screen_drawn = 0;
unsigned long animation_time = millis();

//LEDs setup
Adafruit_NeoPixel strip = Adafruit_NeoPixel(30, LEDStrip, NEO_GRB + NEO_KHZ800);


//RFID variables
boolean scanned_card = false;

int serNum[5];
int test[][5] = {
  {131,222,128,0,221}
};

bool access = false;

// Buffer for reading in bluetooth text
String bt_buffer;
String last_bt = "";
boolean available_bt = false;



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
    

    pinMode(speaker, OUTPUT);
    tone(speaker,1000);
    delay(100);
    noTone(speaker);

}

void loop(){

    //Display switch
    switch(display_state) {
      case 0:
      
      loginDisplay();
      
      
        if(scanned_card) {
          if(available_bt) {
            
            //screen_drawn = 2; //Icky, will keep printing please wait until card is recieved from server
            //We can fix this later
            
            scanned_card = false;
            if (read_bt() == "access1") { //Card is good
              display_state = 1;
              screen_drawn = 0;
            }
            else {
              tft.println("Sorry, no access");
              delay(1000);
              screen_drawn = 0;
            }
          }
        }
        break;
        
      case 1: //Main Display
          homeDisplay();
          
          //If buttons pressed move to corresponding screen
        break;
        
      default:
        break;
    }
              

    // Read bluetooth text and print it to screen
    
    if(bluetooth.available())  // If the bluetooth sent any characters
    {
      char char_in = (char)bluetooth.read();
      if(char_in != '\0'){ // Before end char
        
        bt_buffer = bt_buffer + char_in;
      } else { // Print to screen at end char
        last_bt = bt_buffer;
        available_bt = true;
        bt_buffer = "";
      }
      
    }
    

    // Read buttons and send to bluetooth
//    if(digitalRead(BTN0)==HIGH){
//      bluetooth.print("BTN0");
//    } else if(digitalRead(BTN1)==HIGH) {//      bluetooth.print("BTN1");
//    } else if(digitalRead(BTN2)==HIGH) {
//      bluetooth.print("BTN2");
//    } else if(digitalRead(BTN3)==HIGH) {
//      bluetooth.print("BTN3");
//    }

    // RFID reading
    if(rfid.isCard() && !scanned_card){
        if(rfid.readCardSerial()){
            String serNumStr = "";
            for(int i=0; i<5; i++){
              serNumStr = serNumStr + String(rfid.serNum[i]) + " ";              
            }
            bluetooth.print(serNumStr);
            
            scanned_card = true;
            
            screen_drawn = 2;
            
            tone(speaker, 800);
            delay(120);
            tone(speaker, 1000);
            delay(40);
            noTone(speaker);
                      
        }

//       if(access){
//          tft.setCursor(0, 75);
//          tft.setTextColor(ILI9341_WHITE);
//          tft.setTextSize(4);
//          Serial.println("Welcome TEST!");
//          tft.println("Hey TEST!");
//          colorWipe(strip.Color(0, 100, 0), 10);
//          access = false;
//          homeDisplay();
//        } else {
//           Serial.println("Not allowed!");
//           tft.println("ACCESS DENIED.");
//           colorWipe(strip.Color(255, 0, 0), 10);
//           tone(speaker, 400);
//           delay(150);
//           noTone(speaker);
//           delay(100);
//           tone(speaker, 400);
//           delay(150);
//           noTone(speaker);
//           delay(100);
//           tone(speaker, 400);
//           delay(150);
//           noTone(speaker);
//           delay(100);
//           colorWipe(strip.Color(0, 0, 100), 10);
//           loginDisplay();
//       }        
    }
    
    
    
    rfid.halt();

}

void loginDisplay(){
 
  switch(screen_drawn) {
    case 0: // Initialise
      tft.fillScreen(ILI9341_BLACK);
      tft.setTextSize(2);
      tft.setCursor(126,113); // centred text
      tft.println("bleepr");
      tft.drawCircle(160,120,60,ILI9341_RED); 
      
      screen_drawn = 1;
      animation_time = millis();
      break;
     case 1: //Animate (Currently janky and NEEDS to be fixed)
//      if(millis() - animation_time >= 20) {
//        tft.drawCircle(160,120,60,ILI9341_RED);  
//      }  
//      if(millis() - animation_time >= 200){
//        tft.drawCircle(160,120,60,ILI9341_BLACK);
//      }
//      if(millis() - animation_time >= 240){
//        tft.drawCircle(160,120,80,ILI9341_YELLOW);
//      }
//      if(millis() - animation_time >= 440){
//        tft.drawCircle(160,120,80,ILI9341_BLACK);
//      }
//      if(millis() - animation_time >= 460){
//        tft.drawCircle(160,120,100,ILI9341_GREEN);
//      }
//      if(millis() - animation_time >= 660){
//        tft.drawCircle(160,120,100,ILI9341_GREEN);
//      }
//      if(millis() - animation_time >= 700){
//        tft.drawCircle(160,120,100,ILI9341_BLACK);
//      }
//      if(millis() - animation_time >= 740){
//        animation_time = millis();
//      }
       
       break;
     case 2: //Please wait
       tft.setCursor(126,200); // centred text
       tft.println("Please wait");
       screen_drawn = 1;
  }
}


//Needs to be converted for new steup
void homeDisplay(){
  switch(screen_drawn) {
    case 0: //Setup
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
      
      screen_drawn = 1;
      break;
      
     case 1: //Loop
       break;
     
     default:
       break;
     
  }
  
  
  
}

//BT Functions
String read_bt() {
 if(available_bt) {
   available_bt = false;
   return last_bt;
 } else {
   return "";
 }
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

