#include <M5StickCPlus2.h>

// Define missing color constant
#define GRAY 0x8410

// Game states
enum GameState {
  MENU,
  WAITING_TO_CAST,
  FISHING,
  FISH_HOOKED,
  CAUGHT,
  MISSED_TURN,
  GAME_OVER
};

// Fish types for enhanced gameplay
enum FishType {
  NORMAL,
  GOLDEN,
  FAST,
  RARE
};

// Power-ups for increased interactivity
enum PowerUpType {
  NONE,
  BIG_HOOK,
  TIME_SLOW,
  DOUBLE_POINTS
};

GameState currentState = MENU;

// Timing variables
unsigned long fishingStartTime = 0;
unsigned long gameStartTime = 0;
unsigned long lastUpdate = 0;
unsigned long lastPartialUpdate = 0;
unsigned long missedTurnStartTime = 0;
unsigned long comboTimer = 0;
unsigned long powerUpTimer = 0;

// Game variables
int score = 0;
int highScore = 0;
int combo = 0;
int maxCombo = 0;
bool fishPresent = false;
int tapCount = 0;
int requiredTaps = 0;
unsigned long lastTapTime = 0;
PowerUpType activePowerUp = NONE;
unsigned long powerUpDuration = 0;
int difficulty = 1;

// Display optimization variables to reduce flashing
bool needsFullRedraw = true;
bool hookMoved = false;
bool fishMoved = false;
bool statusChanged = true;

// IMU variables
float accX, accY, accZ;
float gyroX, gyroY, gyroZ;
float prevAccX = 0, prevAccY = 0;

// Rod and string variables
int rodAngle = 0;
int prevRodAngle = 0;
int stringLength = 0;
int prevStringLength = 0;
int maxStringLength = 170;
int stringSpeed = 3;
bool stringExtending = false;
bool stringRetracting = false;
int hookX = 67;
int hookY = 50;
int prevHookX = 67;
int prevHookY = 50;

// Weather system for enhanced gameplay
enum Weather {
  CLEAR,
  RAINY,
  STORMY
};
Weather currentWeather = CLEAR;
unsigned long weatherTimer = 0;
int rainDrops[10][3]; // x, y, speed

// Particle system for visual effects
struct Particle {
  float x, y;
  float vx, vy;
  int color;
  int life;
  int maxLife;
  bool active;
};

Particle particles[20];
int particleCount = 0;

// Enhanced fish structure
struct Fish {
  float x, y;
  float vx, vy;
  float targetX, targetY;
  int color;
  int size;
  int tapsNeeded;
  bool active;
  FishType type;
  int points;
  unsigned long lastMove;
  float phase; // For sine wave movement
  bool scared; // Fish behavior
};

// Increased fish count and variety
Fish fish[12] = {
  {25, 80, 0.5, 0.3, 50, 100, ORANGE, 6, 1, true, NORMAL, 10, 0, 0, false},
  {70, 100, -0.7, 0.5, 40, 120, YELLOW, 8, 2, true, NORMAL, 15, 0, 0, false},
  {110, 120, 0.8, -0.4, 90, 110, RED, 10, 3, true, NORMAL, 20, 0, 0, false},
  {40, 140, -0.6, -0.6, 60, 130, GREEN, 7, 2, true, NORMAL, 15, 0, 0, false},
  {85, 160, 0.9, 0.7, 80, 150, BLUE, 9, 3, true, NORMAL, 20, 0, 0, false},
  {20, 180, -0.5, 0.8, 30, 170, PURPLE, 11, 4, true, RARE, 50, 0, 0, false},
  {100, 200, 0.7, -0.5, 95, 190, PINK, 8, 2, true, NORMAL, 15, 0, 0, false},
  {60, 170, -0.8, -0.7, 65, 160, CYAN, 12, 4, true, RARE, 50, 0, 0, false},
  {30, 90, 1.2, 0.9, 35, 95, WHITE, 5, 1, true, FAST, 30, 0, 0, false},
  {90, 130, -1.1, 0.6, 85, 125, MAGENTA, 6, 2, true, FAST, 25, 0, 0, false},
  {50, 200, 0.3, -0.2, 55, 195, GOLD, 15, 5, true, GOLDEN, 100, 0, 0, false},
  {75, 110, -0.4, 0.7, 70, 115, GOLD, 14, 5, true, GOLDEN, 100, 0, 0, false}
};

