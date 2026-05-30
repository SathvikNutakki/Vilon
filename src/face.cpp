#include "SpecterFace.h"

SpecterFace::SpecterFace(TFT_eSPI* tft_ptr) {
  tft = tft_ptr;
  spr = new TFT_eSprite(tft);
}

SpecterFace::~SpecterFace() {
  spr->deleteSprite();
  delete spr;
}

void SpecterFace::begin() {
  spr->setColorDepth(8); 
  spr->createSprite(canvasW, canvasH);
  
  // Harvey's arrogant, slightly narrowed baseline
  idleBaseLeft =  {20, 135, 80, 45, 12};
  idleBaseRight = {140, 135, 80, 45, 12};
  
  currentLeft = idleBaseLeft;
  currentRight = idleBaseRight;
  targetLeft = idleBaseLeft;
  targetRight = idleBaseRight;
}

void SpecterFace::setLabel(String text) {
  debugLabel = text;
}

// ==========================================
// CINEMATIC SEQUENCES (Boot & Quit)
// ==========================================

void SpecterFace::playBootSequence() {
  currentLeft = {120, 160, 0, 0, 0};
  currentRight = {120, 160, 0, 0, 0};
  renderFrame();
  delay(1000); 

  currentLeft = {110, 158, 8, 4, 2};
  currentRight = {122, 158, 8, 4, 2};
  renderFrame();
  delay(800);

  currentLeft = {10, 110, 90, 80, 16};
  currentRight = {140, 110, 90, 80, 16};
  renderFrame();
  delay(600);

  setVibe(VIBE_IDLE);
}

void SpecterFace::playShutdownSequence() {
  targetLeft = {30, 200, 60, 6, 4};
  targetRight = {150, 200, 60, 6, 4};
  
  for(int i = 0; i < 30; i++) {
    update();
    delay(20);
  }

  currentLeft = {120, 200, 0, 0, 0};
  currentRight = {120, 200, 0, 0, 0};
  renderFrame();
}

// ==========================================
// ACTION ENGINE (Movement Responses)
// ==========================================

void SpecterFace::setAction(HarveyAction newAction) {
  switch (newAction) {
    case ACT_MOVE_FORWARD:
      targetLeft = {25, 110, 70, 40, 10}; targetRight = {145, 110, 70, 40, 10};
      break;
    case ACT_MOVE_BACKWARD:
      targetLeft = {30, 160, 60, 30, 8}; targetRight = {150, 160, 60, 30, 8};
      break;
    case ACT_TURN_LEFT:
      targetLeft = {10, 135, 75, 55, 14}; targetRight = {95, 135, 70, 45, 12};
      break;
    case ACT_TURN_RIGHT:
      targetLeft = {75, 135, 70, 45, 12}; targetRight = {155, 135, 75, 55, 14};
      break;
    case ACT_STOP:
      targetLeft = {25, 120, 70, 70, 15}; targetRight = {145, 120, 70, 70, 15};
      break;
    case ACT_HOW_ARE_YOU:
      targetLeft = {20, 110, 80, 70, 15}; targetRight = {140, 145, 80, 25, 10};
      break;
    case ACT_DANCE:
      targetLeft = {20, 115, 80, 60, 18}; targetRight = {140, 155, 80, 30, 10};
      break;
    case ACT_TEMPERATURE:
      targetLeft = {20, 145, 80, 20, 8}; targetRight = {140, 145, 80, 20, 8};
      break;
    case ACT_DISTANCE:
      targetLeft = {15, 120, 90, 70, 18}; targetRight = {135, 120, 90, 70, 18};
      break;
    case ACT_FOLLOW_ME:
      targetLeft = {30, 135, 60, 40, 12}; targetRight = {150, 135, 60, 40, 12};
      break;
    case ACT_SCARE:
      targetLeft = {10, 90, 95, 110, 8}; targetRight = {135, 90, 95, 110, 8};
      break;
  }
}

// ==========================================
// HARVEY's 30 VIBE MATRIX
// ==========================================

