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
  
  // Set default cinematic idle eyes
  currentLeft =  {20, 135, 80, 50, 12};
  currentRight = {140, 135, 80, 50, 12};
  
  setExpression(EXPR_IDLE);
}

void SpecterFace::setExpression(Expression newExpr) {
  currentExpression = newExpr;
  loadTargetPreset(newExpr);
}

void SpecterFace::blink() {
  isBlinking = true;
  blinkStartTime = millis();
}

void SpecterFace::update() {
  unsigned long now = millis();

  if (!isBlinking && (now - lastBlinkTime > blinkInterval)) {
    blink();
    lastBlinkTime = now;
    blinkInterval = random(3000, 7500);
  }

  EyeState frameTargetLeft = targetLeft;
  EyeState frameTargetRight = targetRight;

  if (isBlinking) {
    if (now - blinkStartTime < BLINK_DURATION) {
      frameTargetLeft.h = 4;
      frameTargetLeft.y += (targetLeft.h / 2) - 2; 
      
      frameTargetRight.h = 4;
      frameTargetRight.y += (targetRight.h / 2) - 2;
    } else {
      isBlinking = false;
    }
  }

  // Kinematic Math (Lerp Transitions)
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

  // Double Buffer Render
  spr->fillSprite(TFT_BLACK);
  
  drawEye(currentLeft, TFT_RED);
  drawEye(currentRight, TFT_RED);
  
  spr->pushSprite(0, 0); 
}

void SpecterFace::drawEye(EyeState state, uint32_t color) {
  int drawX = (int)state.x;
  int drawY = (int)state.y;
  int drawW = (int)state.w;
  int drawH = (int)state.h;
  int drawR = (int)state.r;

  // Hard constraints matching your bounds rules (x: 5–235, y: 60–260)
  drawX = constrain(drawX, 5, 235 - drawW);
  drawY = constrain(drawY, 60, 260 - drawH);
  drawR = constrain(drawR, 8, 20);

  spr->fillRoundRect(drawX, drawY, drawW, drawH, drawR, color);
}

void SpecterFace::loadTargetPreset(Expression expr) {
  switch (expr) {
    case EXPR_IDLE:
    case EXPR_STOP:
      targetLeft =  {20, 135, 80, 50, 14};
      targetRight = {140, 135, 80, 50, 14};
      break;
    case EXPR_MOVE_FORWARD:
      targetLeft =  {15, 130, 85, 60, 12};
      targetRight = {140, 130, 85, 60, 12};
      break;
    case EXPR_MOVE_BACKWARD:
      targetLeft =  {25, 150, 75, 25, 8};
      targetRight = {140, 150, 75, 25, 8};
      break;
    case EXPR_TURN_LEFT:
      targetLeft =  {10, 140, 60, 45, 10};
      targetRight = {85, 130, 90, 55, 16};
      break;
    case EXPR_TURN_RIGHT:
      targetLeft =  {65, 130, 90, 55, 16};
      targetRight = {170, 140, 60, 45, 10};
      break;
    case EXPR_HOW_ARE_YOU:
      targetLeft =  {20, 110, 80, 80, 20};
      targetRight = {140, 145, 80, 30, 10};
      break;
    case EXPR_DANCE:
      targetLeft =  {25, 120, 75, 65, 20};
      targetRight = {140, 140, 75, 65, 20};
      break;
    case EXPR_TEMPERATURE:
      targetLeft =  {30, 155, 70, 15, 8};
      targetRight = {140, 155, 70, 15, 8};
      break;
    case EXPR_DISTANCE:
      targetLeft =  {10, 145, 95, 25, 8};
      targetRight = {135, 145, 95, 25, 8};
      break;
    case EXPR_SLEEP:
      targetLeft =  {35, 180, 65, 8, 8};
      targetRight = {140, 180, 65, 8, 8};
      break;
    case EXPR_FOLLOW:
      targetLeft =  {30, 130, 70, 55, 14};
      targetRight = {140, 130, 70, 55, 14};
      break;
    case EXPR_SCARE:
      targetLeft =  {15, 90, 90, 110, 8};
      targetRight = {135, 90, 90, 110, 8};
      break;
  }
}