

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
#include <pitches.h>

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
Adafruit_NeoPixel strip = Adafruit_NeoPixel(19, LEDStrip, NEO_GRB + NEO_KHZ800);


//RFID variables
boolean scanned_card = false;


bool access = false;

// Buffer for reading in bluetooth text
String bt_buffer;
String last_bt = "";
boolean available_bt = false;

//Order Status
String order_status = "unknown";

//dank
int memes = 0;

//timeouts
//unsigned long timeout = millis();

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

  pinMode(speaker, OUTPUT);
  tone(speaker,1000);
  delay(100);
  noTone(speaker);
 

}

void loop(){
  
    // RFID reading
  if(rfid.isCard()){
    if(rfid.readCardSerial()){
      String serNumStr = "";
      for(int i=0; i<5; i++){
        serNumStr = serNumStr + String(rfid.serNum[i], HEX) + " ";              
      }
      bluetooth.print("c," + serNumStr);

      scanned_card = true;

      screen_drawn = 8;

      tone(speaker, 800);
      delay(120);
      tone(speaker, 1000);
      delay(40);
      noTone(speaker);

    }       
  }

  rfid.halt();

  //Display switch
  switch(display_state) {
  case 0: //Login/Idle animated screen

    loginDisplay();

    if (digitalRead(BTN0)) { 
      tone(speaker,NOTE_C6);
      delay(100);
      noTone(speaker);
      if(memes < 3) {
        colorWipe((0, 0, 255), 1);
        memes++;
      }
      else if (memes == 6) {
        colorWipe((0, 0, 255), 1);
        memes++;
      }
      else {
        colorWipe((0, 0, 0), 1);
        memes = 0;
      }
    }
    if (digitalRead(BTN1)) {
      tone(speaker,NOTE_F6);
      delay(100);
      noTone(speaker);
      if(memes == 3) {
      colorWipe((0, 0, 255), 1);
        memes++;
      }
       else {
       colorWipe((0, 0, 0), 1);
         memes = 0;
       }
    }

    if (digitalRead(BTN2)) {
      tone(speaker,NOTE_DS6);
      delay(100);
      noTone(speaker);
      if(memes == 4) {
      colorWipe((0, 0, 255), 1);
        memes++;
      }
       else {
       colorWipe((0, 0, 0), 1);
         memes = 0;
       }
    }

    if (digitalRead(BTN3)) {
      tone(speaker,NOTE_AS5);
      delay(100);
      noTone(speaker);
      if(memes == 5) {
      colorWipe((0, 0, 255), 1);
        memes++;
      } else {
       colorWipe((0, 0, 0), 1);
         memes = 0;
      }
    }


    if(memes == 7) {
      memes = 0;
      darude();
    }



    if(digitalRead(BTN0) && digitalRead(BTN3)) { //Shorcut to main menu for debugging purposes
      scanned_card = false;
      display_state = 1;
      screen_drawn = 0;
    }



    if(scanned_card) {
      if(available_bt) {

        String access = read_bt();

        if (access == "access1") { //Card is good
          scanned_card = false;
          display_state = 1;
          screen_drawn = 0;
        }
        else {
          tft.setCursor(75,210);
          tft.println("Sorry," + access);
          scanned_card = false;
          delay(1000);
          screen_drawn = 0;
        }

      }
    }
    break;

  case 1: //Main Display
    homeDisplay();

    //If buttons pressed move to corresponding screen
    // Read buttons and send to bluetooth
    if (digitalRead(BTN0)) { //Bill
      tone(speaker,500);
      delay(100);
      noTone(speaker);
      display_state = 2;
      screen_drawn = 0;
    }
    if (digitalRead(BTN1)) { //Leave
      tone(speaker,1500);
      delay(100);
      noTone(speaker);
      display_state = 3;
      screen_drawn = 0;
    }

    if (digitalRead(BTN2)) { //Food Status
      tone(speaker,2000);
      delay(100);
      noTone(speaker);
      display_state = 4;
      screen_drawn = 0;
    }

    if (digitalRead(BTN3)) { //Call server
      tone(speaker,1000);
      delay(100);
      noTone(speaker);
      display_state = 5 ;
      screen_drawn = 0;
    }



    break;

  case 2: //Bill called
    billDisplay();
    break;

  case 3: //Leave Table
    leaveDisplay();
    break;

  case 4: //Food Status
    foodDisplay();
    break;

  case 5: //Call Server
    waiterDisplay();
    break;

  default:
    break;
  }


  // Scan bt and listen for new data
  listen_bt();


  // RFID reading
  if(rfid.isCard()){
    if(rfid.readCardSerial()){
      String serNumStr = "";
      for(int i=0; i<5; i++){
        serNumStr = serNumStr + String(rfid.serNum[i], HEX) + " ";              
      }
      bluetooth.print("c," + serNumStr);

      scanned_card = true;

      screen_drawn = 8;

      tone(speaker, 800);
      delay(120);
      tone(speaker, 1000);
      delay(40);
      noTone(speaker);

    }       
  }

  rfid.halt();

}





