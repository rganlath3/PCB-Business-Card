/*
  By Ranil Ganlath
  This is for testing bitmaps with menu with the IPS Display and the QT Py ESP32-S3 with No PSRAM.
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

// Button debouncing variables
bool lastMenuState = LOW;
bool lastSelectState = LOW;
bool lastUpState = LOW;
bool lastDownState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

// Display parameters
const int MENU_ITEM_HEIGHT = 25;
const int MENU_START_Y = 30;
const int MAX_VISIBLE_ITEMS = 7;
int scrollOffset = 0;

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
  {"An CAMPBELL", 0, nullptr, true},
  {"An OTACON", 0, nullptr, true}
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
  {"Animated", 2, animatedItems, false},
  {"Settings", 2, settingsItems, false},
  {"QR", 3, QRItems, false},  
  {"Games", 0, nullptr, true},
  {"Sleep", 0, nullptr, true}
};

// Root menu
MenuItem rootMenu = {"Main Menu", 7, mainMenuItems, false};

void setup() {
  Serial.begin(115200);
  
  // Initialize buttons (external pulldown resistors)
  pinMode(downButton, INPUT);
  pinMode(upButton, INPUT);
  pinMode(selectButton, INPUT);
  pinMode(menuButton, INPUT);
  
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
  // Read button states (with external pulldown resistors)
  bool menuButtonState = digitalRead(menuButton);
  bool selectButtonState = digitalRead(selectButton);
  bool upButtonState = digitalRead(upButton);
  bool downButtonState = digitalRead(downButton);
  
  // Handle button presses with debouncing
  if (menuButtonState && !lastMenuState && (millis() - lastDebounceTime) > debounceDelay) {
    handleMenuButton();
    lastDebounceTime = millis();
  }
  
  if (selectButtonState && !lastSelectState && (millis() - lastDebounceTime) > debounceDelay) {
    handleSelectButton();
    lastDebounceTime = millis();
  }
  
  if (upButtonState && !lastUpState && (millis() - lastDebounceTime) > debounceDelay) {
    handleUpButton();
    lastDebounceTime = millis();
  }
  
  if (downButtonState && !lastDownState && (millis() - lastDebounceTime) > debounceDelay) {
    handleDownButton();
    lastDebounceTime = millis();
  }
  
  // Update button states
  lastMenuState = menuButtonState;
  lastSelectState = selectButtonState;
  lastUpState = upButtonState;
  lastDownState = downButtonState;
  delay(50);
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

void executeAction(String actionName) {
  // Clear screen and show action

  tft.fillScreen(ColorCodecBG );
  tft.setTextColor(ColorCodecMainText );
  tft.setTextSize(2);
  tft.setCursor(5, 50);
  tft.println("Executing:");
  
  tft.setTextColor(ColorCodecText );
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
    tft.pushImage(0,0,140,240,DEEPTHROAT);
  } else if (actionName == "An CAMPBELL") {
    tft.pushImage(0,0,140,240,CAMPBELL);
    delay(2000);
    tft.pushImage(27,92,91,91,CAMPBELLM);
    delay(2000);
    tft.pushImage(0,0,140,240,CAMPBELL);
    delay(2000);
    tft.pushImage(27,92,91,91,CAMPBELLM);
    //I want to loop this animation until a button is pressed.
  } else if (actionName == "An OTACON") {
    tft.pushImage(0,0,140,240,OTACON);
    //do more here
  } else if (actionName == "Brightness") {
    //adjust backlight pwm
  } else if (actionName == "Restart System") {
    tft.println("Restarting in 3 seconds...");
    delay(3000);
    ESP.restart();
  } else if (actionName == "Sleep") {
    tft.fillScreen(ColorCodecBG );
    tft.setTextColor(ColorCodecMainText );
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
  
  
  // Wait for button release first
  while (digitalRead(selectButton) || digitalRead(menuButton) || 
         digitalRead(upButton) || digitalRead(downButton)) {
    delay(50);
  }
  
  // Wait for button press
  while (!digitalRead(selectButton) && !digitalRead(menuButton) && 
         !digitalRead(upButton) && !digitalRead(downButton)) {
    delay(50);
  }
  
  // Redraw current menu
  drawMenu();
}