Fish* targetFish = nullptr;

// Sound and vibration settings
bool soundEnabled = true;
bool vibrationEnabled = true;

void setup() {
  M5.begin();
  M5.Lcd.setRotation(0);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE);
  
  // Initialize IMU
  M5.Imu.begin();
  
  // Initialize particles
  for (int i = 0; i < 20; i++) {
    particles[i].active = false;
  }
  
  // Initialize rain drops
  for (int i = 0; i < 10; i++) {
    rainDrops[i][0] = random(0, 135);
    rainDrops[i][1] = random(0, 240);
    rainDrops[i][2] = random(2, 5);
  }
  
  gameStartTime = millis();
  lastUpdate = millis();
  weatherTimer = millis();
  
  // Load high score (simulated - in real implementation, load from EEPROM)
  highScore = 500;
  
  currentState = MENU;
  needsFullRedraw = true;
}

void loop() {
  M5.update();
  
  unsigned long currentTime = millis();
  
  // Reduced update frequency to minimize screen flashing
  // Full update every 50ms, partial updates every 20ms
  bool doFullUpdate = (currentTime - lastUpdate >= 50);
  bool doPartialUpdate = (currentTime - lastPartialUpdate >= 20);
  
  if (!doFullUpdate && !doPartialUpdate) {
    delay(5);
    return;
  }
  
  if (doFullUpdate) {
    lastUpdate = currentTime;
  }
  if (doPartialUpdate) {
    lastPartialUpdate = currentTime;
  }
  
  // Read IMU data
  M5.Imu.getAccel(&accX, &accY, &accZ);
  
  // Update weather system
  updateWeather();
  
  // Update particles
  updateParticles();
  
  switch (currentState) {
    case MENU:
      handleMenu();
      break;
      
    case WAITING_TO_CAST:
      handleWaitingToCast();
      break;
      
    case FISHING:
      handleFishing();
      break;
      
    case FISH_HOOKED:
      handleFishHooked();
      break;
      
    case CAUGHT:
      handleCaught();
      break;
      
    case MISSED_TURN:
      handleMissedTurn();
      break;
      
    case GAME_OVER:
      handleGameOver();
      break;
  }
  
  // Only update display if something changed or full update is needed
  if (doFullUpdate || needsFullRedraw || hookMoved || fishMoved || statusChanged) {
    updateDisplay(doFullUpdate);
    hookMoved = false;
    fishMoved = false;
    statusChanged = false;
    needsFullRedraw = false;
  }
}

void handleMenu() {
  if (M5.BtnA.wasPressed()) {
    currentState = WAITING_TO_CAST;
    gameStartTime = millis();
    score = 0;
    combo = 0;
    difficulty = 1;
    activePowerUp = NONE;
    needsFullRedraw = true;
    statusChanged = true;
    
    // Reset fish positions
    for (int i = 0; i < 12; i++) {
      fish[i].x = random(20, 115);
      fish[i].y = random(70, 200);
      fish[i].active = true;
      fish[i].scared = false;
    }
    
    playSound(1000, 100); // Start game sound
  }
  
  if (M5.BtnB.wasPressed()) {
    // Toggle sound/vibration settings
    soundEnabled = !soundEnabled;
    statusChanged = true;
    playSound(500, 50);
  }
}

