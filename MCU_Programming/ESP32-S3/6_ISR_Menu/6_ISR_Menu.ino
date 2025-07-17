/*
  By Ranil Ganlath
  This is for testing bitmaps with menu with the IPS Display and the QT Py ESP32-S3 with No PSRAM.
  Modified to use interrupts for button handling.
  
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

//Define Codec Colors
#define ColorCodecMainText 0x97D9
#define ColorCodecCallText 0x21A6
#define ColorCodecCallTextSelected 0x6551
#define ColorCodecBG 0x0000
#define ColorCodecText 0xC617
#define ColorCodecTextNote 0x42EA

#include <TFT_eSPI.h>       // Hardware-specific library

//Add Bitmap Images
#include "CAMPBELL.h"
#include "CAMPBELLM.h"
#include "DEEPTHROAT.h"
#include "LIQUID.h"
#include "MEILING.h"
#include "MEILINGB.h"
#include "MEILINGM.h"
#include "MERYL.h"
#include "MERYLB.h"
#include "MERYLM.h"
#include "MILLER.h"
#include "MILLERM.h"
#include "NAOMI.h"
#include "NAOMIB.h"
#include "NAOMIM.h"
#include "NASTASHA.h"
#include "NASTASHAB.h"
#include "NASTASHAM.h"
#include "OTACON.h"
#include "OTACONB.h"
#include "OTACONM.h"
#include "SNAKE.h"
#include "SNAKEB.h"
#include "SNAKEM.h"
#include "SNIPERWOLF.h"
#include "SNIPERWOLFB.h"
#include "SNIPERWOLFM.h"
#include "Select.h"

TFT_eSPI tft = TFT_eSPI(135,240);  // Invoke custom library

// Menu structure definitions
struct MenuItem {
  String name;
  int subMenuCount;
  MenuItem* subMenus;
  bool isAction;
};

// Menu state variables
int currentMenuLevel = 0;
int currentSelection[5] = {0, 0, 0, 0, 0}; // Support up to 5 levels deep
MenuItem* currentMenu;
int maxMenuLevels = 5;

// Interrupt-based button handling variables
volatile bool menuButtonPressed = false;
volatile bool selectButtonPressed = false;
volatile bool upButtonPressed = false;
volatile bool downButtonPressed = false;

// Button debouncing variables for interrupts
volatile unsigned long lastMenuInterrupt = 0;
volatile unsigned long lastSelectInterrupt = 0;
volatile unsigned long lastUpInterrupt = 0;
volatile unsigned long lastDownInterrupt = 0;
const unsigned long debounceDelay = 200; // Increased for interrupt handling

// Display parameters
const int MENU_ITEM_HEIGHT = 25;
const int MENU_START_Y = 30;
const int MAX_VISIBLE_ITEMS = 7;
int scrollOffset = 0;

// Animation control variables
bool animationActive = false;
bool animationButtonPressed = false;

// Define submenu items
MenuItem callItems[] = {
  {"SNAKE", 0, nullptr, true},
  {"CAMPBELL", 0, nullptr, true},
  {"NAOMI", 0, nullptr, true},
  {"MILLER", 0, nullptr, true},
  {"LIQUID", 0, nullptr, true},
  {"OTACON", 0, nullptr, true},
  {"SNIPER WOLF", 0, nullptr, true},
  {"MERYL", 0, nullptr, true},
  {"MEI LING", 0, nullptr, true},
  {"NASTASHA", 0, nullptr, true},
  {"DEEPTHROAT", 0, nullptr, true}
};

MenuItem animatedItems[] = {
  {"An SNAKE", 0, nullptr, true},
  {"An CAMPBELL", 0, nullptr, true},
  {"An NAOMI", 0, nullptr, true},
  {"An MILLER", 0, nullptr, true},
  {"An OTACON", 0, nullptr, true},
  {"An SNIPER WOLF", 0, nullptr, true},
  {"An MERYL", 0, nullptr, true},
  {"An MEI LING", 0, nullptr, true},
  {"An NASTASHA", 0, nullptr, true}
};

MenuItem settingsItems[] = {
  {"Brightness", 0, nullptr, true},
  {"Restart System", 0, nullptr, true},
};

MenuItem QRItems[] = {
  {"Business Card", 0, nullptr, true},
  {"LinkedIn", 0, nullptr, true},
  {"Github", 0, nullptr, true},
};

// Main menu items
MenuItem mainMenuItems[] = {
  {"Call", 11, callItems, false},
  {"Incoming Call", 0, nullptr, true},
  {"Animated", 9, animatedItems, false},
  {"Settings", 2, settingsItems, false},
  {"QR", 3, QRItems, false},  
  {"Games", 0, nullptr, true},
  {"Sleep", 0, nullptr, true}
};

// Root menu
MenuItem rootMenu = {"Main Menu", 7, mainMenuItems, false};

// Interrupt Service Routines (ISRs)
void IRAM_ATTR menuButtonISR() {
  unsigned long currentTime = millis();
  if (currentTime - lastMenuInterrupt > debounceDelay) {
    menuButtonPressed = true;
    lastMenuInterrupt = currentTime;
    if (animationActive) {
      animationButtonPressed = true;
    }
  }
}

void IRAM_ATTR selectButtonISR() {
  unsigned long currentTime = millis();
  if (currentTime - lastSelectInterrupt > debounceDelay) {
    selectButtonPressed = true;
    lastSelectInterrupt = currentTime;
    if (animationActive) {
      animationButtonPressed = true;
    }
  }
}

void IRAM_ATTR upButtonISR() {
  unsigned long currentTime = millis();
  if (currentTime - lastUpInterrupt > debounceDelay) {
    upButtonPressed = true;
    lastUpInterrupt = currentTime;
    if (animationActive) {
      animationButtonPressed = true;
    }
  }
}

void IRAM_ATTR downButtonISR() {
  unsigned long currentTime = millis();
  if (currentTime - lastDownInterrupt > debounceDelay) {
    downButtonPressed = true;
    lastDownInterrupt = currentTime;
    if (animationActive) {
      animationButtonPressed = true;
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Initialize buttons (external pulldown resistors)
  pinMode(downButton, INPUT);
  pinMode(upButton, INPUT);
  pinMode(selectButton, INPUT);
  pinMode(menuButton, INPUT);
  
  // Attach interrupts (RISING edge since we're using external pull-down resistors)
  attachInterrupt(digitalPinToInterrupt(menuButton), menuButtonISR, RISING);
  attachInterrupt(digitalPinToInterrupt(selectButton), selectButtonISR, RISING);
  attachInterrupt(digitalPinToInterrupt(upButton), upButtonISR, RISING);
  attachInterrupt(digitalPinToInterrupt(downButton), downButtonISR, RISING);
  
  // Initialize display
  tft.begin();
  tft.setRotation(0);  // portrait
  tft.fillScreen(ColorCodecBG);
  tft.setSwapBytes(true);
  tft.setTextSize(1);
  tft.setTextColor(ColorCodecCallText);
  
  // Initialize menu system
  currentMenu = &rootMenu;
  currentMenuLevel = 0;
  
  // Draw initial menu
  drawMenu();
  
  delay(500);
}

void loop() {
  // Handle button presses from interrupts
  if (menuButtonPressed) {
    menuButtonPressed = false;
    handleMenuButton();
  }
  
  if (selectButtonPressed) {
    selectButtonPressed = false;
    handleSelectButton();
  }
  
  if (upButtonPressed) {
    upButtonPressed = false;
    handleUpButton();
  }
  
  if (downButtonPressed) {
    downButtonPressed = false;
    handleDownButton();
  }
  
  // Small delay to prevent overwhelming the system
  delay(10);
}

void drawMenu() {
  tft.fillScreen(ColorCodecBG); 
  // Draw title
  tft.setTextColor(ColorCodecMainText);
  tft.setTextSize(2);
  tft.setCursor(5, 5);
  tft.println(currentMenu->name);
  
  // Draw menu items
  tft.setTextSize(1);
  int itemCount = currentMenu->subMenuCount;
  
  // Calculate visible range
  int currentPos = currentSelection[currentMenuLevel];
  if (currentPos < scrollOffset) {
    scrollOffset = currentPos;
  }
  if (currentPos >= scrollOffset + MAX_VISIBLE_ITEMS) {
    scrollOffset = currentPos - MAX_VISIBLE_ITEMS + 1;
  }
  
  for (int i = 0; i < MAX_VISIBLE_ITEMS && (i + scrollOffset) < itemCount; i++) {
    int itemIndex = i + scrollOffset;
    int yPos = MENU_START_Y + (i * MENU_ITEM_HEIGHT);
    
    // Highlight selected item
    if (itemIndex == currentPos) {
      tft.setTextColor(ColorCodecCallTextSelected);
    } else {
      tft.setTextColor(ColorCodecCallText);
    }
    
    tft.setCursor(5, yPos);
    
    // Add indicator for submenus
    if (!currentMenu->subMenus[itemIndex].isAction) {
      tft.print("> ");
    } else {
      tft.print("  ");
    }
    
    tft.print(currentMenu->subMenus[itemIndex].name);
  }
  
  // Draw scroll indicators
  if (scrollOffset > 0) {
    tft.setTextColor(ColorCodecTextNote);
    tft.setCursor(120, MENU_START_Y);
    tft.print("^");
  }
  
  if (scrollOffset + MAX_VISIBLE_ITEMS < itemCount) {
    tft.setTextColor(ColorCodecTextNote);
    tft.setCursor(120, MENU_START_Y + (MAX_VISIBLE_ITEMS - 1) * MENU_ITEM_HEIGHT);
    tft.print("v");
  }
  
  // Draw navigation help
  tft.setTextColor(ColorCodecTextNote);
  tft.setTextSize(1);
  tft.setCursor(5, 220);
  tft.print("Menu:Back Select:OK");
}

void handleMenuButton() {
  // Go back to previous menu level
  if (currentMenuLevel > 0) {
    currentMenuLevel--;
    currentSelection[currentMenuLevel + 1] = 0; // Reset submenu selection
    scrollOffset = 0;
    
    // Navigate back to parent menu
    currentMenu = &rootMenu;
    for (int i = 0; i < currentMenuLevel; i++) {
      currentMenu = &currentMenu->subMenus[currentSelection[i]];
    }
    
    drawMenu();
  }
}

void handleSelectButton() {
  int currentPos = currentSelection[currentMenuLevel];
  MenuItem* selectedItem = &currentMenu->subMenus[currentPos];
  
  if (selectedItem->isAction) {
    // Execute action
    executeAction(selectedItem->name);
  } else {
    // Navigate to submenu
    if (currentMenuLevel < maxMenuLevels - 1) {
      currentMenuLevel++;
      currentSelection[currentMenuLevel] = 0;
      scrollOffset = 0;
      currentMenu = selectedItem;
      drawMenu();
    }
  }
}

void handleUpButton() {
  if (currentSelection[currentMenuLevel] > 0) {
    currentSelection[currentMenuLevel]--;
    drawMenu();
  }
}

void handleDownButton() {
  if (currentSelection[currentMenuLevel] < currentMenu->subMenuCount - 1) {
    currentSelection[currentMenuLevel]++;
    drawMenu();
  }
}

void animationSNAKE(){
    // Animated Snake with interrupt-based exit
    animationActive = true;
    animationButtonPressed = false;
    tft.pushImage(0,0,140,240,SNAKE);
    while (!animationButtonPressed) {
      int randomNumber = random(11);
      if(randomNumber>4){//make them talk
        delay(random(300,500));
        tft.pushImage(32,127,86,59,SNAKEM);
        delay(random(300,500));
        tft.pushImage(0,0,140,240,SNAKE);
        if(randomNumber>6){
          delay(random(300,500));
          tft.pushImage(32,127,86,59,SNAKEM);
          delay(random(300,500));
          tft.pushImage(0,0,140,240,SNAKE);
          if(randomNumber>7){
            delay(random(300,500));
            tft.pushImage(32,127,86,59,SNAKEM);
            delay(random(300,500));
            tft.pushImage(0,0,140,240,SNAKE);
          }
          if(randomNumber>8){
            tft.pushImage(11,73,118,35,SNAKEB);
            delay(random(400,600));
            tft.pushImage(0,0,140,240,SNAKE);
          }
        }
      }
      if (animationButtonPressed) break;
      delay(random(400,3000));
      if (animationButtonPressed) break;
    }
    animationActive = false;
    animationButtonPressed = false;
}

void animationCAMPBELL(){
    // Animated Campbell with interrupt-based exit
    animationActive = true;
    animationButtonPressed = false;
    tft.pushImage(0,0,140,240,CAMPBELL);
    while (!animationButtonPressed) {
      int randomNumber = random(11);
      if(randomNumber>4){//make them talk
        delay(random(300,500));
        tft.pushImage(27,92,91,91,CAMPBELLM);
        delay(random(300,500));
        tft.pushImage(0,0,140,240,CAMPBELL);
        if(randomNumber>6){
          delay(random(300,500));
          tft.pushImage(27,92,91,91,CAMPBELLM);
          delay(random(300,500));
          tft.pushImage(0,0,140,240,CAMPBELL);
          if(randomNumber>8){
            delay(random(300,500));
            tft.pushImage(27,92,91,91,CAMPBELLM);
            delay(random(300,500));
            tft.pushImage(0,0,140,240,CAMPBELL);
          }
        }
      }
      if (animationButtonPressed) break;
      delay(random(400,3000));
      if (animationButtonPressed) break;
    }
    animationActive = false;
    animationButtonPressed = false;
}

void animationNAOMI(){
    // Animated Naomi with interrupt-based exit
    animationActive = true;
    animationButtonPressed = false;
    tft.pushImage(0,0,140,240,NAOMI);
    while (!animationButtonPressed) {
      int randomNumber = random(11);
      if(randomNumber>4){//make them talk
        delay(random(300,500));
        tft.pushImage(32,132,70,37,NAOMIM);
        delay(random(300,500));
        tft.pushImage(0,0,140,240,NAOMI);
        if(randomNumber>6){
          delay(random(300,500));
          tft.pushImage(32,132,70,37,NAOMIM);
          delay(random(300,500));
          tft.pushImage(0,0,140,240,NAOMI);
          if(randomNumber>7){
            delay(random(300,500));
            tft.pushImage(32,132,70,37,NAOMIM);
            delay(random(300,500));
            tft.pushImage(0,0,140,240,NAOMI);
          }
          if(randomNumber>8){
            tft.pushImage(11,79,113,32,NAOMIB);
            delay(random(400,600));
            tft.pushImage(0,0,140,240,NAOMI);
          }
        }
      }
      if (animationButtonPressed) break;
      delay(random(400,3000));
      if (animationButtonPressed) break;
    }
    animationActive = false;
    animationButtonPressed = false;
}

void animationMILLER(){
    // Animated Miller with interrupt-based exit
    animationActive = true;
    animationButtonPressed = false;
    tft.pushImage(0,0,140,240,MILLER);
    while (!animationButtonPressed) {
      int randomNumber = random(11);
      if(randomNumber>4){//make them talk
        delay(random(300,500));
        tft.pushImage(32,121,81,43,MILLERM);
        delay(random(300,500));
        tft.pushImage(0,0,140,240,MILLER);
        if(randomNumber>6){
          delay(random(300,500));
          tft.pushImage(32,121,81,43,MILLERM);
          delay(random(300,500));
          tft.pushImage(0,0,140,240,MILLER);
          if(randomNumber>8){
            delay(random(300,500));
            tft.pushImage(32,121,81,43,MILLERM);
            delay(random(300,500));
            tft.pushImage(0,0,140,240,MILLER);
          }
        }
      }
      if (animationButtonPressed) break;
      delay(random(400,3000));
      if (animationButtonPressed) break;
    }
    animationActive = false;
    animationButtonPressed = false;
}

void animationOTACON(){
    // Animated Otacon with interrupt-based exit
    animationActive = true;
    animationButtonPressed = false;
    tft.pushImage(0,0,140,240,OTACON);
    while (!animationButtonPressed) {
      int randomNumber = random(11);
      if(randomNumber>4){//make them talk
        delay(random(300,500));
        tft.pushImage(22,135,86,48,OTACONM);
        delay(random(300,500));
        tft.pushImage(0,0,140,240,OTACON);
        if(randomNumber>6){
          delay(random(300,500));
          tft.pushImage(22,135,86,48,OTACONM);
          delay(random(300,500));
          tft.pushImage(0,0,140,240,OTACON);
          if(randomNumber>7){
            delay(random(300,500));
            tft.pushImage(22,135,86,48,OTACONM);
            delay(random(300,500));
            tft.pushImage(0,0,140,240,OTACON);
          }
          if(randomNumber>8){
            tft.pushImage(27,79,97,45,OTACONB);
            delay(random(400,600));
            tft.pushImage(0,0,140,240,OTACON);
          }
        }
      }
      if (animationButtonPressed) break;
      delay(random(400,3000));
      if (animationButtonPressed) break;
    }
    animationActive = false;
    animationButtonPressed = false;
}

void animationSNIPERWOLF(){
    // Animated Sniper Wolf with interrupt-based exit
    animationActive = true;
    animationButtonPressed = false;
    tft.pushImage(0,0,140,240,SNIPERWOLF);
    while (!animationButtonPressed) {
      int randomNumber = random(11);
      if(randomNumber>4){//make them talk
        delay(random(300,500));
        tft.pushImage(38,129,64,35,SNIPERWOLFM);
        delay(random(300,500));
        tft.pushImage(0,0,140,240,SNIPERWOLF);
        if(randomNumber>6){
          delay(random(300,500));
          tft.pushImage(38,129,64,35,SNIPERWOLFM);
          delay(random(300,500));
          tft.pushImage(0,0,140,240,SNIPERWOLF);
          if(randomNumber>7){
            delay(random(300,500));
            tft.pushImage(38,129,64,35,SNIPERWOLFM);
            delay(random(300,500));
            tft.pushImage(0,0,140,240,SNIPERWOLF);
          }
          if(randomNumber>8){
            tft.pushImage(11,78,118,27,SNIPERWOLFB);
            delay(random(400,600));
            tft.pushImage(0,0,140,240,SNIPERWOLF);
          }
        }
      }
      if (animationButtonPressed) break;
      delay(random(400,3000));
      if (animationButtonPressed) break;
    }
    animationActive = false;
    animationButtonPressed = false;
}

void animationMERYL(){
    // Animated Meryl with interrupt-based exit
    animationActive = true;
    animationButtonPressed = false;
    tft.pushImage(0,0,140,240,MERYL);
    while (!animationButtonPressed) {
      int randomNumber = random(11);
      if(randomNumber>4){//make them talk
        delay(random(300,500));
        tft.pushImage(37,118,70,40,MERYLM);
        delay(random(300,500));
        tft.pushImage(0,0,140,240,MERYL);
        if(randomNumber>6){
          delay(random(300,500));
          tft.pushImage(37,118,70,40,MERYLM);
          delay(random(300,500));
          tft.pushImage(0,0,140,240,MERYL);
          if(randomNumber>7){
            delay(random(300,500));
            tft.pushImage(37,118,70,40,MERYLM);
            delay(random(300,500));
            tft.pushImage(0,0,140,240,MERYL);
          }
          if(randomNumber>8){
            tft.pushImage(11,64,124,35,MERYLB);
            delay(random(400,600));
            tft.pushImage(0,0,140,240,MERYL);
          }
        }
      }
      if (animationButtonPressed) break;
      delay(random(400,3000));
      if (animationButtonPressed) break;
    }
    animationActive = false;
    animationButtonPressed = false;
}

void animationMEILING(){
    // Animated Mei Ling with interrupt-based exit
    animationActive = true;
    animationButtonPressed = false;
    tft.pushImage(0,0,140,240,MEILING);
    while (!animationButtonPressed) {
      int randomNumber = random(11);
      if(randomNumber>4){//make them talk
        delay(random(300,500));
        tft.pushImage(38,138,64,37,MEILINGM);
        delay(random(300,500));
        tft.pushImage(0,0,140,240,MEILING);
        if(randomNumber>6){
          delay(random(300,500));
          tft.pushImage(38,138,64,37,MEILINGM);
          delay(random(300,500));
          tft.pushImage(0,0,140,240,MEILING);
          if(randomNumber>7){
            delay(random(300,500));
            tft.pushImage(38,138,64,37,MEILINGM);
            delay(random(300,500));
            tft.pushImage(0,0,140,240,MEILING);
          }
          if(randomNumber>8){
            tft.pushImage(11,89,118,37,MEILINGB);
            delay(random(400,600));
            tft.pushImage(0,0,140,240,MEILING);
          }
        }
      }
      if (animationButtonPressed) break;
      delay(random(400,3000));
      if (animationButtonPressed) break;
    }
    animationActive = false;
    animationButtonPressed = false;
}

void animationNASTASHA(){
    // Animated Nastasha with interrupt-based exit
    animationActive = true;
    animationButtonPressed = false;
    tft.pushImage(0,0,140,240,NASTASHA);
    while (!animationButtonPressed) {
      int randomNumber = random(11);
      if(randomNumber>4){//make them talk
        delay(random(300,500));
        tft.pushImage(16,138,113,62,NASTASHAM);
        delay(random(300,500));
        tft.pushImage(0,0,140,240,NASTASHA);
        if(randomNumber>6){
          delay(random(300,500));
          tft.pushImage(16,138,113,62,NASTASHAM);
          delay(random(300,500));
          tft.pushImage(0,0,140,240,NASTASHA);
          if(randomNumber>7){
            delay(random(300,500));
            tft.pushImage(16,138,113,62,NASTASHAM);
            delay(random(300,500));
            tft.pushImage(0,0,140,240,NASTASHA);
          }
          if(randomNumber>8){
            tft.pushImage(16,86,113,35,NASTASHAB);
            delay(random(400,600));
            tft.pushImage(0,0,140,240,NASTASHA);
          }
        }
      }
      if (animationButtonPressed) break;
      delay(random(400,3000));
      if (animationButtonPressed) break;
    }
    animationActive = false;
    animationButtonPressed = false;
}

void incomingCall(){
    tft.fillScreen(TFT_BLACK);
    delay(800);
    tft.pushImage(0,116,135,38,Select);
    delay(800);
    tft.fillScreen(TFT_BLACK);
    delay(800);
    tft.pushImage(0,116,135,38,Select);
    delay(800);
    tft.fillScreen(TFT_BLACK);
    delay(1100);
    tft.pushImage(0,116,135,38,Select);
    delay(800);
    tft.fillScreen(TFT_BLACK);
    delay(800);
    tft.pushImage(0,116,135,38,Select);
}

void executeAction(String actionName) {
  // Clear screen and show action
  tft.fillScreen(ColorCodecBG);
  tft.setTextColor(ColorCodecMainText);
  tft.setTextSize(2);
  tft.setCursor(5, 50);
  tft.println("Executing:");
  
  tft.setTextColor(ColorCodecText);
  tft.setTextSize(1);
  tft.setCursor(5, 80);
  tft.println(actionName);
  
  // Show some feedback based on action
  tft.setCursor(5, 110);
  if (actionName == "Connect WiFi") {
    tft.println("Connecting to WiFi...");
    delay(2000);
    tft.setCursor(5, 130);
    tft.println("Connected!");
  } else if (actionName == "SNAKE") {
    tft.pushImage(0,0,140,240,SNAKE);
  } else if (actionName == "CAMPBELL") {
    tft.pushImage(0,0,140,240,CAMPBELL);
  } else if (actionName == "NAOMI") {
    tft.pushImage(0,0,140,240,NAOMI);
  } else if (actionName == "MILLER") {
    tft.pushImage(0,0,140,240,MILLER);
  } else if (actionName == "LIQUID") {
    tft.pushImage(0,0,140,240,LIQUID);
  } else if (actionName == "OTACON") {
    tft.pushImage(0,0,140,240,OTACON);
  } else if (actionName == "SNIPER WOLF") {
    tft.pushImage(0,0,140,240,SNIPERWOLF);
  } else if (actionName == "MERYL") {
    tft.pushImage(0,0,140,240,MERYL);
  } else if (actionName == "MEI LING") {
    tft.pushImage(0,0,140,240,MEILING);
  } else if (actionName == "NASTASHA") {
    tft.pushImage(0,0,140,240,NASTASHA);
  } else if (actionName == "DEEPTHROAT") {
    tft.pushImage(0,0,144,240,DEEPTHROAT);
  } else if (actionName == "An SNAKE") {
    animationSNAKE();
  } else if (actionName == "An CAMPBELL") {
    animationCAMPBELL();
  } else if (actionName == "An NAOMI") {
    animationNAOMI();
  } else if (actionName == "An MILLER") {
    animationMILLER();
  } else if (actionName == "An OTACON") {
    animationOTACON();
  } else if (actionName == "An SNIPER WOLF") {
    animationSNIPERWOLF();
  } else if (actionName == "An MERYL") {
    animationMERYL();
  } else if (actionName == "An MEI LING") {
    animationMEILING();
  } else if (actionName == "An NASTASHA") {
    animationNASTASHA();
  } else if (actionName == "Incoming Call") {
    incomingCall();
  } else if (actionName == "Brightness") {
    //adjust backlight pwm
  } else if (actionName == "Restart System") {
    tft.println("Restarting in 3 seconds...");
    delay(3000);
    ESP.restart();
  } else if (actionName == "Sleep") {
    tft.fillScreen(ColorCodecBG);
    tft.setTextColor(ColorCodecMainText);
    tft.setTextSize(2);
    tft.setCursor(20, 100);
    tft.println("Goodbye!");
    delay(2000);
    //DO ESP SLEEP HERE, setup button input for power on.
  } else if (actionName == "Business Card") {
    delay(2000);
    //do more here
  } else if (actionName == "LinkedIn") {
    delay(2000);
    //do more here
  } else if (actionName == "Github") {
    delay(2000);
    //do more here
  } else if (actionName == "Incoming Call") {
    delay(2000);
    //do more here
  } else {
    tft.println("Action completed!");
  }
  
  // Wait for any button press to return to menu
  // Reset all button flags first
  menuButtonPressed = false;
  selectButtonPressed = false;
  upButtonPressed = false;
  downButtonPressed = false;
  
  // Wait for button press
  while (!menuButtonPressed && !selectButtonPressed && 
         !upButtonPressed && !downButtonPressed) {
    delay(50);
  }
  
  // Clear the pressed flag
  menuButtonPressed = false;
  selectButtonPressed = false;
  upButtonPressed = false;
  downButtonPressed = false;
  
  // Redraw current menu
  drawMenu();
}
