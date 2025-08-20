#include "state_getting_hungry.h"

#define GETTING_HUNGRY_TIME_MS 3000

GettingHungryState::GettingHungryState(Monster* monster) : State("Getting Hungry...", monster) {}

void GettingHungryState::doInitialize() {
  m_monster->setEyebrowPosition(HAPPY_EYEBROWS);
  m_monster->setEyeColor(HAPPY_COLOR);
}

State_T GettingHungryState::doLoop() {
  if (getTimePassedMs() > GETTING_HUNGRY_TIME_MS) {
    return hangry;
  }

  if (getLoopCount() % 10 == 0) {
    //y = -50/3000x + 120 -- Chi-Chi's original formula for the left eye
    m_monster->setEyebrowPosition(
      getLinearValueForCurrentTime(HAPPY_EYEBROWS.leftAngle, HANGRY_EYEBROWS.leftAngle, GETTING_HUNGRY_TIME_MS),
      getLinearValueForCurrentTime(HAPPY_EYEBROWS.rightAngle, HANGRY_EYEBROWS.rightAngle, GETTING_HUNGRY_TIME_MS)
    );
  }

  // Setting the eye color every loop causes slowness, so do this less frequently.
  if (getLoopCount() % 5000 == 0) {
    m_monster->setEyeColor(
       getLinearValueForCurrentTime(HAPPY_COLOR.r, HANGRY_COLOR.r, GETTING_HUNGRY_TIME_MS),
       getLinearValueForCurrentTime(HAPPY_COLOR.g, HANGRY_COLOR.g, GETTING_HUNGRY_TIME_MS),
       getLinearValueForCurrentTime(HAPPY_COLOR.b, HANGRY_COLOR.b, GETTING_HUNGRY_TIME_MS)
    );
  }
  return getting_hungry;
}
