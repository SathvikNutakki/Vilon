#ifndef SPECTER_FACE_H
#define SPECTER_FACE_H

#include <TFT_eSPI.h>

enum Expression {
  EXPR_IDLE,
  EXPR_MOVE_FORWARD,
  EXPR_MOVE_BACKWARD,
  EXPR_TURN_LEFT,
  EXPR_TURN_RIGHT,
  EXPR_STOP,
  EXPR_HOW_ARE_YOU,
  EXPR_DANCE,
  EXPR_TEMPERATURE,
  EXPR_DISTANCE,
  EXPR_SLEEP,
  EXPR_FOLLOW,
  EXPR_SCARE
};

struct EyeState {
  float x, y, w, h, r;
};

class SpecterFace {
  public:
    SpecterFace(TFT_eSPI* tft_ptr);
    ~SpecterFace();
    
    void begin();
    void update();
    void setExpression(Expression newExpr);
    void blink();

  private:
    TFT_eSPI* tft;
    TFT_eSprite* spr; 
    
    // Fixed native portrait canvas dimensions
    const int canvasW = 240;
    const int canvasH = 320;
    
    EyeState currentLeft;
    EyeState currentRight;
    EyeState targetLeft;
    EyeState targetRight;

    Expression currentExpression;
    
    float animationSpeed = 0.20; 
    
    unsigned long lastBlinkTime = 0;
    unsigned long blinkInterval = 3000;
    bool isBlinking = false;
    unsigned long blinkStartTime = 0;
    const int BLINK_DURATION = 140; 

    void loadTargetPreset(Expression expr);
    void drawEye(EyeState state, uint32_t color);
};

#endif