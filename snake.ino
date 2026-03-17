#include <Adafruit_ILI9341.h>
#include <SPI.h>

// ILI9341 TFT
#define TFT_CS 10
#define TFT_RST 8
#define TFT_DC 9

// MOSI, MISO, SCK are hardware SPI on Arduino Mega:
// MOSI = Pin 51
// MISO = Pin 50
// SCK = Pin 52

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// Pins
#define JOYSTICK_VERT A1
#define JOYSTICK_HORZ A2
#define JOYSTICK_SEL 42
#define PUSH_BUTTON 44
#define BUZZER 40

// Game settings
#define GRID_WIDTH 32    // Number of cells horizontally
#define GRID_HEIGHT 20   // Number of cells vertically
#define CELL_SIZE 10     // Size of each cell in pixels
#define MAX_SNAKE_LENGTH 320

// Game grid dimensions in pixels
#define GAME_WIDTH (GRID_WIDTH * CELL_SIZE)   // 320 pixels
#define GAME_HEIGHT (GRID_HEIGHT * CELL_SIZE) // 200 pixels
#define GAME_X 0
#define GAME_Y 20

// colors
#define COLOR_BG ILI9341_BLACK
#define COLOR_SNAKE ILI9341_GREEN
#define COLOR_HEAD ILI9341_YELLOW
#define COLOR_FOOD ILI9341_RED
#define COLOR_GRID ILI9341_DARKGREY
#define COLOR_TEXT ILI9341_WHITE

// Direction constants
#define DIR_UP 0
#define DIR_DOWN 1
#define DIR_LEFT 2
#define DIR_RIGHT 3

// Game state
struct SnakeSegment {
  int x;
  int y;
};

SnakeSegment snake[MAX_SNAKE_LENGTH];
int snakeLength = 3;
int direction = DIR_RIGHT;
int nextDirection = DIR_RIGHT;

int foodX = 16;
int foodY = 10;

boolean gameActive = false;
boolean gameOver = false;
int score = 0;
int speed = 150; // milliseconds between moves

unsigned long lastMoveTime = 0;
unsigned long lastInputTime = 0;
const int INPUT_DEBOUNCE = 100;

//  render
int lastHeadX = -1, lastHeadY = -1;
int lastTailX = -1, lastTailY = -1;
int lastFoodX = -1, lastFoodY = -1;
int lastScore = -1;

void setup() {
  // init display
  tft.begin();
  tft.setRotation(1); // set your tft to landscape if its in simulation e.g wokwi
  
  // Setup pins
  pinMode(JOYSTICK_SEL, INPUT);
  pinMode(PUSH_BUTTON, INPUT);
  pinMode(BUZZER, OUTPUT);
  
  Serial.begin(9600);
  randomSeed(analogRead(A15));
  
  showTitleScreen();
}

void loop() {
  if (!gameActive && !gameOver) {
    if (digitalRead(PUSH_BUTTON) == HIGH) {
      initializeGame();
      gameActive = true;
      lastMoveTime = millis();
      delay(500);
    }
  } 
  else if (gameActive) {
    readJoystick();
    
    if (millis() - lastMoveTime >= speed) {
      updateGame();
      lastMoveTime = millis();
      
      if (score > 0 && score % 5 == 0) {
        speed = max(80, speed - 5);
      }
    }
    
    renderGame();
  } 
  else if (gameOver) {
    if (digitalRead(PUSH_BUTTON) == HIGH) {
      gameActive = false;
      gameOver = false;
      showTitleScreen();
      delay(500);
    }
  }
  
  delay(20);
}

void showTitleScreen() {
  tft.fillScreen(COLOR_BG);
  
  tft.setTextSize(3);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(50, 80);
  tft.println("SNAKE GAME");
  
  tft.setTextSize(2);
  tft.setCursor(40, 160);
  tft.println("Press button to start");
  
  tft.setTextSize(1);
  tft.setCursor(60, 200);
  tft.println("Use joystick to move");
}