void handleWaitingToCast() {
  // Calculate rod angle with smoothing
  int newRodAngle = constrain(-accX * 30, -30, 30);
  if (abs(newRodAngle - rodAngle) > 1) {
    rodAngle = newRodAngle;
    hookMoved = true;
  }
  
  // Enhanced swing detection with gyroscope
  float totalAccel = sqrt(accX*accX + accY*accY + accZ*accZ);
  float accelChange = abs(accX - prevAccX) + abs(accY - prevAccY);
  
  if (totalAccel > 2.0 || accelChange > 0.8) {
    currentState = FISHING;
    fishingStartTime = millis();
    stringLength = 0;
    stringExtending = false;
    stringRetracting = false;
    hookX = 67 + rodAngle;
    hookY = 50;
    
    // Create splash effect
    createSplashEffect(hookX, hookY);
    playSound(800, 150);
    vibrate(50);
    statusChanged = true;
  }
  
  prevAccX = accX;
  prevAccY = accY;
}

void handleFishing() {
  // Update power-up timer
  if (activePowerUp != NONE && millis() - powerUpTimer > powerUpDuration) {
    activePowerUp = NONE;
    statusChanged = true;
  }
  
  // Handle string control with Button B
  if (M5.BtnB.isPressed() && !stringRetracting) {
    if (!stringExtending) {
      stringExtending = true;
      statusChanged = true;
    }
    
    int currentStringSpeed = stringSpeed;
    if (activePowerUp == TIME_SLOW) {
      currentStringSpeed = stringSpeed / 2;
    }
    
    if (stringLength < maxStringLength) {
      stringLength += currentStringSpeed;
      hookY = 50 + stringLength;
      hookMoved = true;
      
      // Create bubbles while descending
      if (random(0, 100) < 20) {
        createBubble(hookX + random(-5, 5), hookY + random(-5, 5));
      }
    } else {
      // Reached bottom - missed turn!
      stringExtending = false;
      stringRetracting = true;
      currentState = MISSED_TURN;
      missedTurnStartTime = millis();
      
      // Reset combo
      combo = 0;
      
      playSound(200, 300);
      vibrate(200);
      statusChanged = true;
      return;
    }
  }
  
  if (M5.BtnB.wasReleased() && stringExtending) {
    stringExtending = false;
    stringRetracting = true;
    statusChanged = true;
    
    // Check if hook hits any fish
    checkFishCollision();
  }
  
  // Handle string retraction
  if (stringRetracting && targetFish == nullptr) {
    if (stringLength > 0) {
      stringLength -= stringSpeed * 2;
      hookY = 50 + stringLength;
      hookMoved = true;
      
      if (stringLength <= 0) {
        stringLength = 0;
        hookY = 50;
        stringRetracting = false;
        currentState = WAITING_TO_CAST; // Return to swing scene
        statusChanged = true;
        needsFullRedraw = true;
      }
    }
  }
  
  // Animate fish movement with enhanced AI
  animateFishEnhanced();
  
  // Random power-up spawn
  if (random(0, 1000) < 2 && activePowerUp == NONE) {
    spawnRandomPowerUp();
  }
  
  // Increase difficulty over time
  unsigned long gameTime = (millis() - gameStartTime) / 1000;
  int newDifficulty = 1 + gameTime / 30;
  if (newDifficulty != difficulty) {
    difficulty = newDifficulty;
    statusChanged = true;
  }
}

