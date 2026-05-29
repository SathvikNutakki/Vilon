#include "HarveyFace.h"

namespace {

float clamp01(float v) {
  if (v < 0.0f) return 0.0f;
  if (v > 1.0f) return 1.0f;
  return v;
}

int iMin(int a, int b) { return a < b ? a : b; }
int iMax(int a, int b) { return a > b ? a : b; }

uint16_t eyeRedForPanel(TFT_eSPI& tft) {
#if TFT_DISPLAY_BGR565
  (void)tft;
  return 0x001F;
#else
  return tft.color565(255, 0, 0);
#endif
}

}  // namespace

HarveyExpr harveyExprFromCommand(const String& cmd) {
  String c = cmd;
  c.trim();
  c.toLowerCase();

  if (c.indexOf("moveforward") >= 0 || c.indexOf("forward") >= 0) return HarveyExpr::SmugForward;
  if (c.indexOf("movebackward") >= 0 || c.indexOf("backward") >= 0) return HarveyExpr::AnnoyedRetreat;
  if (c.indexOf("turnleft") >= 0) return HarveyExpr::GlanceLeft;
  if (c.indexOf("turnright") >= 0) return HarveyExpr::GlanceRight;
  if (c.indexOf("stop") >= 0 || c.indexOf("halt") >= 0 || c.indexOf("freeze") >= 0) return HarveyExpr::DoneStop;
  if (c.indexOf("howareyou") >= 0) return HarveyExpr::SmugHowAreYou;
  if (c.indexOf("dance") >= 0) return HarveyExpr::ReluctantDance;
  if (c.indexOf("temperature") >= 0 || c.indexOf("temp") >= 0) return HarveyExpr::DeadpanTemp;
  if (c.indexOf("distance") >= 0 || c.indexOf("obstacle") >= 0 || c.indexOf("surround") >= 0)
    return HarveyExpr::ScanDistance;
  if (c.indexOf("gotosleep") >= 0 || c.indexOf("sleep") >= 0 || c.indexOf("shut") >= 0) return HarveyExpr::Sleep;
  if (c.indexOf("followme") >= 0 || c.indexOf("follow") >= 0) return HarveyExpr::SoftFollow;
  if (c.indexOf("scare") >= 0 || c.indexOf("crazy") >= 0) return HarveyExpr::Scare;

  return HarveyExpr::Idle;
}

HarveyFace::HarveyFace(TFT_eSPI& display) : tft_(display) {}

HarveyFace::~HarveyFace() {
  if (face_) {
    face_->deleteSprite();
    delete face_;
    face_ = nullptr;
  }
}

void HarveyFace::layout() {
  const int W = tft_.width();
  const int H = tft_.height();
  const int margin = 6;
  const int availW = iMax(40, W - margin * 2);
  const int availH = iMax(40, H - margin * 2);

  sprPad_ = 12;
  baseEyeW_ = iMax(20, iMin(36, availW / 9));
  baseGap_ = iMax(18, iMin(availW / 5, availW - sprPad_ * 2 - baseEyeW_ * 2));

  faceW_ = sprPad_ * 2 + baseEyeW_ * 2 + baseGap_;
  if (faceW_ > availW) {
    baseGap_ = iMax(12, availW - sprPad_ * 2 - baseEyeW_ * 2);
    faceW_ = sprPad_ * 2 + baseEyeW_ * 2 + baseGap_;
  }

  baseEyeH_ = iMax(40, iMin((int)(availH * 0.72f), availH - sprPad_ * 2));
  faceH_ = sprPad_ * 2 + baseEyeH_;
  if (faceH_ > availH) {
    baseEyeH_ = availH - sprPad_ * 2;
    faceH_ = availH;
  }

  faceOx_ = margin + (availW - faceW_) / 2;
  faceOy_ = margin + (availH - faceH_) / 2;
  if (faceOx_ < 0) faceOx_ = 0;
  if (faceOy_ < 0) faceOy_ = 0;
}