void initializeGame() {
  tft.fillScreen(COLOR_BG);
  
  drawGameBoundary();
  
  snakeLength = 3;
  snake[0].x = 16; 
  snake[0].y = 10;
  snake[1].x = 15;
  snake[1].y = 10;
  snake[2].x = 14;
  snake[2].y = 10;
  
  direction = DIR_RIGHT;
  nextDirection = DIR_RIGHT;
  
  score = 0;
  speed = 80; // game speed
  gameOver = false;
  
  lastHeadX = -1;
  lastHeadY = -1;
  lastTailX = -1;
  lastTailY = -1;
  lastFoodX = -1;
  lastFoodY = -1;
  lastScore = -1;
  
  generateFood();
  lastMoveTime = millis();
  
  renderGame();
}

void drawGameBoundary() {
  tft.drawRect(GAME_X - 1, GAME_Y - 1, GAME_WIDTH + 2, GAME_HEIGHT + 2, ILI9341_WHITE);
  
  tft.setTextSize(1);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(5, 5);
  tft.println("Score: 0");
}

void readJoystick() {
  if (millis() - lastInputTime < INPUT_DEBOUNCE) {
    return;
  }
  
  int vertValue = analogRead(JOYSTICK_VERT);
  int horzValue = analogRead(JOYSTICK_HORZ);
  
  if (vertValue > 700 && direction != DIR_DOWN) {
    nextDirection = DIR_UP;
    lastInputTime = millis();
  }
  else if (vertValue < 300 && direction != DIR_UP) {
    nextDirection = DIR_DOWN;
    lastInputTime = millis();
  }
  else if (horzValue > 700 && direction != DIR_RIGHT) {
    nextDirection = DIR_LEFT;
    lastInputTime = millis();
  }
  else if (horzValue < 300 && direction != DIR_LEFT) {
    nextDirection = DIR_RIGHT;
    lastInputTime = millis();
  }
}

void updateGame() {
  direction = nextDirection;
  
  int newHeadX = snake[0].x;
  int newHeadY = snake[0].y;
  
  switch(direction) {
    case DIR_UP:
      newHeadY = (newHeadY - 1 + GRID_HEIGHT) % GRID_HEIGHT;
      break;
    case DIR_DOWN:
      newHeadY = (newHeadY + 1) % GRID_HEIGHT;
      break;
    case DIR_LEFT:
      newHeadX = (newHeadX - 1 + GRID_WIDTH) % GRID_WIDTH;
      break;
    case DIR_RIGHT:
      newHeadX = (newHeadX + 1) % GRID_WIDTH;
      break;
  }
  
  for (int i = 0; i < snakeLength; i++) {
    if (newHeadX == snake[i].x && newHeadY == snake[i].y) {
      endGame();
      return;
    }
  }
  
  boolean foodEaten = false;
  if (newHeadX == foodX && newHeadY == foodY) {
    score++;
    foodEaten = true;
    playSound(800, 100);
  }
  
  if (!foodEaten) {
    for (int i = snakeLength - 1; i > 0; i--) {
      snake[i].x = snake[i - 1].x;
      snake[i].y = snake[i - 1].y;
    }
  } else {
    for (int i = snakeLength; i > 0; i--) {
      snake[i].x = snake[i - 1].x;
      snake[i].y = snake[i - 1].y;
    }
    if (snakeLength < MAX_SNAKE_LENGTH) {
      snakeLength++;
    }
    generateFood();
    lastFoodX = -1;
    lastFoodY = -1;
  }
  
  snake[0].x = newHeadX;
  snake[0].y = newHeadY;
}

void generateFood() {
  boolean validPosition = false;
  
  while (!validPosition) {
    foodX = random(GRID_WIDTH);
    foodY = random(GRID_HEIGHT);
    
    validPosition = true;
    for (int i = 0; i < snakeLength; i++) {
      if (foodX == snake[i].x && foodY == snake[i].y) {
        validPosition = false;
        break;
      }
    }
  }
}

