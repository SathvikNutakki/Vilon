#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

#ifndef TFT_DISPLAY_BGR565
#define TFT_DISPLAY_BGR565 1
#endif

enum class HarveyExpr : uint8_t {
  Idle,
  SmugForward,
  AnnoyedRetreat,
  GlanceLeft,
  GlanceRight,
  DoneStop,
  SmugHowAreYou,
  ReluctantDance,
  DeadpanTemp,
  ScanDistance,
  Sleep,
  SoftFollow,
  Scare,
  Count
};

HarveyExpr harveyExprFromCommand(const String& cmd);

class HarveyFace {
 public:
  explicit HarveyFace(TFT_eSPI& display);
  ~HarveyFace();

  void begin();
  void setExpression(HarveyExpr expr, bool immediate = false);
  void setExpressionFromCommand(const String& cmd);
  void update();

  HarveyExpr currentExpression() const { return targetExpr_; }

 private:
  enum Quirk : uint8_t {
    QuirkNone = 0,
    QuirkWinkHold,
    QuirkCrossStare,
    QuirkVertigo,
    QuirkLazyLag,
    QuirkPuffShrink,
    QuirkFollowTwitch,
    QuirkDoubleTake,
    QuirkCaffeine,
  };

  struct Pose {
    float hL = 1.0f;
    float hR = 1.0f;
    float wL = 1.0f;
    float wR = 1.0f;
    float xL = 0.0f;
    float yL = 0.0f;
    float xR = 0.0f;
    float yR = 0.0f;
    float gap = 1.0f;
  };

  TFT_eSPI& tft_;
  TFT_eSprite* face_ = nullptr;

  uint16_t colEye_ = 0xF800;
  uint16_t colBg_ = TFT_BLACK;

  Pose current_{};
  Pose target_{};
  Pose start_{};
  Pose rendered_{};

  HarveyExpr targetExpr_ = HarveyExpr::Idle;
  HarveyExpr currentExpr_ = HarveyExpr::Idle;

  uint32_t transStartMs_ = 0;
  uint32_t transMs_ = 300;
  uint32_t lastDrawMs_ = 0;
  uint32_t lastBlinkMs_ = 0;
  uint32_t nextBlinkMs_ = 2500;
  uint32_t nextQuirkMs_ = 0;
  uint32_t quirkEndMs_ = 0;
  uint8_t activeQuirk_ = QuirkNone;
  float blink_ = 0.0f;
  uint8_t blinkMode_ = 0;
  float scanT_ = 0.0f;

  int sprPad_ = 12;
  int baseEyeW_ = 0;
  int baseEyeH_ = 0;
  int baseGap_ = 0;
  int faceOx_ = 0;
  int faceOy_ = 0;
  int faceW_ = 0;
  int faceH_ = 0;
  bool spriteOk_ = false;
  bool pushedOnce_ = false;

  void layout();
  void poseFor(HarveyExpr e, Pose& p) const;
  static Pose lerpPose(const Pose& a, const Pose& b, float t);
  static float ease(float t);
  void stepBlink(uint32_t now);
  void rollQuirk(uint32_t now);
  void applyQuirks(uint32_t now);
  void applyMotion(uint32_t now, bool trans);
  void drawEye(TFT_eSPI& dst, int x, int y, int w, int h) const;
  void renderEyesToSprite() const;
  void pushSpriteAt(int ox, int oy, int eraseX, int eraseY, bool erase) const;
  void drawFrame();
  void animateSpriteMove(int x0, int y0, int x1, int y1, uint32_t ms);
  void playBootSequence();
  bool needsRedraw() const;
  bool isHeavyMotion() const;
};
