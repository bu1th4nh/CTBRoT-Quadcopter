#include <nRF24L01.h>
#include <RF24.h>
#include "config.h"
#include "def.h"
#include "types.h"

/*
* Welcome to MultiWii.
*
* If you see this message, chances are you are using the Arduino IDE. That is ok.
* To get the MultiWii program configured for your copter, you must switch to the tab named 'config.h'.
* Maybe that tab is not visible in the list at the top, then you must use the drop down list at the right
* to access that tab. In that tab you must enable your baord or sensors and optionally various features.
* For more info go to http://www.multiwii.com/wiki/index.php?title=Main_Page
*
* Have fun, and do not forget MultiWii is made possible and brought to you under the GPL License.
*
*/
#define CE_PIN   A3
#define CSN_PIN  A2

#define NRF_BUF_SIZE 20
// define buffer indexes
enum BUFFER_INDEXES
{
	IDX_IDENTICAL1, IDX_IDENTICAL2,
	IDX_JOY1BUTTON, IDX_JOY1_X, IDX_JOY1_Y,
	IDX_JOY2BUTTON, IDX_JOY2_X, IDX_JOY2_Y,
	IDX_JOY_LEFT_BTN, IDX_JOY_RIGHT_BTN,
	IDX_JOY_THROTTLE
};

const uint64_t pipe = 0xF6F6DD5566LL; // Define the transmit pipe
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
int joystick[NRF_BUF_SIZE];  // element array holding Joystick readings
extern int16_t rcData[RC_CHANS];
extern volatile int16_t failsafeCnt;


void configureNrfReceiver() {
	radio.begin();
	//radio.setPALevel(RF24_PA_LOW);
	radio.openReadingPipe(1, pipe);
	radio.startListening();
}

/**************************************************************************************/
/***************          compute and Filter the RX data           ********************/
/**************************************************************************************/
void computeNrfRC() {
	for (int i = 0; i < NRF_BUF_SIZE; i++)
		joystick[i] = 0;
	if (radio.available())
	{
		// Read the data payload until we've received everything
		// Fetch the data payload
		radio.read(joystick, sizeof(joystick));
	}
	else
	{
		return;
		//Serial.println("No radio available");
	}
	if (joystick[IDX_IDENTICAL1] != 0x7fbe || joystick[IDX_IDENTICAL2] != 0x6e7f)
	{
		return;
	}
	// Good signal, clear falsafe counter
	if (failsafeCnt > 20) failsafeCnt -= 20; else failsafeCnt = 0;

	rcData[ROLL] = map(joystick[IDX_JOY1_X], 0, 1023, 1400, 1600);
	rcData[PITCH] = map(joystick[IDX_JOY2_Y], 0, 1023, 1400, 1600);
	rcData[THROTTLE] = map(joystick[IDX_JOY_THROTTLE], 0, 1023, 1000, 2000);
	rcData[YAW] = map(joystick[IDX_JOY2_X], 0, 1023, 1000, 2000);
	rcData[AUX1] = map(joystick[IDX_JOY_LEFT_BTN], 0, 1, 1000, 2000);
	rcData[AUX2] = map(joystick[IDX_JOY_RIGHT_BTN], 0, 1, 1000, 2000);
	rcData[AUX3] = map(joystick[IDX_JOY1BUTTON], 0, 1, 1000, 2000);
	rcData[AUX4] = map(joystick[IDX_JOY2BUTTON], 0, 1, 1000, 2000);

}