void handleFishHooked() {
  // Enhanced reeling mechanics
  if (M5.BtnB.wasPressed()) {
    tapCount++;
    lastTapTime = millis();
    
    // Visual feedback
    createTapEffect(hookX, hookY);
    playSound(1200 + tapCount * 100, 50);
    vibrate(30);
    
    if (tapCount >= requiredTaps) {
      // Fish caught successfully!
      int fishPoints = targetFish->points;
      
      // Apply power-up bonus
      if (activePowerUp == DOUBLE_POINTS) {
        fishPoints *= 2;
      }
      
      // Apply combo bonus
      combo++;
      if (combo > 1) {
        fishPoints += combo * 5;
      }
      
      score += fishPoints;
      comboTimer = millis();
      
      if (score > highScore) {
        highScore = score;
      }
      
      // Special effects for different fish types
      if (targetFish->type == GOLDEN) {
        createGoldenEffect(targetFish->x, targetFish->y);
        playSound(1500, 200);
      } else if (targetFish->type == RARE) {
        createRareEffect(targetFish->x, targetFish->y);
        playSound(1300, 150);
      }
      
      currentState = CAUGHT;
      vibrate(100);
      statusChanged = true;
      return;
    }
  }
  
  // Combo timer - lose combo if too slow
  if (millis() - comboTimer > 5000 && combo > 0) {
    combo = 0;
    statusChanged = true;
  }
  
  // Enhanced escape mechanism
  unsigned long escapeTime = 3000;
  if (targetFish->type == FAST) {
    escapeTime = 2000;
  } else if (targetFish->type == RARE || targetFish->type == GOLDEN) {
    escapeTime = 4000;
  }
  
  if (millis() - lastTapTime > escapeTime) {
    // Fish escapes
    if (targetFish != nullptr) {
      targetFish->active = true;
      targetFish->scared = true;
    }
    targetFish = nullptr;
    stringRetracting = true;
    currentState = FISHING;
    
    // Reset combo
    combo = 0;
    
    playSound(300, 200);
    statusChanged = true;
  }
}

void handleCaught() {
  static unsigned long caughtTime = 0;
  if (caughtTime == 0) {
    caughtTime = millis();
  }
  
  if (millis() - caughtTime > 2000) {
    currentState = WAITING_TO_CAST; // Return to swing scene after catching fish
    
    // Respawn the caught fish
    if (targetFish != nullptr) {
      respawnFish(targetFish);
    }
    
    targetFish = nullptr;
    stringLength = 0;
    hookY = 50;
    stringExtending = false;
    stringRetracting = false;
    caughtTime = 0;
    statusChanged = true;
    needsFullRedraw = true;
  }
}

void handleMissedTurn() {
  if (stringLength > 0) {
    stringLength -= stringSpeed * 2;
    hookY = 50 + stringLength;
    hookMoved = true;
  } else {
    if (millis() - missedTurnStartTime > 1000) {
      currentState = WAITING_TO_CAST; // Return to swing scene after missed turn
      stringLength = 0;
      hookY = 50;
      stringExtending = false;
      stringRetracting = false;
      statusChanged = true;
      needsFullRedraw = true;
    }
  }
}

void handleGameOver() {
  if (M5.BtnA.wasPressed()) {
    // Reset game
    currentState = MENU;
    needsFullRedraw = true;
    statusChanged = true;
    playSound(600, 100);
  }
}

void checkFishCollision() {
  for (int i = 0; i < 12; i++) {
    if (fish[i].active && !fish[i].scared) {
      float distance = sqrt(pow(hookX - fish[i].x, 2) + pow(hookY - fish[i].y, 2));
      
      int hitRadius = fish[i].size + 5;
      if (activePowerUp == BIG_HOOK) {
        hitRadius += 3;
      }
      
      if (distance <= hitRadius) {
        targetFish = &fish[i];
        requiredTaps = fish[i].tapsNeeded;
        
        // Adjust difficulty
        if (difficulty > 1) {
          requiredTaps += (difficulty - 1);
        }
        
        tapCount = 0;
        lastTapTime = millis();
        currentState = FISH_HOOKED;
        stringRetracting = false;
        
        fish[i].active = false;
        
        // Different feedback for different fish types
        if (fish[i].type == GOLDEN) {
          playSound(1800, 100);
          vibrate(150);
        } else {
          playSound(1000, 100);
          vibrate(75);
        }
        
        statusChanged = true;
        return;
      }
    }
  }
}