void HarveyFace::begin() {
  layout();
  colEye_ = eyeRedForPanel(tft_);
  colBg_ = TFT_BLACK;

  tft_.fillScreen(colBg_);

  if (face_) {
    face_->deleteSprite();
    delete face_;
    face_ = nullptr;
  }

  face_ = new TFT_eSprite(&tft_);
  spriteOk_ = face_->createSprite(faceW_, faceH_);
  if (!spriteOk_) {
    Serial.println("HarveyFace: sprite alloc failed");
    delete face_;
    face_ = nullptr;
    return;
  }

  poseFor(HarveyExpr::Idle, current_);
  target_ = current_;
  start_ = current_;
  rendered_ = current_;
  targetExpr_ = HarveyExpr::Idle;
  currentExpr_ = HarveyExpr::Idle;
  pushedOnce_ = false;

  uint32_t now = millis();
  lastDrawMs_ = 0;
  lastBlinkMs_ = now;
  nextBlinkMs_ = now + 2000 + (esp_random() % 2500);
  nextQuirkMs_ = now + 4000 + (esp_random() % 5000);
  activeQuirk_ = QuirkNone;

  playBootSequence();
}

void HarveyFace::setExpression(HarveyExpr expr, bool immediate) {
  if (expr >= HarveyExpr::Count) expr = HarveyExpr::Idle;
  if (expr == targetExpr_ && !immediate) return;

  targetExpr_ = expr;
  start_ = current_;
  poseFor(expr, target_);
  transStartMs_ = millis();
  activeQuirk_ = QuirkNone;

  if (expr == HarveyExpr::SoftFollow) {
    activeQuirk_ = QuirkFollowTwitch;
    quirkEndMs_ = millis() + 650;
  } else if (expr == HarveyExpr::SmugHowAreYou && (esp_random() % 3) == 0) {
    activeQuirk_ = QuirkWinkHold;
    quirkEndMs_ = millis() + 500;
  }

  if (immediate) {
    current_ = target_;
    currentExpr_ = expr;
    drawFrame();
  }
}

void HarveyFace::setExpressionFromCommand(const String& cmd) {
  setExpression(harveyExprFromCommand(cmd));
}

float HarveyFace::ease(float t) {
  t = clamp01(t);
  return t * t * (3.0f - 2.0f * t);
}

HarveyFace::Pose HarveyFace::lerpPose(const Pose& a, const Pose& b, float t) {
  Pose p;
  auto m = [&](float x, float y) { return x + (y - x) * t; };
  p.hL = m(a.hL, b.hL);
  p.hR = m(a.hR, b.hR);
  p.wL = m(a.wL, b.wL);
  p.wR = m(a.wR, b.wR);
  p.xL = m(a.xL, b.xL);
  p.yL = m(a.yL, b.yL);
  p.xR = m(a.xR, b.xR);
  p.yR = m(a.yR, b.yR);
  p.gap = m(a.gap, b.gap);
  return p;
}

void HarveyFace::poseFor(HarveyExpr e, Pose& p) const {
  p = Pose{};

  switch (e) {
    case HarveyExpr::Idle:
      break;
    case HarveyExpr::SmugForward:
      p.hL = 0.9f;
      p.hR = 0.58f;
      p.wR = 0.88f;
      p.yL = p.yR = -3.0f;
      break;
    case HarveyExpr::AnnoyedRetreat:
      p.hL = p.hR = 0.38f;
      p.wL = p.wR = 0.78f;
      p.yL = p.yR = 5.0f;
      break;
    case HarveyExpr::GlanceLeft:
      p.xL = -16.0f;
      p.xR = -16.0f;
      p.hR = 1.05f;
      p.hL = 0.65f;
      break;
    case HarveyExpr::GlanceRight:
      p.xL = 16.0f;
      p.xR = 16.0f;
      p.hL = 1.05f;
      p.hR = 0.65f;
      break;
    case HarveyExpr::DoneStop:
      p.hL = p.hR = 0.3f;
      p.wL = p.wR = 0.86f;
      break;
    case HarveyExpr::SmugHowAreYou:
      p.hL = 0.98f;
      p.hR = 0.45f;
      p.wR = 0.82f;
      p.yL = p.yR = -4.0f;
      break;
    case HarveyExpr::ReluctantDance:
      p.hL = p.hR = 1.1f;
      p.wL = 0.9f;
      p.wR = 1.05f;
      break;
    case HarveyExpr::DeadpanTemp:
      p.hL = p.hR = 0.16f;
      p.wL = p.wR = 0.8f;
      p.yL = p.yR = 4.0f;
      break;
    case HarveyExpr::ScanDistance:
      p.hL = p.hR = 1.15f;
      p.wL = p.wR = 0.88f;
      break;
    case HarveyExpr::Sleep:
      p.hL = p.hR = 0.05f;
      p.wL = p.wR = 0.85f;
      break;
    case HarveyExpr::SoftFollow:
      p.hL = 1.12f;
      p.hR = 0.42f;
      p.wL = 0.7f;
      p.wR = 1.08f;
      p.xL = 12.0f;
      p.xR = -10.0f;
      p.yL = -2.0f;
      p.yR = 7.0f;
      p.gap = 0.68f;
      break;
    case HarveyExpr::Scare:
      p.hL = p.hR = 1.25f;
      p.wL = p.wR = 0.95f;
      p.gap = 1.2f;
      break;
    default:
      break;
  }
}

