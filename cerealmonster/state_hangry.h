#pragma once

#include "state.h"

class HangryState: public State {
  public:
    HangryState(Monster* monster);
  private:
    void doInitialize();
    State_T doLoop();
};