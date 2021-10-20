#include <Arduino.h>
#include <Logger.h>
#include "CobController.h"

CobController::CobController() = default;

void CobController::init() {
    Logger::notice(LOG_TAG_COB, "initializing ...");
    ledcAttachPin(MOSFET_PIN_1, 0); // assign a led pins to a channel
    ledcAttachPin(MOSFET_PIN_2, 1); // assign a led pins to a channel
    ledcAttachPin(MOSFET_PIN_3, 2); // assign a led pins to a channel

    // Initialize channels
    // channels 0-15, resolution 1-16 bits, freq limits depend on resolution
    // ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution_bits);
    ledcSetup(0, 4000, 8); // 12 kHz PWM, 8-bit resolution
    ledcSetup(1, 4000, 8); // 12 kHz PWM, 8-bit resolution
    ledcSetup(2, 4000, 8); // 12 kHz PWM, 8-bit resolution
}

void CobController::loop(int* new_forward, int* new_backward, int* new_idle) {
  this->update();
   // is there a change detected
  if(old_forward != *(new_forward) || old_backward != *(new_backward)) {
    if(Logger::getLogLevel() == Logger::VERBOSE) {
      char buf[128];
      snprintf(buf, 128, "change detected: forward is %d was %d, backward is %d was %d",
         *(new_forward), old_forward, *(new_backward), old_backward);
      Logger::verbose(LOG_TAG_LED, buf);
    }

    this->changePattern(Pattern::FADE, (*new_forward) == HIGH, false);

    old_forward = *(new_forward);
    old_backward = *(new_backward);
  }

  //idle state???
  if(old_idle != *(new_idle)) {
    if(*(new_idle) == HIGH) {
      this->idleSequence();
    }
    old_idle = *(new_idle);
  }
}

void CobController::changePattern(Pattern pattern, boolean isForward, boolean repeatPattern) {
    //if (Logger::getLogLevel() == Logger::VERBOSE) {
    char buf[128];
    snprintf(buf, 128, "changePattern new pattern %d, forward %d, repeat %d", pattern, isForward, repeatPattern);
    Logger::notice(LOG_TAG_COB, buf);
    ///}

    if (activePattern == pattern && isForward == (direction == Direction::FORWARD)) {
        Logger::notice(LOG_TAG_COB, "changePattern Forwording and pattern is the same, not change");
        return;
    }
    stopPattern = false;
    repeat = repeatPattern;
    reverseOnComplete = true;
    activePattern = pattern;
    interval = 10;
    totalSteps = MAX_BRIGHTNESS_COB;
    if (isForward) {
        direction = Direction::FORWARD;
    } else {
        direction = Direction::REVERSE;
    }
    switch (pattern) {
        case RESCUE_FLASH_LIGHT:
            totalSteps = 5;
            break;
        default:
            index = direction == Direction::FORWARD ? 0 : totalSteps - 1;
            break;
    }
}

void CobController::update() {
    if (stopPattern)
        return;

    if ((millis() - lastUpdate) > interval) { // time to update
        switch (activePattern) {
            case FADE:
                fade();
                break;
            case RESCUE_FLASH_LIGHT:
                flash();
                break;
            default:
                break;
        }
        increment();
        lastUpdate = millis();
    }
}

// Increment the Index and reset at the end
void CobController::increment() {
    //Logger::notice(LOG_TAG_COB, "run increment");
    if (direction == FORWARD) {
        index++;
        if (index >= totalSteps) {
            index = 0;
            onComplete(); // call the comlpetion callback
        }
    } else { // Direction == REVERSE
        --index;
        if (index <= 0) {
            index = totalSteps - 1;
            onComplete(); // call the comlpetion callback
        }
    }
}

void CobController::onComplete() {
    char buf[128];
    snprintf(buf, 128, "onComplete, reverseOnComplete %d, repeat %d", reverseOnComplete, repeat);
    Logger::verbose(LOG_TAG_COB, buf);
    stopPattern = true;
    if (reverseOnComplete) {
        reverse();
        stopPattern = false;
        return;
    }
    if (repeat) {
        changePattern(activePattern, true, repeat);
        return;
    }
}

// Reverse pattern direction
void CobController::reverse() {
    //Serial.println("reverse: ");
    if (direction == FORWARD) {
        direction = REVERSE;
        index = totalSteps;
    } else {
        direction = FORWARD;
        index = 0;
    }
}

void CobController::fade() {
    writePWM(0, totalSteps - index);
    //writePWM(0, index);
    writePWM(1, 0);
    writePWM(2, index);
}

void CobController::flash() {
    if (index % 2 == 0) {
        if (direction == FORWARD) {
            writePWM(0, MAX_BRIGHTNESS_BRAKE_COB); // set the brightness LED
        } else {
            if (index % 4 == 2) {
                writePWM(1, MAX_BRIGHTNESS_BRAKE_COB); // set the brightness LED
            } else {
                writePWM(2, MAX_BRIGHTNESS_BRAKE_COB); // set the brightness LED
            }
        }
    } else {
        writePWM(0, 0); // turn off the white LED
        writePWM(1, 0); // turn off the red LED
        writePWM(2, 0); // turn off the green LED
    }
}

void CobController::stop() {
    //writePWM(0, 0); // turn off the white LED
    //writePWM(1, 0); // turn off the red LED
    //writePWM(2, 0); // turn off the green LED
}

void CobController::startSequence() {
    Logger::notice(LOG_TAG_COB, "run startSequence");
    changePattern(FADE, true, true);
}

void CobController::idleSequence() {
    //changePattern(FADE, true, true);
    //totalSteps = MAX_BRIGHTNESS_COB / 2;
    //reverseOnComplete = true;
}

void CobController::writePWM(int channel, int dutyCycle) {
    ledcWrite(channel, dutyCycle);
}