#pragma once

#include "constants.h"
#include "monster.h"

enum State_T {
  hangry,
  chewing,
  happy,
  getting_hungry
};

/**This class handles the abstract class for each state. */
class State {
  private:
    unsigned long m_stateStartTime;
    unsigned long m_loopCount;
    char* m_stateName;
  protected:
    Monster* m_monster;
    unsigned long getTimePassedMs();
    unsigned long getLoopCount();
    double getLinearValueForCurrentTime(double startValue, double endValue, double timeRangeMs); 

    // Functions to be implemented by the actual states
    virtual void doInitialize();
    virtual State_T doLoop();
  public:
    State(char* stateName, Monster* monster);
    void initialize();
    State_T loop();
    char* getStateName();
};