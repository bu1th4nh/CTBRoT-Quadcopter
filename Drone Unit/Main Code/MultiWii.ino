#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "config.h"
#include "def.h"
#include "types.h"
#include "optionalFeature.hpp"

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

#define CE_PIN   49
#define CSN_PIN  53
#define NRF_BUF_SIZE 20

//#define __AUTO_ACK___
#define __OPTIONAL_FEATURE__


struct RadioData 
{
    byte throttle;
    byte yaw;
    byte pitch;
    byte roll;
    byte switch_1;      //For SW1, SW2 - 6bits and button-L.
    byte switch_2;      //For SW3, SW4 - 6bits and button-R.

    /*          Bit representation:
            From bit 0 to bit 7: SW-down, SW-mid, SW-up, SW-down, SW-mid, SW-up, NONE, button - switch_1,2.
    */

};
//Acknowledgement Data Packet
struct AckPayload 
{
    float lat;
    float lon;
    int16_t heading;
    int16_t pitch;
    int16_t roll;  
    int32_t alt;
    byte flags;
};


const byte pipeAddress[] = "CTBers"; // Define the transmit pipe
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
extern int16_t rcData[RC_CHANS];
extern volatile int16_t failsafeCnt;
RadioData radioData;

void resetData()
{
	radioData.yaw = 127;
	radioData.roll = 127;
	radioData.pitch = 127;
	radioData.throttle = 0;
	radioData.switch_1 = 0;
	radioData.switch_2 = 0;
}
void configureNrfReceiver() 
{
	radio.begin();
	radio.openReadingPipe(0, pipeAddress);
	radio.setDataRate(RF24_250KBPS);
	radio.setPALevel(RF24_PA_MAX);
	radio.startListening();

    #ifdef __AUTO_ACK___
        radio.setAutoAck(1);                    //Ensure autoACK is enabled
        radio.enableAckPayload();               //Allow optional ack payloads
        radio.enableDynamicPayloads();
    #else
        radio.setAutoAck(false);
    #endif


	pinMode(26, OUTPUT);
	pinMode(27, OUTPUT);
}

/**************************************************************************************/
/***************          compute and Filter the RX data           ********************/
/**************************************************************************************/
void computeNrfRC() 
{
	resetData();
	if (radio.available())
	{
		// Read the data payload until we've received everything
		// Fetch the data payload
		radio.read(&radioData, sizeof(RadioData));
		// Serial.print("Yaw:      \t"); Serial.println(radioData.yaw);
		// Serial.print("Roll:     \t"); Serial.println(radioData.roll);
		// Serial.print("Pitch:    \t"); Serial.println(radioData.pitch);
		// Serial.print("Throttle: \t"); Serial.println(radioData.throttle);
		// Serial.println("");
		digitalWrite(26, 1);
		digitalWrite(27, 0);
	}
	else
	{
		digitalWrite(26, 0);
		digitalWrite(27, 1);
		return;
		// Serial.println("No radio available");
	}


	// Good signal, clear falsafe counter
	if (failsafeCnt > 20) failsafeCnt -= 20; else failsafeCnt = 0;

	rcData[YAW]      = map(radioData.yaw, 0, 255, 1000, 2000);
	rcData[ROLL]     = map(radioData.roll, 0, 255, 1000, 2000);
	rcData[PITCH]    = map(radioData.pitch, 0, 255, 1000, 2000);
	rcData[THROTTLE] = map(radioData.throttle, 0, 255, 1000, 2000);


	#ifdef __OPTIONAL_FEATURE__
		if((radioData.switch_1 >> 0) & 1) 
		{
			digitalWrite(LED_PIN_G1, 1);
		}
		else if((radioData.switch_1 >> 2) & 1)
		{
			digitalWrite(LED_PIN_R1, 1);
		}
		else 
		{
			digitalWrite(LED_PIN_R1, 0);
			digitalWrite(LED_PIN_G1, 0);
		}

		
		if((radioData.switch_2 >> 3) & 1) 
		{
			digitalWrite(LED_PIN_G2, 1);
		}
		else if((radioData.switch_2 >> 5) & 1)
		{
			digitalWrite(LED_PIN_R2, 1);
		}
		else 
		{
			digitalWrite(LED_PIN_R2, 0);
			digitalWrite(LED_PIN_G2, 0);
		}
	#endif


}
