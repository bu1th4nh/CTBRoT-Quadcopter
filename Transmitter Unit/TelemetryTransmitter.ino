/*  
        A basic multi-channel transmitter using the nRF24L01 module. Coded by bu1th4nh
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
#define RF24_CSN 9
#define RF24_CE  8
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

//#define _AUTO_ACK__


//------------------------------------------------------------------------
#include "utlilities.hpp"
#include "operations.hpp"
//------------------------------------------------------------------------
//Setup
void setup()
{
    radio.begin();
    Serial.begin(115200);
    printf_begin();
    radio.printDetails();
    
    
    radio.openWritingPipe(pipeAddress);
    radio.setDataRate(RF24_250KBPS);
    radio.setPALevel(RF24_PA_MAX);

    #ifdef _AUTO_ACK__
        radio.setAutoAck(1);                    //Ensure autoACK is enabled
        radio.enableAckPayload();               //Allow optional ack payloads
        radio.enableDynamicPayloads();
    #else
        radio.setAutoAck(false);
    #endif

    radio.printDetails();
    radio.stopListening();
    radio.printDetails();
    resetRadioData();


    pinMode(SW3_H, INPUT_PULLUP);
    pinMode(SW3_L, INPUT_PULLUP);
}



//------------------------------------------------------------------------
//Main loop
void loop()
{
    readJoystickData();
    readSwitchData();
    DataTrans();
    adjustAckPayloadValues();
    writeDataToSerial();
}