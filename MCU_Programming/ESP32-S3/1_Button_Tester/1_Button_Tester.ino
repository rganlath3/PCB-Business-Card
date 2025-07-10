/*
  By Ranil Ganlath
  This is for testing the pushbuttons on the card with the QT Py ESP32-S3 with No PSRAM.
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
  TBD
*/

#define downButton 18
#define upButton 17
#define selectButton 9
#define menuButton 8

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // make the pushbutton's pin an input:
  pinMode(downButton, INPUT);
  pinMode(upButton, INPUT);
  pinMode(selectButton, INPUT);
  pinMode(menuButton, INPUT);
  Serial.println("Button Tester for PCB Business Card");
  Serial.println("By Ranil Ganlath");
}

// the loop routine runs over and over again forever:
void loop() {
  // print out the state of the button:
  Serial.print("Menu Button:");
  Serial.print(digitalRead(menuButton));
  Serial.print("   Select Button:");
  Serial.print(digitalRead(selectButton));
  Serial.print("   Up Button:");
  Serial.print(digitalRead(upButton));
  Serial.print("   Down Button:");
  Serial.print(digitalRead(downButton));
  Serial.println(" ");
  Serial.println(" ");
  delay(200);        // delay in between reads for stability
}