//BT Functions
void listen_bt() { 
  if(bluetooth.available())  // If the bluetooth sent any characters
  {
    char char_in = (char)bluetooth.read();
    if(char_in != '\0'){ // Before end char

      bt_buffer = bt_buffer + char_in;
    } 
    else { // Print to screen at end char
      last_bt = bt_buffer;
      available_bt = true;
      bt_buffer = "";
    }

  }
}

String read_bt() {
  if(available_bt) {
    available_bt = false;
    return last_bt;
  } 
  else {
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

//ScreenDisplays
void loginDisplay(){

  switch(screen_drawn) {
  case 0: // Initialise
    colorWipe(strip.Color(0, 0, 0), 25);
    tft.fillScreen(ILI9341_BLACK);
    tft.setTextSize(2);
    tft.setCursor(126,113); // centred text
    tft.println("bleepr");

    screen_drawn = 1;
    animation_time = millis();
    break;
  case 1: //Animate Red circle
    if(millis() - animation_time >= 0) {
      //colorWipe(strip.Color(255, 0, 0), 1);
      tft.drawCircle(160,120,60,ILI9341_RED);  
      screen_drawn = 2;
    }  
    break;

  case 2: //Animate black circle clear
    if(millis() - animation_time >= 200){
      tft.drawCircle(160,120,60,ILI9341_BLACK);
      screen_drawn = 3;
    }  
    break;

  case 3: //Animate yellow circle
    if(millis() - animation_time >= 240){
      //  colorWipe(strip.Color(255, 255, 0), 1);
      tft.drawCircle(160,120,80,ILI9341_YELLOW); 
      screen_drawn = 4;
    }  
    break;

  case 4: //Animate black circle clear
    if(millis() - animation_time >= 440){
      tft.drawCircle(160,120,80,ILI9341_BLACK); 
      screen_drawn = 5;
    }  
    break;

  case 5: //Animate green circle
    if(millis() - animation_time >= 480){
      // colorWipe(strip.Color(0, 255, 0), 1);
      tft.drawCircle(160,120,100,ILI9341_GREEN); 
      screen_drawn = 6;
    }  
    break;
  case 6: //Animate (Currently janky and NEEDS to be fixed)
    if(millis() - animation_time >= 680){
      tft.drawCircle(160,120,100,ILI9341_BLACK); 
      screen_drawn = 7;
    }  
    break;

  case 7: //Resets
    if(millis() - animation_time >= 740){
      animation_time = millis();
      screen_drawn = 1;
    }
    break;



  case 8: //Please wait
    tft.setCursor(100,200); // centred text
    tft.println("Please wait");
    screen_drawn = 1;
    break;
  }
}

void homeDisplay(){
  switch(screen_drawn) {
  case 0: //Setup

    //LEDs blue
    colorWipe(strip.Color(0, 0, 255), 25);


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


void billDisplay(){
  switch(screen_drawn) {
  case 0: //Setup

    //LEDs yellow
    colorWipe(strip.Color(255, 255, 0), 25);

    tft.fillRect(160,0,160,120,ILI9341_RED);

    tft.setTextSize(2);
    tft.setCursor(165,60);
    tft.println("Calling bill");

    bluetooth.print("request_bill");

    screen_drawn = 1;
    break;

  case 1: //Loop
    if(available_bt) {
      if( read_bt() == "ok") {
        screen_drawn = 2;
      }
    }
    break;

  case 2: //Exit state
    colorWipe(strip.Color(0, 255, 0), 25);

    tft.fillRect(160,0,160,120,ILI9341_GREEN);

    tft.setTextSize(2);
    tft.setCursor(165,60);
    tft.println("Bill on way");

    delay(1500);
    screen_drawn = 0;
    display_state = 1; //Main menu
    break;

  default:
    break;

  }
}

void waiterDisplay(){
  switch(screen_drawn) {
  case 0: //Setup

    //LEDs yellow
    colorWipe(strip.Color(255, 255, 0), 25);

    tft.fillRect(0,0,160,120,ILI9341_RED);

    tft.setTextSize(2);
    tft.setCursor(5,60);
    tft.println("Calling server");

    bluetooth.print("call_waiter");

    screen_drawn = 1;
    break;

  case 1: //Loop
    if(available_bt) {
      if( read_bt() == "ok") {
        screen_drawn = 2;
      }
    }
    break;

  case 2: //Exit state
    colorWipe(strip.Color(0, 255, 0), 25);

    tft.fillRect(0,0,160,120,ILI9341_GREEN);

    tft.setTextSize(2);
    tft.setCursor(5,60);
    tft.println("Server on way");

    delay(1500);
    screen_drawn = 0;
    display_state = 1; //Main menu
    break;

  default:
    break;

  }
}

void leaveDisplay(){
  switch(screen_drawn) {
  case 0: //Setup

    //LEDs yellow
    colorWipe(strip.Color(255, 255, 0), 25);

    tft.fillRect(160,120,160,120,ILI9341_RED);

    tft.setTextSize(2);
    tft.setCursor(165,180);
    tft.println("Hang on!");

    bluetooth.print("leave_table");

    screen_drawn = 1;
    break;

  case 1: //Loop
    if(available_bt) {
      if( read_bt() == "ok") {
        screen_drawn = 2;
      }
    }
    break;

  case 2: //Exit state
    colorWipe(strip.Color(0, 255, 0), 25);

    tft.fillRect(160,120,160,120,ILI9341_GREEN);

    tft.setTextSize(2);
    tft.setCursor(165,180);
    tft.println("Goodbye");

    delay(2000);
    screen_drawn = 0;
    display_state = 0; //login screen
    break;

  default:
    break;

  }
}

void foodDisplay(){
  switch(screen_drawn) {
  case 0: //Setup

    //LEDs yellow
    colorWipe(strip.Color(255, 255, 0), 25);

    tft.fillRect(0,120,160,120,ILI9341_RED);

    tft.setTextSize(2);
    tft.setCursor(5,180);
    tft.println("Status check");

    bluetooth.print("order_status");
    
//    timeout = millis();

    screen_drawn = 1;
    
    break;

  case 1: //Loop
    if(available_bt) {
      screen_drawn = 2;
    }
    
//    if (timeout - millis() > 10000) {
//      screen_drawn = 3;
//    }
//    break;

  case 2: //Exit state
      colorWipe(strip.Color(0, 255, 0), 25);

    tft.fillRect(0,120,160,120,ILI9341_GREEN);

    order_status = read_bt();

    tft.setTextSize(2);
    tft.setCursor(5,175);
    tft.println(order_status);

    delay(1500);
//    tft.fillRect(0,120,160,120,ILI9341_LIGHTGREY);
//    tft.setCursor(5,180);
//    tft.println("Food status");
    screen_drawn = 0;
    display_state = 1; //Main menu
    break;
    
//  case 3: //Timeout state
//    
//    colorWipe(strip.Color(255, 0, 0), 25);
//
//    tft.fillRect(0,120,160,120,ILI9341_BLUE);
//
//    tft.setTextSize(2);
//    tft.setCursor(5,175);
//    tft.println("Try again");
//
//    delay(1500);
//    tft.fillRect(0,120,160,120,ILI9341_LIGHTGREY);
//    tft.setCursor(5,180);
//    tft.println("Food status");
//    screen_drawn = 1;
//    display_state = 1; //Main menu
//    break;

  default:
    break;

  }
}


void darude() {
  
  rainbowCycle(0);

  int melody[] = {
    NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6,
    NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6,
    NOTE_F6, NOTE_F6, NOTE_F6, NOTE_F6, NOTE_F6,
    NOTE_DS6, NOTE_DS6, NOTE_DS6, NOTE_DS6, NOTE_DS6,
    
    NOTE_AS5, NOTE_AS5, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6,
    NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6,
    NOTE_DS6, NOTE_DS6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6,
    
    NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6,
//    NOTE_DS6, NOTE_DS6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6,
  };

  // note durations: 4 = quarter note, 8 = eighth note, etc.:
  int noteDurations[] = {
    8, 8, 8, 8, 4,
    8, 8, 8, 8, 8, 8, 4,
    8, 8, 8, 8, 4,
    8, 8, 8, 8, 4,
    
    8, 8, 8, 8, 8, 8, 4,
    8, 8, 8, 8, 8, 8, 4,
    8, 8, 8, 8, 8, 8, 4,
    
    8, 8, 8, 8, 8, 8, 4,
//    8, 8, 8, 8, 8, 8, 4,
    
  };
  for (int thisNote = 0; thisNote < sizeof(melody); thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(speaker, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(speaker);
  } 
  
  colorWipe((0,0,0),25);

}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}



// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}






