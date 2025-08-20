#include "state_happy.h"

struct Color HAPPY_COLOR = {0, 20, 0};
struct EyebrowPosition HAPPY_EYEBROWS = {120, 60};
#define HAPPY_TIME_MS 4000

HappyState::HappyState(Monster* monster) : State("Happy", monster) {}

void HappyState::doInitialize() {
  m_monster->setEyebrowPosition(HAPPY_EYEBROWS);
  m_monster->setEyeColor(HAPPY_COLOR);
  m_monster->playSound("/happy.mp3");
}

State_T HappyState::doLoop() {
  if (getTimePassedMs() > HAPPY_TIME_MS) {
    return getting_hungry;
  }
  return happy;
}
