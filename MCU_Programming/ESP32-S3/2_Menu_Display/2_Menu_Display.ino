/*
  By Ranil Ganlath
  This is for testing basic menus with the IPS Display and the QT Py ESP32-S3 with No PSRAM.
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
MenuItem settingsWifiItems[] = {
  {"Connect WiFi", 0, nullptr, true},
  {"Disconnect WiFi", 0, nullptr, true},
  {"WiFi Status", 0, nullptr, true},
  {"Scan Networks", 0, nullptr, true}
};

MenuItem settingsDisplayItems[] = {
  {"Brightness", 0, nullptr, true},
  {"Contrast", 0, nullptr, true},
  {"Orientation", 0, nullptr, true},
  {"Sleep Timer", 0, nullptr, true}
};

MenuItem settingsItems[] = {
  {"WiFi Settings", 4, settingsWifiItems, false},
  {"Display Settings", 4, settingsDisplayItems, false},
  {"Reset Settings", 0, nullptr, true},
  {"About", 0, nullptr, true}
};

MenuItem systemToolsItems[] = {
  {"System Info", 0, nullptr, true},
  {"Memory Usage", 0, nullptr, true},
  {"Restart System", 0, nullptr, true},
  {"Factory Reset", 0, nullptr, true}
};

MenuItem sensorsItems[] = {
  {"Temperature", 0, nullptr, true},
  {"Humidity", 0, nullptr, true},
  {"Pressure", 0, nullptr, true},
  {"Light Level", 0, nullptr, true}
};

// Main menu items
MenuItem mainMenuItems[] = {
  {"Settings", 4, settingsItems, false},
  {"System Tools", 4, systemToolsItems, false},
  {"Sensors", 4, sensorsItems, false},
  {"Games", 0, nullptr, true},
  {"Exit", 0, nullptr, true}
};

// Root menu
MenuItem rootMenu = {"Main Menu", 5, mainMenuItems, false};

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
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  
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
  tft.fillScreen(TFT_BLACK);
  
  // Draw title
  tft.setTextColor(TFT_CYAN);
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
      tft.fillRect(0, yPos - 2, 135, MENU_ITEM_HEIGHT - 2, TFT_BLUE);
      tft.setTextColor(TFT_WHITE);
    } else {
      tft.setTextColor(TFT_YELLOW);
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
    tft.setTextColor(TFT_GREEN);
    tft.setCursor(120, MENU_START_Y);
    tft.print("^");
  }
  
  if (scrollOffset + MAX_VISIBLE_ITEMS < itemCount) {
    tft.setTextColor(TFT_GREEN);
    tft.setCursor(120, MENU_START_Y + (MAX_VISIBLE_ITEMS - 1) * MENU_ITEM_HEIGHT);
    tft.print("v");
  }
  
  // Draw navigation help
  tft.setTextColor(TFT_BLUE);
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
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(2);
  tft.setCursor(5, 50);
  tft.println("Executing:");
  
  tft.setTextColor(TFT_WHITE);
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
  } else if (actionName == "System Info") {
    tft.println("ESP32-S3 Info:");
    tft.setCursor(5, 130);
    tft.println("Free Heap: " + String(ESP.getFreeHeap()));
    tft.setCursor(5, 150);
    tft.println("CPU Freq: " + String(ESP.getCpuFreqMHz()) + " MHz");
  } else if (actionName == "Restart System") {
    tft.println("Restarting in 3 seconds...");
    delay(3000);
    ESP.restart();
  } else if (actionName == "Temperature") {
    tft.println("Temperature: 25.6°C");
  } else if (actionName == "Exit") {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_RED);
    tft.setTextSize(2);
    tft.setCursor(20, 100);
    tft.println("Goodbye!");
    delay(2000);
  } else {
    tft.println("Action completed!");
  }
  
  // Wait for button press to continue
  tft.setTextColor(TFT_BLUE);
  tft.setCursor(5, 200);
  tft.println("Press any button to continue");
  
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
