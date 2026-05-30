#ifndef SPECTER_FACE_H
#define SPECTER_FACE_H

#include <TFT_eSPI.h>

// Harvey's 30 Distinct Moods/Vibes
enum HarveyVibe {
  VIBE_IDLE, VIBE_BORED, VIBE_SUSPICIOUS, VIBE_AMUSED, VIBE_ANNOYED, 
  VIBE_ANGRY, VIBE_FOCUSED, VIBE_CONFUSED, VIBE_SARCASTIC, VIBE_SMUG, 
  VIBE_DISAPPOINTED, VIBE_INTIMIDATING, VIBE_SCANNING, VIBE_GLITCHING, VIBE_SURPRISED, 
  VIBE_SLEEPY, VIBE_JUDGING, VIBE_IMPATIENT, VIBE_CALCULATING, VIBE_MISCHIEVOUS, 
  VIBE_OVERHEATED, VIBE_LISTENING, VIBE_DISMISSIVE, VIBE_PLOTTING, VIBE_COCKY, 
  VIBE_SHOCKED, VIBE_LOADING, VIBE_VICTORIOUS, VIBE_DEFENSIVE, VIBE_MENACING
};

// Movement & Action Commands (from your screenshots)
enum HarveyAction {
  ACT_MOVE_FORWARD, ACT_MOVE_BACKWARD, ACT_TURN_LEFT, ACT_TURN_RIGHT,
  ACT_STOP, ACT_HOW_ARE_YOU, ACT_DANCE, ACT_TEMPERATURE, ACT_DISTANCE,
  ACT_FOLLOW_ME, ACT_SCARE
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
    
    // Core Triggers
    void setVibe(HarveyVibe newVibe);
    void setAction(HarveyAction newAction);
    
    // Multi-Step Cinematic Sequences
    void playBootSequence();
    void playShutdownSequence();

    // Text Label Overlay
    void setLabel(String text);

  private:
    TFT_eSPI* tft;
    TFT_eSprite* spr; 
    
    const int canvasW = 240;
    const int canvasH = 320;
    
    EyeState currentLeft;
    EyeState currentRight;
    EyeState targetLeft;
    EyeState targetRight;
    EyeState idleBaseLeft;
    EyeState idleBaseRight;
    
    float animationSpeed = 0.22; 
    String debugLabel = "";
    
    // Blinking Engine
    unsigned long lastBlinkTime = 0;
    unsigned long blinkInterval = 3000;
    bool isBlinking = false;
    unsigned long blinkStartTime = 0;
    const int BLINK_DURATION = 130; 

    void drawEye(EyeState state, uint32_t color);
    void renderFrame();
};

#endif