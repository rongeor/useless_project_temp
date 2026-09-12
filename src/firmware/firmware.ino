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
const int PIN_TRIG   = 11;
const int PIN_ECHO   = 12;

const int STEPS_PER_LEG = 25;
const int OBSTACLE_DISTANCE_CM = 20; // Trigger distance in cm

String targetDestination = "";

void soundBeep(unsigned int freq, unsigned long ms) {
  tone(PIN_BUZZER, freq, ms);
  delay(ms);
  noTone(PIN_BUZZER);
}

// Measure distance in centimeters using HC-SR04
long readUltrasonicCM() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  long duration = pulseIn(PIN_ECHO, HIGH, 25000); // 25ms timeout (~4 meters)
  if (duration == 0) return 999;                  // No echo received
  return duration * 0.034 / 2;
}

// Blocks until button is pressed, or returns TRUE early if an obstacle is spotted
bool waitForWalkOrObstacle() {
  Serial.println(F("[WALKING] Pacing forward... Ultrasonic collision avoidance active."));

  while (true) {
    // 1. Check Button Press
    if (digitalRead(PIN_BUTTON) == LOW) {
      soundBeep(2200, 80);
      while (digitalRead(PIN_BUTTON) == LOW) { delay(10); } // Wait for release
      delay(100);
      return false; // Completed walk normally
    }

    // 2. Check Ultrasonic Distance
    long dist = readUltrasonicCM();
    if (dist > 0 && dist <= OBSTACLE_DISTANCE_CM) {
      return true; // Obstacle detected!
    }

    delay(60); // Polling interval
  }
}

// Regular button-wait function for turns and idle screens
void waitForButtonOnly() {
  while (digitalRead(PIN_BUTTON) == HIGH) {
    delay(10);
  }
  soundBeep(2200, 80);
  while (digitalRead(PIN_BUTTON) == LOW) {
    delay(10);
  }
  delay(100);
}

void drawArrowUp() {
  display.fillTriangle(64, 4, 46, 24, 82, 24, SH110X_WHITE);
  display.fillRect(58, 24, 12, 16, SH110X_WHITE);
}

void drawArrowRight() {
  display.fillTriangle(84, 22, 64, 6, 64, 38, SH110X_WHITE);
  display.fillRect(44, 16, 20, 12, SH110X_WHITE);
}

void showTurnDirective() {
  display.clearDisplay();
  drawArrowRight();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(16, 44);
  display.println(F("COURSE VECTOR"));
  display.setCursor(10, 54);
  display.println(F("TURN 90*R >PRESS<"));
  display.display();

  Serial.println(F("TURN: Rotate 90 degrees right, then press button."));

  soundBeep(1300, 120);
  delay(80);
  soundBeep(1300, 200);
  waitForButtonOnly();
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
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  display.begin(i2c_Address, true);
  showSelectDestinationScreen();
}

void executeMockJourney() {
  // 1. Initial Mock Pathfinding Display
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

  Serial.print(F("Routing directly to: "));
  Serial.println(targetDestination);

  for (int i = 0; i < 4; i++) {
    soundBeep(900 + (i * 200), 80);
    delay(150);
  }

  // 2. The 4-Leg Navigation Loop
  int leg = 1;
  while (leg <= 4) {
    // --- Step A: Forward Leg Directive ---
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

    Serial.print(F("LEG "));
    Serial.print(leg);
    Serial.println(F("/4: Walk straight ahead."));

    soundBeep(1800, 150);

    // Watch for button click OR an obstacle
    bool obstacleEncountered = waitForWalkOrObstacle();

    if (obstacleEncountered) {
      // Emergency Obstacle Screen
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SH110X_WHITE);
      display.setCursor(12, 10);
      display.println(F("! OBSTACLE DETECTED !"));
      display.setCursor(6, 30);
      display.println(F("EMERGENCY REROUTE"));
      display.setCursor(8, 48);
      display.println(F("FORCING IMMEDIATE TURN"));
      display.display();

      Serial.println(F("ALERT: Obstacle detected within 20cm! Rerouting with forced turn..."));

      // Alarm warning beeps
      soundBeep(700, 100); delay(50);
      soundBeep(700, 100); delay(50);
      soundBeep(700, 250);
      delay(1000);

      // Force course alteration
      showTurnDirective();
      leg++; // Advance leg count
      continue;
    }

    // --- Step B: Regular Turn (Legs 1-3) ---
    if (leg < 4) {
      showTurnDirective();
    }
    leg++;
  }

  // 3. Triumphant Arrival
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

  // Fanfare
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