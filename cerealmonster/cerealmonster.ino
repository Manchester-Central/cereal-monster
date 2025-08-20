#include "constants.h"
#include "monster.h"
#include "state.h"
#include "state_hangry.h"
#include "state_chewing.h"
#include "state_happy.h"
#include "state_getting_hungry.h"

// Monster contains all the code we need for controlling hardware on the physical monster
Monster* monster = new Monster();

State_T currentStateT = happy;

// Define all states here and add to the `getState` function
HangryState* hangryState = new HangryState(monster);
ChewingState* chewingState = new ChewingState(monster);
HappyState* happyState = new HappyState(monster);
GettingHungryState* gettingHungryState = new GettingHungryState(monster);

State* getState(State_T state) {
  switch(state) {
    case hangry:
      return hangryState;
    case chewing:
      return chewingState;
    case happy:
      return happyState;
    case getting_hungry:
      return gettingHungryState;
    default:
      return hangryState;
  }
}

void setup() {
  monster->setup();
  changeState(hangry);
}

void changeState(State_T newStateT) {
  State* previousState = getState(currentStateT);
  State* newState = getState(newStateT);
  Serial.print("State transition: ");
  Serial.print(previousState->getStateName());
  Serial.print(" -> ");
  Serial.print(newState->getStateName());
  Serial.print("\n");

  monster->stopSounds();
  currentStateT = newStateT;

  newState->initialize();
}


void loop() {
  // put your main code here, to run repeatedly:
  State* state = getState(currentStateT);
  State_T requestedNewStateT = state->loop();
  if (requestedNewStateT != currentStateT) {
    changeState(requestedNewStateT);
  }
}



