#ifndef __COB_CONTROLLER_H__
#define __COB_CONTROLLER_H__

#include "config.h"
#include "ILedController.h"

#ifndef MOSFET_PIN_1
 #define MOSFET_PIN_1 22
#endif //MOSFET_PIN_1

#ifndef MOSFET_PIN_2
 #define MOSFET_PIN_2  23
#endif //MOSFET_PIN_2

#ifndef MOSFET_PIN_3
 #define MOSFET_PIN_3  32
#endif //MOSFET_PIN_3

#define LOG_TAG_COB "CobController"

class CobController : public ILedController {
    public:
      CobController();
        void init() override;
        void changePattern(Pattern pattern, boolean isForward, boolean repeatPattern) override;
        void update() override;
        void stop() override;
        void startSequence() override;
        void idleSequence() override;
        void loop(int* new_forward, int* new_backward, int* new_idle);

    private:
      void fade();
      void flash();
      void increment();
      void onComplete();
      void reverse();
      static void writePWM(int channel, int dutyCycle);
      unsigned long interval    = 0;     // milliseconds between updates
      unsigned long lastUpdate  = 0;     // last update of position
      boolean stopPattern       = false; // is pattern stopped
      boolean repeat            = true; // repeat the pattern infinitly
      boolean reverseOnComplete = true; // reverse the pattern onComplete
      Pattern activePattern     = FADE;
      Direction direction       = FORWARD;
      uint16_t totalSteps       = 0;     // total number of steps in the pattern
      uint16_t index            = 0;     // current step within the pattern
      int old_forward           = LOW;
      int old_backward          = LOW;
      int old_idle              = LOW;
};

#endif //__COB_CONTROLLER_H__