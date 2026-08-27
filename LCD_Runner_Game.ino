#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 4, 5, 6, 7);
const int jumpButtonPin = 8;
const int duckButtonPin = 10;
const int buzzerPin = 9;

enum GameState { MENU, PLAYING, GAME_OVER };
GameState state = MENU;

// Timing variables
unsigned long lastFrameTime = 0;
const int frameRate = 120; 

// Player logic
int playerY = 1; 
bool isJumping = false;
bool isDucking = false;
unsigned long jumpStartTime = 0;
const int jumpDuration = 600; 

// Obstacle logic (0 = Cactus, 1 = Ptero)
int obstacleX = 15;
int obstacleType = 0; 
int score = 0;

// Animation states
bool frameFlag = false; 
bool lastJumpState = false; 

// Custom Sprites (5x8 Pixel Art)
byte dinoRun1[8]  = { B00111, B00101, B00111, B10110, B11111, B01010, B01010, B00000 };
byte dinoRun2[8]  = { B00111, B00101, B00111, B10110, B11111, B01010, B00100, B00010 };
byte cactus[8]    = { B00100, B00101, B10101, B10111, B11111, B00100, B00100, B00100 };
byte ptero1[8]    = { B00100, B01101, B11111, B01100, B00100, B00000, B00000, B00000 };
byte ptero2[8]    = { B00000, B00100, B01100, B11111, B01101, B00100, B00000, B00000 };
byte dinoDuck1[8] = { B00000, B00000, B00000, B01110, B11111, B01010, B01010, B00000 };
byte dinoDuck2[8] = { B00000, B00000, B00000, B01110, B11111, B01010, B00100, B00010 };

void setup() {
  lcd.begin(16, 2);
  pinMode(jumpButtonPin, INPUT);
  pinMode(duckButtonPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  
  // Load sprites into CGRAM
  lcd.createChar(0, dinoRun1);
  lcd.createChar(1, dinoRun2);
  lcd.createChar(2, cactus);
  lcd.createChar(3, ptero1);
  lcd.createChar(4, ptero2);
  lcd.createChar(5, dinoDuck1);
  lcd.createChar(6, dinoDuck2);

  // Initialize PRNG using analog noise
  randomSeed(analogRead(0));
}

void loop() {
  unsigned long currentMillis = millis();
  bool jumpPressed = (digitalRead(jumpButtonPin) == HIGH);
  bool duckPressed = (digitalRead(duckButtonPin) == HIGH);
  
  bool justPressedJump = (jumpPressed && !lastJumpState);
  lastJumpState = jumpPressed;

  switch (state) {
    case MENU:
      lcd.setCursor(0, 0);
      lcd.print("   LCD Runner   ");
      lcd.setCursor(0, 1);
      lcd.print(" Press JUMP...  ");
      
      if (justPressedJump) { 
        state = PLAYING;
        score = 0;
        obstacleX = 15;
        obstacleType = 0;
        lcd.clear();
        tone(buzzerPin, 1000, 100);
        delay(200); 
      }
      break;

    case PLAYING:
      // 1. Duck logic (Hold to duck)
      if (duckPressed && !isJumping) {
        isDucking = true;
      } else {
        isDucking = false;
      }

      // 2. Jump logic (Disabled while ducking)
      if (justPressedJump && !isJumping && !isDucking) {
        isJumping = true;
        playerY = 0; 
        jumpStartTime = currentMillis;
        tone(buzzerPin, 800, 50); 
        lcd.setCursor(1, 1);
        lcd.print(" ");
      }

      // 3. Landing logic
      if (isJumping && (currentMillis - jumpStartTime > jumpDuration)) {
        isJumping = false;
        playerY = 1; 
        lcd.setCursor(1, 0);
        lcd.print(" ");
      }

      // 4. Game Engine (Frame advancement)
      if (currentMillis - lastFrameTime >= frameRate) {
        lastFrameTime = currentMillis;
        frameFlag = !frameFlag;

        // Clear previous obstacle if visible
        if (obstacleX >= 0 && obstacleX <= 15) {
          lcd.setCursor(obstacleX, obstacleType == 0 ? 1 : 0);
          lcd.print(" "); 
        }

        obstacleX--;

        // Spawn new obstacle
        if (obstacleX < 0) {
          obstacleX = random(15, 24);
          obstacleType = random(0, 2);
          score++;
          tone(buzzerPin, 1500, 30); 
        }

        // Collision Detection
        bool isHit = false;
        if (obstacleX == 1) {
          if (obstacleType == 0 && playerY == 1) {
            isHit = true;
          }
          if (obstacleType == 1 && !isDucking) {
            isHit = true;
          }
        }

        if (isHit) {
          state = GAME_OVER;
          tone(buzzerPin, 150, 600); 
          break; 
        }

        // Rendering
        lcd.setCursor(1, playerY);
        if (isJumping) {
          lcd.write(byte(0));
        } else if (isDucking) {
          lcd.write(byte(frameFlag ? 5 : 6));
        } else {
          lcd.write(byte(frameFlag ? 0 : 1));
        }

        // Render obstacle if on screen
        if (obstacleX <= 15) {
          if (obstacleType == 0) {
            lcd.setCursor(obstacleX, 1);
            lcd.write(byte(2));
          } else {
            lcd.setCursor(obstacleX, 0);
            lcd.write(byte(frameFlag ? 3 : 4));
          }
        }

        // Render score
        lcd.setCursor(13, 0);
        if(score < 100) lcd.print("0");
        if(score < 10) lcd.print("0");
        lcd.print(score);
      }
      break;

    case GAME_OVER:
      lcd.setCursor(0, 0);
      lcd.print("   GAME OVER!   ");
      lcd.setCursor(0, 1);
      lcd.print("Score: ");
      lcd.print(score);
      lcd.print("       ");
      
      if (justPressedJump) { 
        state = MENU;
        delay(300); 
      }
      break;
  }
}
