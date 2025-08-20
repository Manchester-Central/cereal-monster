#pragma once

#include "state.h"

class GettingHungryState: public State {
  public:
    GettingHungryState(Monster* monster);
  private:
    void doInitialize();
    State_T doLoop();
};