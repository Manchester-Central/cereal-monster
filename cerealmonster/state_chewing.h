#pragma once

#include "state.h"

class ChewingState: public State {
  public:
    ChewingState(Monster* monster);
  private:
    void doInitialize();
    State_T doLoop();
};