void animateFishEnhanced() {
  bool anyFishMoved = false;
  
  for (int i = 0; i < 12; i++) {
    if (fish[i].active) {
      float prevX = fish[i].x;
      float prevY = fish[i].y;
      
      // Enhanced AI behavior
      if (fish[i].scared) {
        // Scared fish move away from hook
        float dx = fish[i].x - hookX;
        float dy = fish[i].y - hookY;
        float dist = sqrt(dx*dx + dy*dy);
        
        if (dist > 0) {
          fish[i].vx = (dx / dist) * 2;
          fish[i].vy = (dy / dist) * 1;
        }
        
        // Gradually calm down
        if (millis() - fish[i].lastMove > 3000) {
          fish[i].scared = false;
        }
      } else {
        // Normal behavior based on fish type
        switch (fish[i].type) {
          case FAST:
            // Fast fish move quickly in straight lines
            fish[i].x += fish[i].vx * 1.5;
            fish[i].y += fish[i].vy * 1.5;
            break;
            
          case GOLDEN:
          case RARE:
            // Rare fish move in sine patterns
            fish[i].phase += 0.1;
            fish[i].x += sin(fish[i].phase) * 0.5;
            fish[i].y += cos(fish[i].phase * 0.7) * 0.3;
            break;
            
          default:
            // Normal fish movement with some randomness
            fish[i].x += fish[i].vx + (random(-10, 10) / 100.0);
            fish[i].y += fish[i].vy + (random(-10, 10) / 100.0);
            break;
        }
      }
      
      // Weather effects
      if (currentWeather == STORMY) {
        fish[i].vx += random(-20, 20) / 100.0;
        fish[i].vy += random(-20, 20) / 100.0;
      }
      
      // Boundary checking with wrapping
      if (fish[i].x < 10) {
        fish[i].x = 125;
        fish[i].vx = abs(fish[i].vx);
      } else if (fish[i].x > 125) {
        fish[i].x = 10;
        fish[i].vx = -abs(fish[i].vx);
      }
      
      if (fish[i].y < 60) {
        fish[i].y = 210;
        fish[i].vy = abs(fish[i].vy);
      } else if (fish[i].y > 210) {
        fish[i].y = 60;
        fish[i].vy = -abs(fish[i].vy);
      }
      
      // Check if fish moved significantly
      if (abs(fish[i].x - prevX) > 0.5 || abs(fish[i].y - prevY) > 0.5) {
        anyFishMoved = true;
      }
    }
  }
  
  fishMoved = anyFishMoved;
}

void updateWeather() {
  // Change weather every 30 seconds
  if (millis() - weatherTimer > 30000) {
    currentWeather = (Weather)random(0, 3);
    weatherTimer = millis();
    statusChanged = true;
  }
  
  // Update rain animation
  if (currentWeather == RAINY || currentWeather == STORMY) {
    for (int i = 0; i < 10; i++) {
      rainDrops[i][1] += rainDrops[i][2];
      if (rainDrops[i][1] > 240) {
        rainDrops[i][0] = random(0, 135);
        rainDrops[i][1] = 0;
      }
    }
  }
}

void updateParticles() {
  bool anyActive = false;
  for (int i = 0; i < 20; i++) {
    if (particles[i].active) {
      particles[i].x += particles[i].vx;
      particles[i].y += particles[i].vy;
      particles[i].vy += 0.1; // gravity
      particles[i].life--;
      
      if (particles[i].life <= 0) {
        particles[i].active = false;
      } else {
        anyActive = true;
      }
    }
  }
  
  if (anyActive) {
    statusChanged = true;
  }
}

void updateDisplay(bool fullUpdate) {
  if (fullUpdate || needsFullRedraw) {
    M5.Lcd.fillScreen(BLACK);
  }
  
  if (currentState == MENU) {
    drawMenu();
  } else if (currentState == WAITING_TO_CAST) {
    drawFirstScreen();
  } else {
    drawFishingScreen(fullUpdate);
  }
  
  if (currentState != MENU) {
    drawGameStatus();
  }
}

