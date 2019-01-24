//Data reading
void readJoystickData()
{
    boolean mode1 = 0;
  
    // The calibration numbers used here should be measured 
    // for your joysticks using the TestJoysticks sketch.
    if ( mode1 ) 
    {
        radioData.throttle = mapJoystickValues(dbThrot = analogRead(A0), THROTTLE_RANGE, true );
        radioData.yaw      = mapJoystickValues(dbYaw   = analogRead(A3), YAW_RANGE, false );
        radioData.pitch    = mapJoystickValues(dbPitch = analogRead(A2), PITCH_RANGE, true );
        radioData.roll     = mapJoystickValues(dbRoll  = analogRead(A1), ROLL_RANGE, true );
    }
    else 
    {
        radioData.throttle = mapJoystickValues(dbThrot = analogRead(A0), THROTTLE_RANGE, false );
        radioData.yaw      = mapJoystickValues(dbYaw   = analogRead(A1), YAW_RANGE, false );
        radioData.pitch    = mapJoystickValues(dbPitch = analogRead(A2), PITCH_RANGE, false );
        radioData.roll     = mapJoystickValues(dbRoll  = analogRead(A3), ROLL_RANGE, true );
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


//Debug
void debugData()
{
    int yaw    = map(radioData.yaw, 0, 255, 1000, 2000);
	int throt  = map(radioData.throttle, 0, 255, 1000, 2000);
	int roll   = map(radioData.roll, 0, 255, 1000, 2000);
	int pitch  = map(radioData.pitch, 0, 255, 1000, 2000);


    Serial.print("Sensor|Trans|Microsec: \t");
    Serial.print("Throt: "); Serial.print(dbThrot); Serial.print("|"); Serial.print(radioData.throttle);  Serial.print("|"); Serial.print(throt);   Serial.print("\t");
    Serial.print("Yaw: "); Serial.print(dbYaw);     Serial.print("|"); Serial.print(radioData.yaw);       Serial.print("|"); Serial.print(yaw);     Serial.print("\t");
    Serial.print("Roll: "); Serial.print(dbRoll);   Serial.print("|"); Serial.print(radioData.roll);      Serial.print("|"); Serial.print(roll);    Serial.print("\t");
    Serial.print("Pitch: "); Serial.print(dbPitch); Serial.print("|"); Serial.print(radioData.pitch);     Serial.print("|"); Serial.print(pitch);   Serial.print("\t");
    Serial.println(";");

}