bool HarveyFace::isHeavyMotion() const {
  return targetExpr_ == HarveyExpr::Scare || targetExpr_ == HarveyExpr::ReluctantDance ||
         targetExpr_ == HarveyExpr::ScanDistance;
}

void HarveyFace::stepBlink(uint32_t now) {
  if (targetExpr_ == HarveyExpr::Sleep) {
    blink_ = 0.0f;
    return;
  }

  if (blink_ > 0.001f) {
    uint32_t el = now - lastBlinkMs_;
    if (el < 70) {
      blink_ = 1.0f - el / 70.0f;
    } else if (el < 140) {
      blink_ = (el - 70) / 70.0f;
    } else {
      blink_ = 0.0f;
      lastBlinkMs_ = now;
      nextBlinkMs_ = now + 2200 + (esp_random() % 3500);
      blinkMode_ = esp_random() % 4;
    }
    return;
  }

  if (now - lastBlinkMs_ > nextBlinkMs_) {
    lastBlinkMs_ = now;
    blink_ = 1.0f;
    blinkMode_ = esp_random() % 4;
  }
}

void HarveyFace::rollQuirk(uint32_t now) {
  if (now < quirkEndMs_) return;
  if (now < nextQuirkMs_) return;
  if (targetExpr_ == HarveyExpr::Sleep || isHeavyMotion()) return;

  activeQuirk_ = QuirkNone;
  nextQuirkMs_ = now + 6000 + (esp_random() % 10000);

  uint8_t roll = esp_random() % 100;
  if (targetExpr_ == HarveyExpr::SoftFollow) {
    activeQuirk_ = QuirkFollowTwitch;
  } else if (roll < 20) {
    activeQuirk_ = QuirkWinkHold;
  } else if (roll < 35) {
    activeQuirk_ = QuirkCrossStare;
  } else if (roll < 50) {
    activeQuirk_ = QuirkVertigo;
  } else if (roll < 62) {
    activeQuirk_ = QuirkLazyLag;
  } else if (roll < 74) {
    activeQuirk_ = QuirkPuffShrink;
  } else if (roll < 86) {
    activeQuirk_ = QuirkDoubleTake;
  } else if (roll < 94) {
    activeQuirk_ = QuirkCaffeine;
  } else {
    return;
  }

  quirkEndMs_ = now + 300 + (esp_random() % 550);
}

void HarveyFace::applyQuirks(uint32_t now) {
  if (activeQuirk_ == QuirkNone || now >= quirkEndMs_) {
    activeQuirk_ = QuirkNone;
    return;
  }

  const float t = (now % 360) * 0.017f;

  switch (activeQuirk_) {
    case QuirkWinkHold:
      current_.hR *= 0.12f;
      break;
    case QuirkCrossStare:
      current_.xL += 10.0f;
      current_.xR -= 10.0f;
      current_.gap *= 0.82f;
      break;
    case QuirkVertigo:
      current_.xL += sinf(t * 9.0f) * 7.0f;
      current_.xR += cosf(t * 8.0f) * 7.0f;
      break;
    case QuirkLazyLag:
      current_.hL *= 0.55f;
      current_.yL += 6.0f;
      break;
    case QuirkPuffShrink:
      current_.wL *= 1.2f;
      current_.wR *= 0.65f;
      current_.hR *= 1.15f;
      current_.hL *= 0.75f;
      break;
    case QuirkFollowTwitch:
      current_.hR = target_.hR * (0.35f + 0.1f * sinf(t * 14.0f));
      current_.xR += sinf(t * 11.0f) * 6.0f;
      break;
    case QuirkDoubleTake:
      current_.xL -= 12.0f;
      current_.xR -= 12.0f;
      current_.hL = current_.hR = 1.15f;
      break;
    case QuirkCaffeine:
      current_.hL *= 1.18f;
      current_.hR *= 1.18f;
      current_.yL -= 4.0f;
      current_.yR -= 4.0f;
      break;
    default:
      break;
  }
}

