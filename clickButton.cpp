
#define CLICKBTN_PULLUP HIGH

#include <stdint.h>
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

#include "clickButton.h"

ClickButton::ClickButton(uint8_t buttonPin)
{
  _pin           = buttonPin;
  _activeHigh    = LOW;           // Assume active-low button
  _btnState      = !_activeHigh;  // initial button state in active-high logic
  _lastState     = _btnState;
  _clickCount    = 0;
  clicks         = 0;
  depressed      = false;
  _lastBounceTime= 0;
  debounceTime   = 20;            // Debounce timer in ms
  multiclickTime = 250;           // Time limit for multi clicks
  longClickTime  = 1000;          // time until long clicks register
  pinMode(_pin, INPUT);
}


ClickButton::ClickButton(uint8_t buttonPin, bool activeType)
{
  _pin           = buttonPin;
  _activeHigh    = activeType;
  _btnState      = !_activeHigh;  // initial button state in active-high logic
  _lastState     = _btnState;
  _clickCount    = 0;
  clicks         = 0;
  depressed      = 0;
  _lastBounceTime= 0;
  debounceTime   = 20;            // Debounce timer in ms
  multiclickTime = 250;           // Time limit for multi clicks
  longClickTime  = 1000;          // time until long clicks register
  pinMode(_pin, INPUT);
}

ClickButton::ClickButton(uint8_t buttonPin, bool activeType, bool internalPullup)
{
  _pin           = buttonPin;
  _activeHigh    = activeType;
  _btnState      = !_activeHigh;  // initial button state in active-high logic
  _lastState     = _btnState;
  _clickCount    = 0;
  clicks         = 0;
  depressed      = 0;
  _lastBounceTime= 0;
  debounceTime   = 20;            // Debounce timer in ms
  multiclickTime = 250;           // Time limit for multi clicks
  longClickTime  = 1000;          // time until "long" click register

  // Turn on internal pullup resistor if applicable

  if (_activeHigh == LOW && internalPullup == CLICKBTN_PULLUP)
        {
                pinMode(_pin, INPUT);
                pullUpDnControl(_pin, PUD_UP);
        }
  else
        {
                printf("setting _pin to INPUT\n");
                pinMode(_pin, INPUT);
        }
}

void ClickButton::Update()
{
  long now = (long)millis();      // get current time

  _btnState = digitalRead(_pin);  // current appearant button state

  // Make the button logic active-high in code
  if (!_activeHigh) _btnState = !_btnState;

  // If the switch changed, due to noise or a button press, reset the debounce timer
  if (_btnState != _lastState) _lastBounceTime = now;


  // debounce the button (Check if a stable, changed state has occured)
  if (now - _lastBounceTime > debounceTime && _btnState != depressed)
  {
    depressed = _btnState;
    if (depressed) _clickCount++;
  }

  // If the button released state is stable, report nr of clicks and start new cycle
  if (!depressed && (now - _lastBounceTime) > multiclickTime)
  {
    // positive count for released buttons
    clicks = _clickCount;
    _clickCount = 0;
  }

  // Check for "long click"
  if (depressed && (now - _lastBounceTime > longClickTime))
  {
    // negative count for long clicks
    clicks = 0 - _clickCount;
    _clickCount = 0;
  }

  _lastState = _btnState;
}
