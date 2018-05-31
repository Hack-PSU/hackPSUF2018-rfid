#include "hackPSUstatemachine.h"

namespace hackPSU {

	StateMachine::StateMachine() {
		for (uint8_t i = 0; i < StateMachine::MAX_STATES; i++) {
			this->state[i] = nullptr;
		}
		this->presentState = 0;
		this->finalState = 255;
	}
  
	uint8_t StateMachine::getNumStates(void) {
		return this->finalState + 1;
	}

	uint8_t StateMachine::getPresentState(void) {
		return this->presentState;
	}

	bool StateMachine::addState(callback state) {
		if (this->finalState >= StateMachine::MAX_STATES)
			return false;
		this->state[++(this->finalState)] = state;
		return true;
	}

	void StateMachine::removeState(callback state) {
		for (uint8_t i = 0; i < StateMachine::MAX_STATES; i++) {
			if (this->state[i] == state) {
				this->state[i] = nullptr;
				while(i < StateMachine::MAX_STATES - 1) {
					this->state[i] = this->state[++i];
				}
				break;
			}
		}
		this->finalState--;
	}

	void StateMachine::setState(uint8_t state) {
		this->presentState = state;
	}

	void StateMachine::start(void) {

		while (this->state[presentState] != nullptr) {

			this->state[presentState]();
			if (++this->presentState > this->finalState) {
				this->presentState = 0;
			}

			yield();

		}

	}

}

