#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define i2c_Address 0x3c
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int PIN_BUTTON = 2;
const int PIN_BUZZER = 8;
const int STEPS_PER_LEG = 25;

String targetDestination = "";

void soundBeep(unsigned int freq, unsigned long ms) {
  tone(PIN_BUZZER, freq, ms);
  delay(ms);
  noTone(PIN_BUZZER);
}

void waitForButtonPress() {
  while (digitalRead(PIN_BUTTON) == HIGH) {
    delay(10);
  }
  soundBeep(2200, 80);
  while (digitalRead(PIN_BUTTON) == LOW) {
    delay(10);
  }
  delay(150);
}

void drawArrowUp() {
  display.fillTriangle(64, 4, 46, 24, 82, 24, SH110X_WHITE);
  display.fillRect(58, 24, 12, 16, SH110X_WHITE);
}

void drawArrowRight() {
  display.fillTriangle(84, 22, 64, 6, 64, 38, SH110X_WHITE);
  display.fillRect(44, 16, 20, 12, SH110X_WHITE);
}

void showSelectDestinationScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(14, 15);
  display.println(F("AWAITING TARGET..."));
  display.setCursor(6, 35);
  display.println(F("SELECT DESTINATION"));
  display.setCursor(24, 48);
  display.println(F("ON LAPTOP"));
  display.display();
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);

  display.begin(i2c_Address, true);
  showSelectDestinationScreen();
}

void executeMockJourney() {
  // 1. Mock Pathfinding Display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 10);
  display.print(F("TARGET: "));
  display.println(targetDestination);
  display.setCursor(0, 30);
  display.println(F("OPTIMIZING TRAJECTORY"));
  display.setCursor(0, 45);
  display.println(F("AVOIDING DETOURS..."));
  display.display();

  Serial.print(F("Calculating direct-line vector to: "));
  Serial.println(targetDestination);

  for (int i = 0; i < 4; i++) {
    soundBeep(900 + (i * 200), 80);
    delay(150);
  }

  // 2. Interactive Navigation Loop
  for (int leg = 1; leg <= 4; leg++) {
    // --- Walk Leg ---
    display.clearDisplay();
    drawArrowUp();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(28, 44);
    display.print(F("LEG "));
    display.print(leg);
    display.println(F("/4: AHEAD"));
    display.setCursor(8, 54);
    display.print(F("WALK "));
    display.print(STEPS_PER_LEG);
    display.println(F("P >PRESS<"));
    display.display();

    // Mirror to Laptop
    Serial.print(F("LEG "));
    Serial.print(leg);
    Serial.print(F("/4: Walk "));
    Serial.print(STEPS_PER_LEG);
    Serial.println(F(" paces straight, then press the device button."));

    soundBeep(1800, 150);
    waitForButtonPress();

    // --- Turn Leg ---
    if (leg < 4) {
      display.clearDisplay();
      drawArrowRight();
      display.setTextSize(1);
      display.setTextColor(SH110X_WHITE);
      display.setCursor(16, 44);
      display.println(F("COURSE VECTOR"));
      display.setCursor(10, 54);
      display.println(F("TURN 90*R >PRESS<"));
      display.display();

      // Mirror to Laptop
      Serial.println(F("TURN: Rotate 90 degrees right, then press the device button."));

      soundBeep(1300, 120);
      delay(80);
      soundBeep(1300, 200);
      waitForButtonPress();
    }
  }

  // 3. Arrival
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(4, 10);
  display.println(F("ROUTE FULLY RESOLVED"));

  display.setTextSize(2);
  display.setCursor(0, 26);
  display.println(F("DESTINATION"));
  display.setCursor(20, 46);
  display.println(F("REACHED"));
  display.display();

  Serial.println(F("DESTINATION REACHED"));

  // Victory Fanfare
  soundBeep(523, 150); delay(40);
  soundBeep(659, 150); delay(40);
  soundBeep(784, 150); delay(40);
  soundBeep(1046, 500);

  delay(5000);
  showSelectDestinationScreen();
}

void loop() {
  if (Serial.available() > 0) {
    String msg = Serial.readStringUntil('\n');
    msg.trim();
    if (msg.startsWith("START:")) {
      targetDestination = msg.substring(6);
      executeMockJourney();
    }
  }
}