void drawMenu() {
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(20, 30);
  M5.Lcd.println("Enhanced Fishing");
  M5.Lcd.setCursor(30, 50);
  M5.Lcd.println("M5StickC Plus 2");
  
  M5.Lcd.setCursor(10, 80);
  M5.Lcd.printf("High Score: %d", highScore);
  
  M5.Lcd.setCursor(10, 110);
  M5.Lcd.println("Features:");
  M5.Lcd.setCursor(10, 125);
  M5.Lcd.println("- 12 different fish");
  M5.Lcd.setCursor(10, 140);
  M5.Lcd.println("- Weather effects");
  M5.Lcd.setCursor(10, 155);
  M5.Lcd.println("- Power-ups");
  M5.Lcd.setCursor(10, 170);
  M5.Lcd.println("- Combo system");
  
  M5.Lcd.setCursor(10, 200);
  M5.Lcd.println("A: Start  B: Settings");
  
  M5.Lcd.setCursor(10, 220);
  M5.Lcd.printf("Sound: %s", soundEnabled ? "ON" : "OFF");
}

void drawFirstScreen() {
  // Draw water line
  M5.Lcd.drawLine(0, 50, 135, 50, CYAN);
  
  // Draw boat deck
  M5.Lcd.fillRect(0, 200, 135, 40, WHITE);
  
  // Draw tilting fishing rod
  int rodBaseX = 67;
  int rodBaseY = 230;
  int rodTipX = rodBaseX + sin(rodAngle * PI / 180) * 80;
  int rodTipY = rodBaseY - cos(rodAngle * PI / 180) * 80;
  
  M5.Lcd.drawLine(rodBaseX, rodBaseY, rodTipX, rodTipY, WHITE);
  
  // Draw reel
  M5.Lcd.fillCircle(rodBaseX - 5, rodBaseY - 10, 4, WHITE);
  M5.Lcd.fillCircle(rodBaseX - 5, rodBaseY - 10, 2, BLACK);
  
  // Instructions
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(10, 60);
  M5.Lcd.println("Tilt to aim rod");
  M5.Lcd.setCursor(10, 75);
  M5.Lcd.println("Swing to start");
  
  // Show weather
  drawWeatherIcon(100, 60);
}

void drawFishingScreen(bool fullUpdate) {
  if (fullUpdate) {
    // Draw water surface
    M5.Lcd.drawLine(0, 50, 135, 50, CYAN);
    
    // Draw weather effects
    drawWeatherEffects();
  }
  
  // Draw fishing line/string from rod tip to hook
  if (hookMoved || fullUpdate) {
    int lineStartX = 67 + sin(rodAngle * PI / 180) * 30;
    int lineStartY = 50 - cos(rodAngle * PI / 180) * 30;
    
    // Clear previous line (simple approach)
    if (!fullUpdate) {
      M5.Lcd.drawLine(67, 20, prevHookX, prevHookY, BLACK);
    }
    
    M5.Lcd.drawLine(lineStartX, lineStartY, hookX, hookY, WHITE);
    
    prevHookX = hookX;
    prevHookY = hookY;
  }
  
  // Draw hook
  M5.Lcd.fillCircle(hookX, hookY, 2, YELLOW);
  
  // Draw fish (only if they moved or full update)
  if (fishMoved || fullUpdate) {
    for (int i = 0; i < 12; i++) {
      if (fish[i].active) {
        drawEnhancedFish(fish[i].x, fish[i].y, fish[i].color, fish[i].size, fish[i].type);
      }
    }
  }
  
  // Highlight target fish if hooked
  if (targetFish != nullptr) {
    M5.Lcd.drawCircle(targetFish->x, targetFish->y, targetFish->size + 3, WHITE);
  }
  
  // Draw particles
  drawParticles();
  
  // Draw power-up indicator
  if (activePowerUp != NONE) {
    drawPowerUpIndicator();
  }
  
  // Draw depth indicator
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(100, 55);
  M5.Lcd.printf("D:%d", stringLength);
}

