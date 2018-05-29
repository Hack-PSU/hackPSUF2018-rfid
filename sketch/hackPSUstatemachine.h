#pragma once

/**
 * The type callback references a function like: void foo(void).  This is to simplify the StateMachine class
 */
typedef void (*callback)(void);

/**
 * This class abstracts the FSM that is the control structure for the rfid reader.  This class also overrides Arduino's native watchdog timer conrols to prevent unexpected reboots.
 */
class StateMachine {
  private:
    static const uint8_t MAX_STATES = 16;
    uint8_t presentState;
    uint8_t finalState;
    callback state[MAX_STATES];
    
  public:
    uint8_t getNumStates(void);
    uint8_t getPresentState(void);
    bool addState(callback);
    void removeState(callback);
    void setState(uint8_t);
    void start(void);
    
}

