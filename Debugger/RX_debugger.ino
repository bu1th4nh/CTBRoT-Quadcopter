/*  
        A basic 4 channel transmitter using the nRF24L01 module. Coded by bu1th4nh
        Special thanks to iforce2d YT Channel. Visit his channel here: https://www.youtube.com/channel/UCTXOorupCLqqQifs2jbz7rQ

        The telemetry feature is still WIP and will be updated in the near future
        Code using RF24 library form TMRh20. Check out his repository!: https://github.com/nRF24/RF24
*/

//------------------------------------------------------------------------
//Libraries and namespaces
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "printf.h"


//------------------------------------------------------------------------
//Macroes
#define RF24_CSN 53
#define RF24_CE  49
#define SW1 A6
#define SW2 A7
#define SW3_L  8
#define SW3_H  7
#define SW4_L  6
#define SW4_H  5
#define BUTT_L 4
#define BUTT_R 3


const byte pipeAddress[] = "CTBers";
//------------------------------------------------------------------------
//Data and ACK packets
//Main Data Packet - The sizeof this struct should not exceed 32 bytes
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

//------------------------------------------------------------------------
//Typedefs
const byte serialHeader[4] = {255,254,253,252};
RF24 radio(RF24_CE, RF24_CSN);
int pps = 0, ppsCounter = 0;
unsigned long lastPPS = 0;

AckPayload ackPayload;
RadioData radioData;

//#define __AUTO_ACK___

void resetRadioData() 
{
    radioData.throttle = 127;
    radioData.yaw = 127;
    radioData.pitch = 127;
    radioData.roll = 127;
    radioData.switch_1 = 0;
    radioData.switch_2 = 0;
}

//------------------------------------------------------------------------

//------------------------------------------------------------------------
//Setup
void setup()
{
    radio.begin();
    Serial.begin(57600);
    printf_begin();
    
    
    radio.openReadingPipe(0, pipeAddress);
    radio.setDataRate(RF24_250KBPS);
    radio.setPALevel(RF24_PA_MAX);

    #ifdef __AUTO_ACK___
        radio.setAutoAck(1);                    //Ensure autoACK is enabled
        radio.enableAckPayload();               //Allow optional ack payloads
        radio.enableDynamicPayloads();
    #else
        radio.setAutoAck(false);
    #endif

    radio.startListening();
    radio.printDetails();
    resetRadioData();
    pinMode(26, OUTPUT);
    pinMode(27, OUTPUT);
}

void loop()
{
    if(radio.available())
    {
        // Read the data payload until we've received everything
		// Fetch the data payload
		radio.read(&radioData, sizeof(RadioData));
		Serial.print("Yaw:      \t"); Serial.println(radioData.yaw);
		Serial.print("Roll:     \t"); Serial.println(radioData.roll);
		Serial.print("Pitch:    \t"); Serial.println(radioData.pitch);
		Serial.print("Throttle: \t"); Serial.println(radioData.throttle);
		Serial.println("");
		digitalWrite(26, 0);
		digitalWrite(27, 1);
    }
    else
    {
        Serial.println("No data");
        digitalWrite(26, 1);
		digitalWrite(27, 0);
    }

}