void drawGameStatus() {
  // Clear status area with minimal flashing
  if (statusChanged || needsFullRedraw) {
    M5.Lcd.fillRect(0, 0, 135, 40, BLACK);
  }
  
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(5, 5);
  
  switch (currentState) {
    case WAITING_TO_CAST:
      M5.Lcd.setTextColor(WHITE);
      M5.Lcd.println("Ready to fish!");
      break;
      
    case FISHING:
      M5.Lcd.setTextColor(WHITE);
      if (stringExtending) {
        M5.Lcd.println("Hold B - Extending");
      } else if (stringRetracting) {
        M5.Lcd.println("Retracting...");
      } else {
        M5.Lcd.println("Hold B to cast");
      }
      break;
      
    case FISH_HOOKED:
      M5.Lcd.setTextColor(RED);
      M5.Lcd.println("FISH ON!");
      M5.Lcd.setTextColor(WHITE);
      M5.Lcd.setCursor(5, 15);
      M5.Lcd.printf("Tap B: %d/%d", tapCount, requiredTaps);
      break;
      
    case CAUGHT:
      M5.Lcd.setTextColor(GREEN);
      M5.Lcd.println("CAUGHT!");
      break;
      
    case MISSED_TURN:
      M5.Lcd.setTextColor(RED);
      M5.Lcd.println("MISSED! Too deep");
      break;
      
    case GAME_OVER:
      M5.Lcd.setTextColor(WHITE);
      M5.Lcd.println("Game Over!");
      break;
  }
  
  // Score and combo
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(5, 25);
  M5.Lcd.printf("Score: %d", score);
  
  if (combo > 1) {
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setCursor(70, 15);
    M5.Lcd.printf("x%d", combo);
  }
  
  // Time
  unsigned long gameTime = (millis() - gameStartTime) / 1000;
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(70, 25);
  M5.Lcd.printf("%dm%ds", gameTime/60, gameTime%60);
}

void drawEnhancedFish(int x, int y, int color, int size, FishType type) {
  // Fish body with type-specific effects
  if (type == GOLDEN) {
    // Golden fish with shimmer effect
    int shimmerColor = (millis() % 500 < 250) ? GOLD : YELLOW;
    M5.Lcd.fillCircle(x, y, size, shimmerColor);
  } else if (type == FAST) {
    // Fast fish with motion blur effect
    M5.Lcd.fillCircle(x-1, y, size-1, color);
    M5.Lcd.fillCircle(x, y, size, color);
  } else {
    M5.Lcd.fillCircle(x, y, size, color);
  }
  
  // Fish tail
  int tailSize = size / 2;
  M5.Lcd.fillTriangle(x - size - tailSize, y, 
                      x - size, y - tailSize, 
                      x - size, y + tailSize, color);
  
  // Fish eye
  M5.Lcd.fillCircle(x + size/3, y - size/3, size/4, BLACK);
  M5.Lcd.fillCircle(x + size/3 + 1, y - size/3 - 1, size/6, WHITE);
  
  // Type indicator
  if (type == RARE) {
    M5.Lcd.drawCircle(x, y, size + 2, PURPLE);
  }
}

void drawWeatherIcon(int x, int y) {
  switch (currentWeather) {
    case CLEAR:
      M5.Lcd.fillCircle(x, y, 8, YELLOW);
      break;
    case RAINY:
      M5.Lcd.fillCircle(x, y, 6, WHITE);
      M5.Lcd.drawLine(x-3, y+8, x-5, y+12, CYAN);
      M5.Lcd.drawLine(x, y+8, x-2, y+12, CYAN);
      M5.Lcd.drawLine(x+3, y+8, x+1, y+12, CYAN);
      break;
    case STORMY:
      M5.Lcd.fillCircle(x, y, 6, GRAY);
      M5.Lcd.drawLine(x-2, y+8, x+2, y+14, YELLOW);
      break;
  }
}

void drawWeatherEffects() {
  if (currentWeather == RAINY || currentWeather == STORMY) {
    for (int i = 0; i < 10; i++) {
      M5.Lcd.drawPixel(rainDrops[i][0], rainDrops[i][1], CYAN);
    }
  }
}