void HarveyFace::applyMotion(uint32_t now, bool trans) {
  rollQuirk(now);
  applyQuirks(now);

  if (targetExpr_ == HarveyExpr::ReluctantDance) {
    float w = sinf(now * 0.02f);
    current_.hL = target_.hL + w * 0.14f;
    current_.hR = target_.hR - w * 0.14f;
    current_.xL = target_.xL + w * 5.0f;
    current_.xR = target_.xR - w * 5.0f;
  }

  if (targetExpr_ == HarveyExpr::ScanDistance) {
    scanT_ += 0.14f;
    float s = sinf(scanT_) * 18.0f;
    current_.xL = target_.xL + s;
    current_.xR = target_.xR + s;
  }

  if (targetExpr_ == HarveyExpr::Scare) {
    float j = sinf(now * 0.022f) * 6.0f;
    current_.xL = target_.xL + j;
    current_.xR = target_.xR - j;
    current_.hL = target_.hL + fabsf(sinf(now * 0.035f)) * 0.18f;
    current_.hR = target_.hR + fabsf(cosf(now * 0.03f)) * 0.18f;
  }

  (void)trans;
}

void HarveyFace::drawEye(TFT_eSPI& dst, int x, int y, int w, int h) const {
  if (h < 4) {
    dst.fillRoundRect(x, y, w, 4, 2, colEye_);
    return;
  }
  int r = w / 2;
  if (r < 2) r = 2;
  dst.fillRoundRect(x, y, w, h, r, colEye_);
}

void HarveyFace::renderEyesToSprite() const {
  if (!spriteOk_ || !face_) return;

  face_->fillSprite(colBg_);

  const int gap = iMax(8, (int)(baseGap_ * current_.gap));
  const int midY = faceH_ / 2;

  auto dims = [&](float hs, float ws, int side, int& ow, int& oh) {
    oh = iMax(4, (int)(baseEyeH_ * hs * (1.0f - blink_)));
    ow = iMax(6, (int)(baseEyeW_ * ws));
    if (blinkMode_ == 1 && side < 0) oh = iMax(3, (int)(baseEyeH_ * hs * (1.0f - blink_ * 0.15f)));
    if (blinkMode_ == 2 && side > 0) oh = iMax(3, (int)(baseEyeH_ * hs * (1.0f - blink_ * 0.15f)));
    if (blinkMode_ == 3) oh = iMax(3, (int)(oh * (side < 0 ? 0.2f : 1.0f)));
  };

  int lw, lh, rw, rh;
  dims(current_.hL, current_.wL, -1, lw, lh);
  dims(current_.hR, current_.wR, 1, rw, rh);

  int lx = sprPad_ + (int)current_.xL;
  int rx = sprPad_ + gap + lw + (int)current_.xR;
  int ly = midY - lh / 2 + (int)current_.yL;
  int ry = midY - rh / 2 + (int)current_.yR;

  drawEye(*face_, lx, ly, lw, lh);
  drawEye(*face_, rx, ry, rw, rh);
}

void HarveyFace::pushSpriteAt(int ox, int oy, int eraseX, int eraseY, bool erase) const {
  if (!spriteOk_ || !face_) return;

  tft_.startWrite();
  if (erase) {
    tft_.fillRect(eraseX, eraseY, faceW_, faceH_, colBg_);
  }
  face_->pushSprite(ox, oy);
  tft_.endWrite();
}

void HarveyFace::drawFrame() {
  renderEyesToSprite();
  pushSpriteAt(faceOx_, faceOy_, faceOx_, faceOy_, false);
  rendered_ = current_;
  pushedOnce_ = true;
}

