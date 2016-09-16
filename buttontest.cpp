#include <wiringPi.h>
#include "clickButton.h"

/* ClickButton library demo

  Output a message on Serial according to different clicks on one button.

  Short clicks:

    Single click -
    Double click -
    Triple click -

  Long clicks (hold button for one second or longer on last click):

    Single-click -
    Double-click -
    Triple-click -

  2010, 2013 raron

 GNU GPLv3 license
*/

// build with g++ buttontest.cpp clickButton.cpp -lwiringPi 

int main(void)
{

	if(wiringPiSetup() == -1){ //when initialize wiring failed,print messageto screen
                printf("setup wiringPi failed !");
                return 1;
        }

	// the Button
	const int buttonPin1 = 27;
	ClickButton button1(buttonPin1, LOW, CLICKBTN_PULLUP);

	// Button results
	int function = 0;

//	pullUpDnControl(buttonPin1, PUD_UP);

	// Setup button timers (all in milliseconds / ms)
	// (These are default if not set, but changeable for convenience)
	button1.debounceTime   = 20;   // Debounce timer in ms
	button1.multiclickTime = 250;  // Time limit for multi clicks
	button1.longClickTime  = 1000; // time until "held-down clicks" register

	while(1){
  		// Update button state
  		button1.Update();

  		// Save click codes in LEDfunction, as click codes are reset at next Update()
		if (button1.clicks != 0) function = button1.clicks;

		if(button1.clicks == 1) printf("SINGLE click\n");

		if(function == 2) printf("DOUBLE click\n");

		if(function == 3) printf("TRIPLE click\n");

		if(function == -1) printf("SINGLE LONG click\n");

		if(function == -2) printf("DOUBLE LONG click\n");

		if(function == -3) printf("TRIPLE LONG click\n");

		function = 0;
		delay(5);
	}
}
