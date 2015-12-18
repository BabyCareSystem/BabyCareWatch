/*
    RetroWatch Arduino is a part of open source smart watch project.
    Copyright (C) 2014  Suh Young Bae

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see [http://www.gnu.org/licenses/].
*/
/*
Retro Watch Arduino v1.0 - u8g (supports u8glib)

  Get the latest version, android host app at 
  ------> https://github.com/godstale/retrowatch
  ------> or http://www.hardcopyworld.com

Written by Suh Young Bae (godstale@hotmail.com)
All text above must be included in any redistribution
*/

//#include <avr/pgmspace.h>
#include <SoftwareSerial.h>
#include <math.h>
#include "bitmap.h"

#include "U8glib.h"

///////////////////////////////////////////////////////////////////
//----- OLED instance

// IMPORTANT NOTE: The complete list of supported devices 
// with all constructor calls is here: http://code.google.com/p/u8glib/wiki/device

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);	// I2C / TWI 
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//----- BT instance
SoftwareSerial BTSerial(7, 8); //Connect HC-06, RX, TX
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//----- Protocol

//----- Bluetooth protocol parsing

#define CMD_TYPE_START 0x00
#define CMD_TYPE_IDLE 0x01
#define CMD_TYPE_TEMP_38 'A'   // ascill '8'
#define CMD_TYPE_TEMP_39 'B'
#define CMD_TYPE_TEMP_40 'C'
#define CMD_TYPE_TEMP_41 'D'
#define CMD_TYPE_TEMP_42 'E'
#define CMD_TYPE_TEMP_43 'F'
#define CMD_TYPE_SOUND 'G'   // ascill '''
#define CMD_TYPE_ROLLINGOVER 'H' // ascill 'p'
#define CMD_TYPE_HUMIDITY 'I'    // ascill


byte TR_COMMAND = CMD_TYPE_START;
int sleepCount = 0;

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////

byte centerX = 64;
byte centerY = 32;
byte iRadius = 28;

//PROGMEM const char* strIntro[] = {"BabyAngel", "Band", "v1.0"};
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//----- Button control
int buttonPin = 11;
int vibPin = 10;
boolean isClicked = HIGH;
boolean isReceived = false;
boolean isConnected = false;
///////////////////////////////////////////////////////////////////


void setup()   {
  //Serial.begin(9600);    // WARNING: Do not enable this if there is not enough memory
  //Serial.println(F("RetroWatch v1.0 u8g"));

  //----- Set button
  pinMode(buttonPin, INPUT_PULLUP);  // Defines button pin
  pinMode(vibPin, OUTPUT);  // Defines button pin
  //----- Set display features
  centerX = u8g.getWidth() / 2;
  centerY = u8g.getHeight() / 2;
  iRadius = centerY - 2;

  //----- Setup serial connection with BT
  BTSerial.begin(9600);  // set the data rate for the BT port
  
  //@@----- Show logo
  //drawStartUp();    // Show RetroWatch Logo
  //delay(3000);
}


void loop() {
  
    vibrate(1);
  drawWaringMessage('G');
  delay(2000);
 /*

  isClicked = HIGH;
  isReceived = t;
  
  if(digitalRead(buttonPin) == LOW) 
    isClicked = LOW;
  
  // Receive data from remote and parse
  //@@if(isConnectedBT())
  //@@  isReceived = receiveBluetoothData();
  //@@isReceived =true;
 //@@ TR_COMMAND = 'A';
  // Display routine
  onDraw();
  
  // If data doesn't arrive, wait for a while to save battery
  if(!isReceived)
    delay(300);
    
   isConnected = false;
   */
}


void vibrate(int second){
  digitalWrite(vibPin,HIGH);
  delay(second*1000);
  digitalWrite(vibPin,LOW);
}


///////////////////////////////////
//----- BT, connected  functions
///////////////////////////////////

// Parsing packet according to current mode
boolean isConnectedBT() {
  if(isConnected)
      return true;
  
  if(BTSerial.available()) 
      if(BTSerial.read()=='X'){
        isConnected = true;
        return true;
      }
  return false;
  
}  // End of receiveBluetoothData()

///////////////////////////////////
//----- BT, Data parsing functions
///////////////////////////////////

// Parsing packet according to current mode
boolean receiveBluetoothData() {
  if(BTSerial.available()) {
      TR_COMMAND = BTSerial.read();
      return true;
  }else{
   return false; 
  }
  
}  // End of receiveBluetoothData()

///////////////////////////////////
//----- Drawing methods
///////////////////////////////////

