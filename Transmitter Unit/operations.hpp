//Data reading
void readJoystickData()
{
    boolean mode1 = 0;
  
    // The calibration numbers used here should be measured 
    // for your joysticks using the TestJoysticks sketch.
    if ( mode1 ) 
    {
        radioData.throttle = mapJoystickValues( analogRead(A0), 180, 497, 807, true );
        radioData.yaw      = mapJoystickValues( analogRead(A3), 165, 564, 906, false );
        radioData.pitch    = mapJoystickValues( analogRead(A2), 136, 462, 779, true );
        radioData.roll     = mapJoystickValues( analogRead(A1), 109, 460, 848, true );
    }
    else 
    {
        radioData.throttle = mapJoystickValues( analogRead(A0), 180, 497, 807, false );
        radioData.yaw      = mapJoystickValues( analogRead(A1), 109, 460, 848, false );
        radioData.pitch    = mapJoystickValues( analogRead(A2), 136, 462, 779, false );
        radioData.roll     = mapJoystickValues( analogRead(A3), 165, 564, 906, true );
    }
}
void readSwitchData()
{
    radioData.switch_1 = radioData.switch_2 = 0;

    //SW1 and 2
    switch(checkAnalogSwitch(analogRead(SW1)))
    {
        case -1: radioData.switch_1 |= 0b00000001; break;
        case 0 : radioData.switch_1 |= 0b00000010; break;
        case 1 : radioData.switch_1 |= 0b00000100; break;
    }
    switch(checkAnalogSwitch(analogRead(SW2)))
    {
        case -1: radioData.switch_1 |= 0b00001000; break;
        case 0 : radioData.switch_1 |= 0b00010000; break;
        case 1 : radioData.switch_1 |= 0b00100000; break;
    }
    //SW3 and 4
    switch(checkDigitalSwitch(digitalRead(SW3_L), digitalRead(SW3_H)))
    {
        case -1: radioData.switch_2 |= 0b00000001; break;
        case 0 : radioData.switch_2 |= 0b00000010; break;
        case 1 : radioData.switch_2 |= 0b00000100; break;
    }
    switch(checkDigitalSwitch(digitalRead(SW4_L), digitalRead(SW4_H)))
    {
        case -1: radioData.switch_2 |= 0b00001000; break;
        case 0 : radioData.switch_2 |= 0b00010000; break;
        case 1 : radioData.switch_2 |= 0b00100000; break;
    }


    if(digitalRead(BUTT_L) == LOW) radioData.switch_1 |= 0b10000000;
    if(digitalRead(BUTT_R) == LOW) radioData.switch_2 |= 0b10000000;
}


//Data Transceiver
void DataTrans()
{

    radio.write(&radioData, sizeof(RadioData));

    #ifdef __AUTO_ACK___
        while ( radio.isAckPayloadAvailable()) 
        {
            radio.read(&ackPayload, sizeof(AckPayload));
            ppsCounter++;
        }
        unsigned long now = millis();
        if ( now - lastPPS > 1000 ) 
        {
            pps = ppsCounter;
            ppsCounter = 0;
            lastPPS = now;
        }
    #endif
}
