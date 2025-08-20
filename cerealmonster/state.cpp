#include "state.h"

State::State(char* stateName, Monster* monster) {
  m_stateName = stateName;
  m_monster = monster;
  m_stateStartTime = 0;
  m_loopCount = 0;
}

void State::initialize() {
  m_stateStartTime = 0;
  m_loopCount = 0;
  doInitialize();
}

State_T State::loop() {
  doLoop();
  m_loopCount++;
}

char* State::getStateName() {
  return m_stateName;
}

unsigned long State::getTimePassedMs() {
  unsigned long currentTime = millis();
  return currentTime - m_stateStartTime;
}

unsigned long State::getLoopCount() {
  return m_loopCount;
}

double State::getLinearValueForCurrentTime(double startValue, double endValue, double timeRangeMs) {
  double valueRange = endValue - startValue;
  return (valueRange / timeRangeMs) * getTimePassedMs() + startValue;
}