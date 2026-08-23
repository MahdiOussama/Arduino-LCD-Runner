#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 4, 5, 6, 7);
const int buttonPin = 8;
const int buzzerPin = 9;

// Game states
enum GameState { MENU, PLAYING, GAME_OVER };
GameState state = MENU;

// Timing variables
unsigned long lastFrameTime = 0;
const int frameRate = 150; 

// Player and world logic
int playerY = 1; // 1 = ground, 0 = jumping
bool isJumping = false;
unsigned long jumpStartTime = 0;
const int jumpDuration = 600; 
int obstacleX = 15;
int score = 0;
bool dinoFrame = false; 

// Custom sprites (5x8 pixel art)
byte dinoCorsa1[8] = { B00111, B00101, B00111, B10110, B11111, B01010, B01010, B00000 };
byte dinoCorsa2[8] = { B00111, B00101, B00111, B10110, B11111, B01010, B00100, B00010 };
byte ostacolo[8]   = { B00100, B00101, B10101, B10111, B11111, B00100, B00100, B00100 };

void setup() {
  lcd.begin(16, 2);
  pinMode(buttonPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  
  // Load sprites into LCD CGRAM
  lcd.createChar(0, dinoCorsa1);
  lcd.createChar(1, dinoCorsa2);
  lcd.createChar(2, ostacolo);
}

void loop() {
  unsigned long currentMillis = millis();
  bool buttonPressed = (digitalRead(buttonPin) == HIGH);

  switch (state) {
    
    // --- START SCREEN ---
    case MENU:
      lcd.setCursor(0, 0);
      lcd.print("  Arcade Oussama  ");
      lcd.setCursor(0, 1);
      lcd.print(" Press to Start ");
      
      if (buttonPressed) {
        state = PLAYING;
        score = 0;
        obstacleX = 15;
        lcd.clear();
        tone(buzzerPin, 1000, 100);
        delay(200); // Start debounce
      }
      break;

    // --- GAMEPLAY ---
    case PLAYING:
      // 1. Non-blocking input read
      if (buttonPressed && !isJumping) {
        isJumping = true;
        playerY = 0; 
        jumpStartTime = currentMillis;
        tone(buzzerPin, 800, 50); 
      }

      // 2. Gravity logic
      if (isJumping && (currentMillis - jumpStartTime > jumpDuration)) {
        isJumping = false;
        playerY = 1; 
        lcd.setCursor(1, 0);
        lcd.print(" "); // Clear ghosting
      }

      // 3. Rendering and physics (per frame)
      if (currentMillis - lastFrameTime >= frameRate) {
        lastFrameTime = currentMillis;

        lcd.setCursor(obstacleX, 1);
        lcd.print(" "); // Erase previous obstacle

        obstacleX--;
        if (obstacleX < 0) {
          obstacleX = 15; 
          score++;
          tone(buzzerPin, 1500, 30); 
        }

        // Collision detection
        if (obstacleX == 1 && playerY == 1) {
          state = GAME_OVER;
          tone(buzzerPin, 150, 600); 
          break; 
        }

        // Render Player
        lcd.setCursor(1, playerY);
        if (isJumping) {
          lcd.write(byte(0)); 
        } else {
          lcd.write(byte(dinoFrame ? 0 : 1)); 
          dinoFrame = !dinoFrame;
        }

        // Render Obstacle
        lcd.setCursor(obstacleX, 1);
        lcd.write(byte(2));

        // Render Score
        lcd.setCursor(13, 0);
        if(score < 100) lcd.print("0");
        if(score < 10) lcd.print("0");
        lcd.print(score);
      }
      break;

    // --- GAME OVER SCREEN ---
    case GAME_OVER:
      lcd.setCursor(0, 0);
      lcd.print("   GAME OVER!   ");
      lcd.setCursor(0, 1);
      lcd.print("Score: ");
      lcd.print(score);
      lcd.print("       "); 
      
      if (buttonPressed) {
        state = MENU;
        delay(300); // Reset debounce
      }
      break;
  }
}