void renderGame() {
  // Draw head if it moved - only redraws when position changes
  // This is efficient because head moves every frame
  if (snake[0].x != lastHeadX || snake[0].y != lastHeadY) {
    int x = GAME_X + snake[0].x * CELL_SIZE;
    int y = GAME_Y + snake[0].y * CELL_SIZE;
    tft.fillRect(x + 1, y + 1, CELL_SIZE - 2, CELL_SIZE - 2, COLOR_HEAD);
    lastHeadX = snake[0].x;
    lastHeadY = snake[0].y;
  }
  
  // Clear old tail position when tail moves
  // This removes the tail pixel as snake moves forward (normal mode without food)
  // When food eaten, lastTailX/Y is reset to -1, skip this
  if (snakeLength > 0 && (snake[snakeLength - 1].x != lastTailX || snake[snakeLength - 1].y != lastTailY)) {
    int oldTailX = GAME_X + lastTailX * CELL_SIZE;
    int oldTailY = GAME_Y + lastTailY * CELL_SIZE;
    tft.fillRect(oldTailX, oldTailY, CELL_SIZE, CELL_SIZE, COLOR_BG);
    lastTailX = snake[snakeLength - 1].x;
    lastTailY = snake[snakeLength - 1].y;
  }
  
  // Draw food only when position changes (when newly generated)
  if (foodX != lastFoodX || foodY != lastFoodY) {
    if (lastFoodX != -1) {
      int oldFoodX = GAME_X + lastFoodX * CELL_SIZE;
      int oldFoodY = GAME_Y + lastFoodY * CELL_SIZE;
      tft.fillRect(oldFoodX, oldFoodY, CELL_SIZE, CELL_SIZE, COLOR_BG);
    }
    
    int foodPixelX = GAME_X + foodX * CELL_SIZE;
    int foodPixelY = GAME_Y + foodY * CELL_SIZE;
    tft.fillRect(foodPixelX + 2, foodPixelY + 2, CELL_SIZE - 4, CELL_SIZE - 4, COLOR_FOOD);
    lastFoodX = foodX;
    lastFoodY = foodY;
  }
  
  // Update score display only when score changes
  if (score != lastScore) {
    tft.setTextSize(1);
    tft.setTextColor(COLOR_BG);
    tft.setCursor(5, 5);
    tft.print("Score: 0       ");
    
    tft.setTextColor(COLOR_TEXT);
    tft.setCursor(5, 5);
    tft.print("Score: ");
    tft.println(score);
    lastScore = score;
  }
}

void drawGrid() {
  for (int x = 0; x <= GRID_WIDTH; x++) {
    int pixelX = GAME_X + x * CELL_SIZE;
    tft.drawLine(pixelX, GAME_Y, pixelX, GAME_Y + GAME_HEIGHT, COLOR_GRID);
  }
  
  for (int y = 0; y <= GRID_HEIGHT; y++) {
    int pixelY = GAME_Y + y * CELL_SIZE;
    tft.drawLine(GAME_X, pixelY, GAME_X + GAME_WIDTH, pixelY, COLOR_GRID);
  }
}

void endGame() {
  gameActive = false;
  gameOver = true;
  
  playSound(200, 200);
  delay(100);
  playSound(200, 200);
  
  tft.fillScreen(COLOR_BG);
  
  tft.setTextSize(3);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(60, 100);
  tft.println("GAME OVER");
  
  tft.setTextSize(2);
  tft.setCursor(80, 150);
  tft.print("Score: ");
  tft.println(score);
  
  tft.setTextSize(1);
  tft.setCursor(50, 200);
  tft.println("Press button to restart");
  
  Serial.print("Game Over! Score: ");
  Serial.println(score);
  
  // FIX: Wait for button release before returning to main loop
  // Problem: Button might still be held from the collision that killed the snake
  // Without this wait, main loop would immediately detect HIGH state and start new game
  // This blocking loop forces player to release button first
  while (digitalRead(PUSH_BUTTON) == HIGH) {
    delay(50);
  }
  
  // Additional 1 second delay prevents accidental re-press
  // Gives player time to see game over screen and prepare for restart
  delay(1000);
}

void playSound(int frequency, int duration) {
  tone(BUZZER, frequency, duration);
  delay(duration);
  noTone(BUZZER);
}