void SpecterFace::setVibe(HarveyVibe newVibe) {
  switch (newVibe) {
    case VIBE_IDLE: targetLeft = idleBaseLeft; targetRight = idleBaseRight; break;
    case VIBE_BORED: targetLeft = {20, 155, 80, 25, 10}; targetRight = {140, 155, 80, 25, 10}; break;
    case VIBE_SUSPICIOUS: targetLeft = {40, 140, 60, 20, 8}; targetRight = {120, 140, 60, 20, 8}; break;
    case VIBE_AMUSED: targetLeft = {20, 130, 80, 45, 20}; targetRight = {140, 130, 80, 45, 20}; break;
    case VIBE_ANNOYED: targetLeft = {25, 135, 75, 30, 5}; targetRight = {140, 135, 75, 30, 5}; break;
    case VIBE_ANGRY: targetLeft = {15, 120, 85, 60, 8}; targetRight = {140, 120, 85, 60, 8}; break;
    case VIBE_FOCUSED: targetLeft = {35, 140, 50, 40, 15}; targetRight = {155, 140, 50, 40, 15}; break;
    case VIBE_CONFUSED: targetLeft = {20, 120, 80, 70, 15}; targetRight = {140, 145, 60, 30, 10}; break;
    case VIBE_SARCASTIC: targetLeft = {10, 145, 70, 25, 8}; targetRight = {95, 145, 70, 25, 8}; break;
    case VIBE_SMUG: targetLeft = {20, 125, 80, 35, 15}; targetRight = {140, 125, 80, 35, 15}; break;
    case VIBE_DISAPPOINTED: targetLeft = {25, 160, 70, 20, 8}; targetRight = {145, 160, 70, 20, 8}; break;
    case VIBE_INTIMIDATING: targetLeft = {10, 100, 100, 90, 10}; targetRight = {130, 100, 100, 90, 10}; break;
    case VIBE_SCANNING: targetLeft = {10, 140, 90, 15, 6}; targetRight = {140, 140, 90, 15, 6}; break;
    case VIBE_GLITCHING: targetLeft = {20, 135, 20, 80, 5}; targetRight = {140, 135, 80, 20, 5}; break;
    case VIBE_SURPRISED: targetLeft = {25, 90, 70, 90, 25}; targetRight = {145, 90, 70, 90, 25}; break;
    case VIBE_SLEEPY: targetLeft = {20, 175, 80, 8, 4}; targetRight = {140, 175, 80, 8, 4}; break;
    case VIBE_JUDGING: targetLeft = {20, 145, 80, 15, 6}; targetRight = {140, 125, 80, 55, 14}; break;
    case VIBE_IMPATIENT: targetLeft = {20, 135, 80, 45, 14}; targetRight = {140, 120, 80, 45, 14}; break;
    case VIBE_CALCULATING: targetLeft = {30, 135, 40, 45, 10}; targetRight = {160, 135, 40, 45, 10}; break;
    case VIBE_MISCHIEVOUS: targetLeft = {30, 115, 70, 40, 18}; targetRight = {140, 115, 70, 40, 18}; break;
    case VIBE_OVERHEATED: targetLeft = {20, 150, 80, 30, 10}; targetRight = {140, 165, 80, 15, 8}; break;
    case VIBE_LISTENING: targetLeft = {50, 110, 60, 60, 18}; targetRight = {170, 110, 60, 60, 18}; break;
    case VIBE_DISMISSIVE: targetLeft = {10, 160, 70, 20, 8}; targetRight = {90, 160, 70, 20, 8}; break;
    case VIBE_PLOTTING: targetLeft = {40, 145, 55, 25, 8}; targetRight = {145, 145, 55, 25, 8}; break;
    case VIBE_COCKY: targetLeft = {20, 115, 80, 65, 14}; targetRight = {140, 140, 80, 25, 10}; break;
    case VIBE_SHOCKED: targetLeft = {45, 145, 30, 30, 15}; targetRight = {165, 145, 30, 30, 15}; break;
    case VIBE_LOADING: targetLeft = {20, 145, 80, 10, 5}; targetRight = {140, 145, 80, 10, 5}; break;
    case VIBE_VICTORIOUS: targetLeft = {20, 110, 80, 60, 20}; targetRight = {140, 110, 80, 60, 20}; break;
    case VIBE_DEFENSIVE: targetLeft = {10, 140, 60, 40, 10}; targetRight = {170, 140, 60, 40, 10}; break;
    case VIBE_MENACING: targetLeft = {20, 110, 80, 50, 5}; targetRight = {140, 110, 80, 50, 5}; break;
  }
}

// ==========================================
// CORE RENDERING ENGINE
// ==========================================

void SpecterFace::update() {
  unsigned long now = millis();

  if (!isBlinking && (now - lastBlinkTime > blinkInterval)) {
    isBlinking = true;
    blinkStartTime = now;
    lastBlinkTime = now;
    blinkInterval = random(2000, 6000);
  }

  EyeState frameTargetLeft = targetLeft;
  EyeState frameTargetRight = targetRight;

  if (isBlinking) {
    if (now - blinkStartTime < BLINK_DURATION) {
      frameTargetLeft.h = 4; frameTargetLeft.y += (targetLeft.h / 2) - 2; 
      frameTargetRight.h = 4; frameTargetRight.y += (targetRight.h / 2) - 2;
    } else {
      isBlinking = false;
    }
  }

  currentLeft.x += (frameTargetLeft.x - currentLeft.x) * animationSpeed;
  currentLeft.y += (frameTargetLeft.y - currentLeft.y) * animationSpeed;
  currentLeft.w += (frameTargetLeft.w - currentLeft.w) * animationSpeed;
  currentLeft.h += (frameTargetLeft.h - currentLeft.h) * animationSpeed;
  currentLeft.r += (frameTargetLeft.r - currentLeft.r) * animationSpeed;

  currentRight.x += (frameTargetRight.x - currentRight.x) * animationSpeed;
  currentRight.y += (frameTargetRight.y - currentRight.y) * animationSpeed;
  currentRight.w += (frameTargetRight.w - currentRight.w) * animationSpeed;
  currentRight.h += (frameTargetRight.h - currentRight.h) * animationSpeed;
  currentRight.r += (frameTargetRight.r - currentRight.r) * animationSpeed;

  renderFrame();
}

void SpecterFace::renderFrame() {
  spr->fillSprite(TFT_BLACK);
  drawEye(currentLeft, TFT_RED);
  drawEye(currentRight, TFT_RED);
  
  if (debugLabel.length() > 0) {
    spr->setTextColor(TFT_WHITE);
    spr->setTextDatum(TC_DATUM); 
    spr->drawString(debugLabel, canvasW / 2, 10, 2); 
  }
  
  spr->pushSprite(0, 0); 
}

void SpecterFace::drawEye(EyeState state, uint32_t color) {
  if (state.w <= 0 || state.h <= 0) return;
  
  int drawX = constrain((int)state.x, 5, 235 - (int)state.w);
  int drawY = constrain((int)state.y, 60, 260 - (int)state.h);
  int drawR = constrain((int)state.r, 2, 25);

  spr->fillRoundRect(drawX, drawY, (int)state.w, (int)state.h, drawR, color);
}