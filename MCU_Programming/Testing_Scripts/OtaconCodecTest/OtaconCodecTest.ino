// This example is based on this tutorial
// https://www.instructables.com/id/Converting-Images-to-Flash-Memory-Iconsimages-for-/
// https://github.com/STEMpedia/eviveProjects/blob/master/imageToFlashMemoryIconsForTFT/

//Don't forget to change User_Setup.h inside TFT_eSPI library !

//OB: Basic Otacon
//OT: Talking Otacon
//O1: Shock 1 Otacon
//O2: Shock 2 Otacon
//O3: Shock 3 Otacon


#include <TFT_eSPI.h>
#include "bitmap.h" //(Bitmap generated with LCD Image Converter) https://sourceforge.net/projects/lcd-image-converter/
TFT_eSPI tft = TFT_eSPI();   // Invoke library

void setup(void) {
  Serial.begin(115200);
  Serial.print("Codec ID Badge Bitmap Test");

  tft.begin();     // initialize a ST7789 chip
  tft.setSwapBytes(true); // Swap the byte order for pushImage() - corrects endianness
  tft.fillScreen(TFT_BLACK); //Clear screen
}

void loop() {
  tft.pushImage(0,0,140,240,OB);
  delay(350);
  tft.pushImage(0,0,140,240,OT);
  delay(350);
  tft.pushImage(0,0,140,240,OB);
  delay(350);
  tft.pushImage(0,0,140,240,OB);
  delay(350);
  tft.pushImage(0,0,140,240,OT);
  delay(350);
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
}
