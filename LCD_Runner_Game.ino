#include <LiquidCrystal.h>

// RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 4, 5, 6, 7); 

const int buttonPin = 8;
const int buzzerPin = 9;

void setup() {

  lcd.begin(16, 2);
  
  lcd.print("Hello World!");
  
  lcd.setCursor(0, 1);
  lcd.print("Arcade Mahdi");

  pinMode(buttonPin, INPUT); 
  pinMode(buzzerPin, OUTPUT);
}

void loop() {

  if (digitalRead(buttonPin) == HIGH) {
    //(1000 Hz)
    lcd.setCursor(0, 0);
    lcd.print("Button PRESSED");
    tone(buzzerPin, 1000); 
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Hello World!    ");
    noTone(buzzerPin);
  }
}