void HarveyFace::animateSpriteMove(int x0, int y0, int x1, int y1, uint32_t ms) {
  if (!spriteOk_) return;

  uint32_t t0 = millis();
  int lastX = x0;
  int lastY = y0;

  while (true) {
    uint32_t el = millis() - t0;
    float u = (ms > 0) ? ease(el / (float)ms) : 1.0f;
    int x = x0 + (int)((x1 - x0) * u);
    int y = y0 + (int)((y1 - y0) * u);

    renderEyesToSprite();
    pushSpriteAt(x, y, lastX, lastY, true);
    lastX = x;
    lastY = y;

    if (el >= ms) break;
    delay(22);
  }
}

void HarveyFace::playBootSequence() {
  if (!spriteOk_) return;

  const int margin = 6;
  const int W = tft_.width();
  const int H = tft_.height();
  const int homeX = faceOx_;
  const int homeY = faceOy_;

  const int corners[4][2] = {{margin, margin},
                             {W - faceW_ - margin, margin},
                             {W - faceW_ - margin, H - faceH_ - margin},
                             {margin, H - faceH_ - margin}};

  tft_.fillScreen(colBg_);

  blink_ = 0;
  current_.hL = current_.hR = 1.08f;
  current_.wL = current_.wR = 0.9f;
  current_.gap = 1.0f;
  current_.xL = 0.0f;
  current_.xR = 0.0f;
  current_.yL = 0.0f;
  current_.yR = 0.0f;

  int px = homeX;
  int py = homeY;
  renderEyesToSprite();
  pushSpriteAt(px, py, px, py, false);
  delay(120);

  animateSpriteMove(px, py, corners[0][0], corners[0][1], 240);
  px = corners[0][0];
  py = corners[0][1];
  delay(80);

  animateSpriteMove(px, py, corners[1][0], corners[1][1], 240);
  px = corners[1][0];
  py = corners[1][1];
  delay(80);

  animateSpriteMove(px, py, corners[2][0], corners[2][1], 240);
  px = corners[2][0];
  py = corners[2][1];
  delay(80);

  animateSpriteMove(px, py, corners[3][0], corners[3][1], 240);
  px = corners[3][0];
  py = corners[3][1];
  delay(80);

  animateSpriteMove(px, py, homeX, homeY, 320);
  px = homeX;
  py = homeY;
  delay(100);

  poseFor(HarveyExpr::Idle, current_);
  target_ = current_;
  blink_ = 0.0f;

  renderEyesToSprite();
  pushSpriteAt(homeX, homeY, px, py, true);

  rendered_ = current_;
  pushedOnce_ = true;
  lastDrawMs_ = millis();

  Serial.println("Boot sequence done.");
}

bool HarveyFace::needsRedraw() const {
  if (blink_ > 0.02f) return true;
  if (activeQuirk_ != QuirkNone) return true;

  auto changed = [&](float a, float b) { return fabsf(a - b) > 0.02f; };
  const Pose& a = current_;
  const Pose& b = rendered_;

  if (changed(a.hL, b.hL) || changed(a.hR, b.hR) || changed(a.wL, b.wL) || changed(a.wR, b.wR) ||
      changed(a.xL, b.xL) || changed(a.xR, b.xR) || changed(a.yL, b.yL) || changed(a.yR, b.yR) ||
      changed(a.gap, b.gap)) {
    return true;
  }

  return false;
}

void HarveyFace::update() {
  if (!spriteOk_) return;

  uint32_t now = millis();

  float t = (float)(now - transStartMs_) / (float)transMs_;
  bool trans = t < 1.0f;
  if (trans) {
    current_ = lerpPose(start_, target_, ease(t));
  } else {
    current_ = target_;
    currentExpr_ = targetExpr_;
  }

  applyMotion(now, trans);
  stepBlink(now);

  if (!needsRedraw()) return;

  uint32_t minGap = 250;
  if (trans) {
    minGap = 35;
  } else if (blink_ > 0.02f) {
    minGap = 35;
  } else if (activeQuirk_ != QuirkNone) {
    minGap = 40;
  } else if (isHeavyMotion()) {
    minGap = 50;
  }

  if (now - lastDrawMs_ < minGap) return;

  lastDrawMs_ = now;
  drawFrame();
}