void drawParticles() {
  for (int i = 0; i < 20; i++) {
    if (particles[i].active) {
      int alpha = map(particles[i].life, 0, particles[i].maxLife, 0, 255);
      M5.Lcd.drawPixel(particles[i].x, particles[i].y, particles[i].color);
    }
  }
}

void drawPowerUpIndicator() {
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(MAGENTA);
  M5.Lcd.setCursor(5, 35);
  
  switch (activePowerUp) {
    case BIG_HOOK:
      M5.Lcd.println("BIG HOOK");
      break;
    case TIME_SLOW:
      M5.Lcd.println("TIME SLOW");
      break;
    case DOUBLE_POINTS:
      M5.Lcd.println("2X POINTS");
      break;
  }
}

// Effect creation functions
void createSplashEffect(int x, int y) {
  for (int i = 0; i < 5; i++) {
    if (!particles[i].active) {
      particles[i].x = x + random(-5, 5);
      particles[i].y = y;
      particles[i].vx = random(-20, 20) / 10.0;
      particles[i].vy = random(-30, -10) / 10.0;
      particles[i].color = CYAN;
      particles[i].life = 30;
      particles[i].maxLife = 30;
      particles[i].active = true;
      break;
    }
  }
}

void createBubble(int x, int y) {
  for (int i = 0; i < 20; i++) {
    if (!particles[i].active) {
      particles[i].x = x;
      particles[i].y = y;
      particles[i].vx = random(-5, 5) / 10.0;
      particles[i].vy = random(-20, -5) / 10.0;
      particles[i].color = WHITE;
      particles[i].life = 40;
      particles[i].maxLife = 40;
      particles[i].active = true;
      break;
    }
  }
}

void createTapEffect(int x, int y) {
  for (int i = 0; i < 3; i++) {
    if (!particles[i].active) {
      particles[i].x = x + random(-8, 8);
      particles[i].y = y + random(-8, 8);
      particles[i].vx = 0;
      particles[i].vy = 0;
      particles[i].color = YELLOW;
      particles[i].life = 15;
      particles[i].maxLife = 15;
      particles[i].active = true;
    }
  }
}

void createGoldenEffect(int x, int y) {
  for (int i = 0; i < 8; i++) {
    if (!particles[i].active) {
      particles[i].x = x;
      particles[i].y = y;
      particles[i].vx = cos(i * PI / 4) * 2;
      particles[i].vy = sin(i * PI / 4) * 2;
      particles[i].color = GOLD;
      particles[i].life = 50;
      particles[i].maxLife = 50;
      particles[i].active = true;
    }
  }
}

void createRareEffect(int x, int y) {
  for (int i = 0; i < 6; i++) {
    if (!particles[i].active) {
      particles[i].x = x + random(-10, 10);
      particles[i].y = y + random(-10, 10);
      particles[i].vx = random(-10, 10) / 10.0;
      particles[i].vy = random(-10, 10) / 10.0;
      particles[i].color = PURPLE;
      particles[i].life = 35;
      particles[i].maxLife = 35;
      particles[i].active = true;
    }
  }
}

void spawnRandomPowerUp() {
  activePowerUp = (PowerUpType)random(1, 4);
  powerUpTimer = millis();
  powerUpDuration = 10000; // 10 seconds
  statusChanged = true;
  playSound(1500, 100);
}

void respawnFish(Fish* fish) {
  fish->x = random(20, 115);
  fish->y = random(70, 200);
  fish->active = true;
  fish->scared = false;
  fish->phase = 0;
}

// Sound and vibration functions
void playSound(int frequency, int duration) {
  if (soundEnabled) {
    // Simplified sound - in real implementation use M5.Beep
    // M5.Beep.tone(frequency, duration);
  }
}

void vibrate(int duration) {
  if (vibrationEnabled) {
    // Simplified vibration - in real implementation use M5.Power
    // M5.Power.lightSleep(duration);
  }
}