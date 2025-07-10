/*
  By Ranil Ganlath
  This is for testing displaying looping bitmap images with the IPS Display and the QT Py ESP32-S3 with No PSRAM.
  ESP32-S3 GPIO18 - Pushbutton, Down
  ESP32-S3 GPIO17 - Pushbutton, Up
  ESP32-S3 GPIO9 - Pushbutton, Select
  ESP32-S3 GPIO8 - Pushbutton, Menu/Back
  ESP32-S3 GPIO7 - Battery Sense, (4.3V = 2.96V, 2.8V = 1.93V) (Not wired)
  ESP32-S3 GPIO6 - TFT_BL, BLK (Backlight)
  ESP32-S3 GPIO5 - TFT_CS
  ESP32-S3 GPIO35 - TFT_MOSI
  ESP32-S3 GPIO37 - TFT_DC, DC (Data Command)
  ESP32-S3 GPIO36 - TFT_SCLK, SCL (CLK / SCK)
  ESP32-S3 GPIO16 - TFT_RST, RES (Reset)
  IPS Display Size: 135x240
  
  IDE Configuration:
  Make sure all the display driver and pin connections are correct by
  editing the User_Setup.h file in the TFT_eSPI library folder.

 #########################################################################
 ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
 #########################################################################
*/

#define downButton 18
#define upButton 17
#define selectButton 9
#define menuButton 8

#include <TFT_eSPI.h>       // Hardware-specific library

TFT_eSPI tft = TFT_eSPI();  // Invoke custom library

// Include the header files that contain the icons
#include "OtaconB.h" //Otacon Default
#include "OtaconT.h" //Otacon Talking
#include "OtaconS1.h" //Shocked 1
#include "OtaconS2.h" //Shocked 2
#include "OtaconS3.h" //Shocked 3


void setup()
{
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(0);	// portrait
  tft.fillScreen(TFT_BLACK);

  // Swap the colour byte order when rendering
  tft.setSwapBytes(true);

  // Draw the icons
  tft.pushImage(0,0,140,240,OB);

  delay(500);
}

void loop()
{
  int state=random(1,4); //generates a random number that is either 1, 2,or 3.
  if(state==1){otaconTalk();}
  if(state==2){otaconSilent();}
  if(state==3){otaconYell();} 
}

void otaconTalk(){
  tft.pushImage(0,0,140,240,OB);
  delay(350);
  tft.pushImage(0,0,140,240,OT);
  delay(350);
  tft.pushImage(0,0,140,240,OB);
  delay(350);
  tft.pushImage(0,0,140,240,OT);
  delay(350);
  tft.pushImage(0,0,140,240,OB);
  delay(350);
}

void otaconSilent(){
  tft.pushImage(0,0,140,240,OB);
  delay(1750);
}

void otaconYell(){
  tft.pushImage(0,0,140,240,OB);
  delay(350);
  tft.pushImage(0,0,140,240,O1);
  delay(250);
  tft.pushImage(0,0,140,240,O3);
  delay(250);
  tft.pushImage(0,0,140,240,O2);
  delay(250);
  tft.pushImage(0,0,140,240,O3);
  delay(250);
  tft.pushImage(0,0,140,240,OB);
  delay(350);
}