// Main drawing routine.
// Every drawing except splash screen starts here.
// displayMode parameter determines which page to be shown.
// Internal, external event and user input by button changes displayMode
// To avoid too fast redraw most page has display interval
//
void onDraw() {
  
  if(isReceived == true)
  {
    
    vibrate(1);
    u8g.sleepOff();
    sleepCount = 0;
    
    if(TR_COMMAND == CMD_TYPE_TEMP_38) {
      drawTEMPMessage(38);
    }else if(TR_COMMAND == CMD_TYPE_TEMP_39) {
      drawTEMPMessage(38.5);
    }else if(TR_COMMAND == CMD_TYPE_TEMP_40) {
      drawTEMPMessage(39);
    }else if(TR_COMMAND == CMD_TYPE_TEMP_41) {
      drawTEMPMessage(39.5);
    }else if(TR_COMMAND == CMD_TYPE_TEMP_42) {
      drawTEMPMessage(40);
    }else if(TR_COMMAND == CMD_TYPE_TEMP_43) {
      drawTEMPMessage(40.5);
    }else if(TR_COMMAND == CMD_TYPE_SOUND) {
      drawWaringMessage(TR_COMMAND);
    }else if(TR_COMMAND == CMD_TYPE_ROLLINGOVER) {
      drawWaringMessage(TR_COMMAND);
    }else if(TR_COMMAND == CMD_TYPE_HUMIDITY) {
      drawWaringMessage(TR_COMMAND);
    }else{
      return;
    }
    isClicked = HIGH;
    while(isClicked == HIGH)
    {
      if(digitalRead(buttonPin) == LOW) isClicked = LOW;
    }
  }else if( isClicked ==LOW){
    u8g.sleepOff();
    sleepCount = 0;
    drawEmptyMessage();
  }else{
    if( sleepCount < 10 )
    {
      drawWait();
      sleepCount++;
    }else{
       u8g.sleepOn();
    }
  }
}  // End of onDraw()

// RetroWatch splash screen
void drawStartUp() {
  // picture loop  
  u8g.firstPage();  
  do {
    u8g_prepare();
    // draw logo
    //u8g.drawBitmapP( 10, 15, 3, 24, IMG_logo_24x24);
    u8g.drawXBMP( 10, 15 , 35, 35, IMG_babyface_35X35);
    // show text
    u8g.setFont(u8g_font_courB10);
    u8g.setFontRefHeightExtendedText();
    u8g.setDefaultForegroundColor();
    u8g.setFontPosTop();
    //u8g.drawStr(45, 12, (char*)pgm_read_word(&(strIntro[0])));
    //u8g.drawStr(45, 28, (char*)pgm_read_word(&(strIntro[1])));
    
    u8g.setFont(u8g_font_fixed_v0);
    u8g.setFontRefHeightExtendedText();
    u8g.setDefaultForegroundColor();
    u8g.setFontPosTop();
    //u8g.drawStr(45, 45, (char*)pgm_read_word(&(strIntro[2])));
  } while( u8g.nextPage() );
}

// Draw Temperature message page
void drawTEMPMessage(float temperature) {
  // get icon index
  int icon_num = 60;
  char mystr1[6];
  int dec, sign;
  // picture loop  
  u8g.firstPage();  
  do {
    u8g_prepare();
    // draw icon and string
    u8g.drawXBMP( centerX - 12, centerY - 25, 35, 35, IMG_temperature_35X35);   
    dtostrf( temperature, 4, 1, mystr1);
    //char mytemp[30] = "warning ";
    char mytemp[30] = "";
    char mystr2[] ="'c";
    strcat(mytemp, mystr1);
    strcat(mytemp, mystr2);
    int string_start_x = u8g.getStrPixelWidth((char *)(mytemp)) / 2;
    if(string_start_x > 0) {
      if(string_start_x > centerX) 
        string_start_x = 0;
      else 
        string_start_x = centerX - string_start_x;
        
      u8g.setFont(u8g_font_fixed_v0);
      u8g.setFontRefHeightExtendedText();
      u8g.setDefaultForegroundColor();
      u8g.setFontPosTop();
      u8g.drawStr( string_start_x, centerY + 10, (char *)(mytemp) );
    }
  } while( u8g.nextPage() );
}
// Draw wait page
void drawWait() {
  // get icon index
  // picture loop  
  u8g.firstPage();  
  do {
    u8g_prepare();   
    u8g.drawXBMP( 0, 0, 128, 64, IMG_babywait_128X64);
  } while( u8g.nextPage() );
}
// Draw Empty Message page
void drawEmptyMessage() {
  // picture loop  
  u8g.firstPage();  
  do {
    u8g_prepare();
    // draw icon and string
    u8g.drawXBMP( 0, 0, 128, 64, IMG_babywork_128X64);
  } while( u8g.nextPage() );
}
// Draw warning page
void drawWaringMessage(int data) {
  // picture loop  
  u8g.firstPage();  
  do {
    u8g_prepare();
    if(data == CMD_TYPE_SOUND) {
        u8g.drawXBMP( 0, 0, 128, 64, IMG_babysound_128X64);
    }else if(data == CMD_TYPE_ROLLINGOVER) {
        u8g.drawXBMP( 0, 0, 128, 64, IMG_babyrollingover_128X64);
    }else if(data == CMD_TYPE_HUMIDITY) {
        u8g.drawXBMP( 0, 0, 128, 64, IMG_babydiaper_128X64);
    }
  } while( u8g.nextPage() );
}

// U8glib Initialization
void u8g_prepare(void) {
  // Add display setting code here
  //u8g.setFont(u8g_font_6x10);
  //u8g.setFontRefHeightExtendedText();
  //u8g.setDefaultForegroundColor();
  //u8g.setFontPosTop();
}

