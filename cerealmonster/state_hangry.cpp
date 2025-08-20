#include "state_hangry.h"

struct Color HANGRY_COLOR = {20, 0, 0};
struct EyebrowPosition HANGRY_EYEBROWS = {70, 110};

HangryState::HangryState(Monster* monster) : State("Hangry", monster) {}

void HangryState::doInitialize() {
  m_monster->setEyebrowPosition(HANGRY_EYEBROWS);
  m_monster->setEyeColor(HANGRY_COLOR);
}

State_T HangryState::doLoop() {
  if (m_monster->isFed()) {
    return chewing;
  }
  return hangry;
}
