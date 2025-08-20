#pragma once

#include "state.h"

class HappyState: public State {
  public:
    HappyState(Monster* monster);
  private:
    void doInitialize();
    State_T doLoop();
};