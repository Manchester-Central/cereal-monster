#include "state_chewing.h"

// Chewing State Constants
struct Color CHEWING_COLOR = {15, 5, 0};
struct EyebrowPosition CHEWING_EYEBROWS = {90, 90};
#define CHEWING_TIME_MS 5000

ChewingState::ChewingState(Monster* monster) : State("Chewing", monster) {}

void ChewingState::doInitialize() {
  m_monster->setEyebrowPosition(CHEWING_EYEBROWS);
  m_monster->setEyeColor(CHEWING_COLOR);
  m_monster->playSound("/chewing.mp3");
}

State_T ChewingState::doLoop() {
  if (getTimePassedMs() > CHEWING_TIME_MS) {
    return happy;
  }
  return chewing;
}
