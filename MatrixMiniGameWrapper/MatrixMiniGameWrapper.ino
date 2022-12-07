/*
ATmega16:
16kb flash
1kb ram
512 kb eeprom
*/

#include "LedControl.h"
#include "MatrixMiniGame.h"
#include "LCD.h"

byte pointerArrow[] = {
  B00000,
  B01000,
  B01100,
  B01110,
  B01110,
  B01100,
  B01000,
  B00000
};

byte cat[] = {
  B10001,
  B11111,
  B10101,
  B11111,
  B00100,
  B01110,
  B01110,
  B01010
};

byte mouse[] = {
  B11011,
  B11111,
  B10101,
  B01110,
  B00100,
  B01110,
  B01110,
  B01010
};

bool actionStarted = false;
unsigned long elapsedTime = 0;
byte buttonState;
byte lastButtonState = HIGH;
unsigned long lastButtonChange;
unsigned long buttonChange;
unsigned int debounceDuration = 200;

char * title = "                The cat and the mouse!                ";

byte systemState = 0;   // 0 main menu    1 game    2 exit screen    3 info screen

void setup() {
  Serial.begin(9600);

  // affects systate
  setupMatrix();

  pinMode(blackLightPin, OUTPUT);
  pinMode(contrastPin, OUTPUT);

  lcd.createChar(0, pointerArrow);
  lcd.createChar(1, cat);
  lcd.createChar(2, mouse);

  lcd.begin(16, 2);
  lcd.setCursor(4, 0);
  lcd.print("Welcome");
  delay(500);
  lcd.setCursor(7, 1);
  lcd.print("to");
  delay(750);
  lcd.clear();

  lcd.setCursor(6, 1);
  lcd.write((byte)1);
  lcd.setCursor(9, 1);
  lcd.write((byte)2);
  printLongText(0, 0, title);

  lcd.clear();
  lastButtonChange = 0;
}


void loop() {
  buttonState = digitalRead(switchPin);

  analogWrite(contrastPin, 100);
  analogWrite(blackLightPin, 100);

  if (buttonState != lastButtonState && lastButtonState == HIGH) {
    switch (systemState) {
      case 0:
        setupMatrix();
        lcd.clear();
        systemState = 1;
        break;
      case 1:
        lcd.clear();
        systemState = 2;
        break;
      case 2:
        lcd.clear();
        systemState = 3;
        break;
      case 3:
        score = 0;
        // matrix variables reset
        lcd.clear();
        systemState = 0;
        break;
    }
  }

  switch (systemState) {
    case 0:
    Serial.println("State 0");
      lcd.setCursor(0, 0);
      lcd.write((byte)0);
      lcd.setCursor(1, 0);
      lcd.print("Start");
      break;
    case 1:
      Serial.println("State 1");
      MatrixGame();
      // stop lcd from always writing
      lcd.setCursor(0, 0);
      lcd.print("Score:");
      lcd.setCursor(7, 0);
      lcd.print(score);
      delay(150);
      break;
    case 2:
      Serial.println("State 2");
      lcd.setCursor(0, 0);
      lcd.print("Playtime over!");
      lcd.setCursor(6, 1);
      lcd.write((byte)1);
      lcd.setCursor(9, 1);
      lcd.write((byte)2);

      break;
    case 3:
      Serial.println("State 3");
      lcd.setCursor(0, 0);
      lcd.print("Score: " + String(score));
      lcd.setCursor(0, 1);
      lcd.print("U are a winner!!");
      break;
  }

  lastButtonState = buttonState;
}