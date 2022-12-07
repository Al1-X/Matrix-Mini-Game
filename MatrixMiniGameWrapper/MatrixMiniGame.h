/*
ATmega16:
16kb flash
1kb ram
512 kb eeprom
*/

#include "LedControl.h"
#include <Arduino.h>

const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;

const int xPin = A0; 
const int yPin = A1;
const int switchPin = 2;

byte matrixBrightness = 2;

byte xPosition = 0;
byte yPosition = 0;

byte xLastPosition = 0;
byte yLastPosition = 0;

byte currentFoodPositionX = 0;
byte currentFoodPositionY = 0;

const int joyMinThreshold = 200;
const int joyMaxThreshold = 600;

unsigned long joyLastMoveTime = 0;
const byte joyDebounceDuration = 200;
const byte defaultPosition = B10000000;

const byte matrixSize = 8;

int xValue;
int yValue;

const byte minRowNumber = 0;
const byte maxRowNumber = 8;
const byte minColumnNumber = 0;
const byte maxColumnNumber = 8;

unsigned int score = 0;

bool foodActive = false;
bool matrixChanged = false;

LedControl ledControl = LedControl(dinPin, clockPin, loadPin, 1);

byte matrixByte[matrixSize] = {
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000
};

byte foodRow;
void generateFood() {
  currentFoodPositionX = random(minRowNumber, maxRowNumber); 
  currentFoodPositionY = random(minColumnNumber, maxColumnNumber);

  if(currentFoodPositionX == xPosition && currentFoodPositionY == yPosition){
    generateFood();
  }

  byte xorByte = 1 << (matrixSize - 1 - currentFoodPositionY);
  matrixByte[currentFoodPositionX] = matrixByte[currentFoodPositionX] ^ xorByte;
  foodRow = matrixByte[currentFoodPositionX];

  foodActive = true;
  matrixChanged = true;
}

void reachFood() {
  if (currentFoodPositionX == yPosition &&
      currentFoodPositionY == xPosition) {
        score++;
        foodActive = false;

        matrixByte[yPosition] = 0;
    }
}

void updatePosition() {
  xValue = analogRead(xPin);
  yValue = analogRead(yPin);

  if (xValue < joyMinThreshold) {
    if (xPosition > 0) {
      xPosition--;
    }
    else xPosition = matrixSize -1;
  }
  else {
    if (xValue > joyMaxThreshold) {
      if (xPosition < matrixSize - 1) {
        xPosition++;
      }
      else xPosition = 0;
    }
  }

  if (yValue > joyMaxThreshold) {
    if (yPosition < matrixSize - 1) {
      yPosition++;
    }
    else yPosition = 0;
  }
  else {
    if (yValue < joyMinThreshold) {
      if (yPosition > 0) {
        yPosition--;
      }
      else yPosition = matrixSize - 1;
    }
  }
  
  // y pe joy tine randurile, x coloanele
  if (xPosition != xLastPosition || yPosition != yLastPosition) {
    byte xorByte = 1 << (matrixSize - 1 - xLastPosition);
    matrixByte[yLastPosition] = matrixByte[yLastPosition] ^ xorByte;
    xorByte = 1 << (matrixSize - 1 - xPosition);
    matrixByte[yPosition] = matrixByte[yPosition] ^ xorByte;

    if (yPosition == currentFoodPositionX && xPosition == currentFoodPositionY) {
        matrixByte[yPosition] = foodRow;
    }
    
    matrixChanged = true;
  }

  xLastPosition = xPosition;
  yLastPosition = yPosition;
}

void updateMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    ledControl.setRow(0, row, matrixByte[row]);
  }
}

unsigned long lastBlinkTime = 0;
void blink() {
  if (millis() - lastBlinkTime > 300) {
    lastBlinkTime = millis();

    byte xorByte = 1 << (matrixSize - 1 - currentFoodPositionY);
    matrixByte[currentFoodPositionX] = matrixByte[currentFoodPositionX] ^ xorByte; 
    ledControl.setRow(0, currentFoodPositionX, matrixByte[currentFoodPositionX]);
  }
}

void matrixReset() {
  xPosition = 0;
  yPosition = 0;
  xLastPosition = 0;
  yLastPosition = 0;
  currentFoodPositionX = 0;
  currentFoodPositionY = 0;

  for (int i = 0; i < matrixSize; i++) {
    matrixByte[i] = 0;
  }

  foodActive = false;
  matrixChanged = false;
  joyLastMoveTime = 0;
}

void MatrixGame() {
  if (foodActive == false) {
    generateFood();
  }
  if (millis() - joyLastMoveTime > joyDebounceDuration) {
    updatePosition();
    joyLastMoveTime = millis();
  }
  
  if (matrixChanged == true) {
    updateMatrix();
    matrixChanged = false;
  }
  reachFood();
  blink();
}

void setupMatrix() {
  ledControl.shutdown(0, false);
  ledControl.setIntensity(0, matrixBrightness);
  ledControl.clearDisplay(0);
  
// make global var
  matrixByte[0] = defaultPosition;  
  ledControl.setRow(0, 0, matrixByte[0]);

  pinMode(switchPin, INPUT_PULLUP);  

  randomSeed(analogRead(0));
}
