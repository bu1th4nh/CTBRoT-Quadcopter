//Data-related utilities
//Returns a corrected value for a joystick position that takes into account
//the values of the outer extents and the middle of the joystick range.
int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse)
{
    val = constrain(val, lower, upper);
    if (val < middle)
        val = map(val, lower, middle, 0, 128);
    else
        val = map(val, middle, upper, 128, 255);
    return (reverse ? 255 - val : val);
}
//Reset data
void resetRadioData() 
{
    radioData.throttle = 127;
    radioData.yaw = 127;
    radioData.pitch = 127;
    radioData.roll = 127;
    radioData.switch_1 = 0;
    radioData.switch_2 = 0;
}
//Data cleanup
void cleanData()
{
    if ( radioData.throttle == 255 ) radioData.throttle = 254;
    if ( radioData.yaw == 255 ) radioData.yaw = 254;
    if ( radioData.pitch == 255 ) radioData.pitch = 254;
    if ( radioData.roll == 255 ) radioData.roll = 254;
    if ( radioData.switch_2 == 255 ) radioData.switch_1 = 254;
    if ( radioData.switch_1 == 255 ) radioData.switch_2 = 254;

    if ( ackPayload.alt == 255 ) ackPayload.alt = 254; // not quite sufficient, but unlikely that any other bytes of the altitude will be 255
    ackPayload.flags &= 0x2; // ok because we are only interested in bit 2 (ARMED)
}
void adjustAckPayloadValues()
{
    ackPayload.pitch *= 0.1;
    ackPayload.roll *= 0.1;
}
void writeDataToSerial() 
{
    cleanData();
    
    /*      Acknowledgement payloads data
    Serial.print("  Pitch "); Serial.print(ackPayload.pitch);
    Serial.print("  Roll ");  Serial.print(ackPayload.roll);
    Serial.print("  Yaw ");   Serial.print(ackPayload.heading);
    Serial.print("  Alt ");   Serial.print(ackPayload.alt);
    Serial.print("  PPS ");   Serial.println(pps);
    Serial.print("  Flags "); Serial.print(ackPayload.flags);
    Serial.write((uint8_t*)&pps, 2);
    */

    //      Debug TX data
    Serial.print("  Pitch "); Serial.print(radioData.pitch);
    Serial.print("  Roll ");  Serial.print(radioData.roll);
    Serial.print("  Yaw ");   Serial.print(radioData.yaw);
    Serial.print("  Throttle ");   Serial.print(radioData.throttle);
    Serial.print("  PPS ");   Serial.println(pps);
    
      
    // Serial.write(serialHeader, 4);
    // Serial.write((uint8_t*)&radioData, sizeof(RadioData));
    // Serial.write((uint8_t*)&ackPayload, sizeof(AckPayload));
}

//Misc. utilities
bool inRange(int lower, int upper, int value)
{
    return lower <= value && value <= upper;
}
int checkDigitalSwitch(int up, int down)
{
    up   = (up   == HIGH) ? 0 : 1;
    down = (down == HIGH) ? 0 : 1;
    return up ? 1 : (down ? -1 : 0);
}
int checkAnalogSwitch(int value)
{
    return inRange(950, 1023, value) ? 1 : (inRange(0, 200, value) ? -1 : 0);
}