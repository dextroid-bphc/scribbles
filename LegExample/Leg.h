#include <Servo.h>

//#define DEBUG 1
#ifdef DEBUG
 #include <HardwareSerial.h>
#endif

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include "pins_arduino.h"
  #include "WConstants.h"
#endif

class Leg
{
private:
    // we need to store data about every servo - the pin, the position, the step direction, and the transition time
    uint8_t _hipPin, _kneePin, _anklePin;
    uint16_t _hip, _knee, _ankle;
    // 0 = increment; 1 = decrement
    bool _hipd, _kneed, _ankled;
    // the time is actually the delay that needs to be elapsed after every update
    uint32_t _hipTime = 2, _kneeTime = 2, _ankleTime = 2;
    // we'll also need variables for the next position
    uint16_t _hipn, _kneen, _anklen;
    // and for updating the servos
    uint32_t prevHipUpdate = 0, prevKneeUpdate = 0, prevAnkleUpdate = 0;
    // and for storing the transition time
    uint32_t _transTime = 0;
    // a beautiful variable this is
    bool _precision = false;
    // we'll just create servo objects for now and initialise it later
    Servo Hip;
    Servo Knee;
    Servo Ankle;

public:

    Leg(uint8_t hipPin, uint8_t kneePin, uint8_t anklePin)
    {
        // default constructor with basic pin initialisations 
        _hipPin = hipPin;
        _kneePin = kneePin;
        _anklePin = anklePin;
    }

    /*
    Leg()
    {
        // not being used for now, I have unstructured plans for this
        _hip = hip;
        _knee = knee;
        _ankle = ankle;
    }
    */

    ~Leg(){}

    void initialise(uint8_t hip, uint8_t knee, uint8_t ankle)
    {
        // The initialise is required because the starting position values are needed
        // Also, the starting position might not be (0,0,0)
        // The values of hip, knee and ankle will serve as the default value of a straight leg
        pinMode(_hipPin, OUTPUT);
        pinMode(_anklePin, OUTPUT);
        pinMode(_kneePin, OUTPUT);
        Hip.attach(_hipPin);
        Knee.attach(_kneePin);
        Ankle.attach(_anklePin);
        _hip = hip;
        _knee = knee;
        _ankle = ankle;
        // initialisation should be as fast as possible
        Hip.write(hip);
        Knee.write(knee);
        Ankle.write(ankle);
    }

    void higherPrecision(bool precision)
    {
        // sets the precision from 180 steps to 1000 steps
        // a following call of move() will update the time variables automatically
        if(_precision == precision)
            // nothing to do if previous value is same
            return;
        else
        {
            _precision = precision;
            if(_precision)
            {
                // scale values up
                _hip = map(_hip, 0, 180, 1000, 2000);
                _knee = map(_knee, 0, 180, 1000, 2000);
                _ankle = map(_ankle, 0, 180, 1000, 2000);
                _hipn = map(_hip, 0, 180, 1000, 2000);
                _kneen = map(_knee, 0, 180, 1000, 2000);
                _anklen = map(_ankle, 0, 180, 1000, 2000);
            }
            else
            {
                // scale values down
                _hip = map(_hip, 1000, 2000, 0, 180);
                _knee = map(_knee, 1000, 2000, 0, 180);
                _ankle = map(_ankle, 1000, 2000, 0, 180);
                _hipn = map(_hip, 1000, 2000, 0, 180);
                _kneen = map(_knee, 1000, 2000, 0, 180);
                _anklen = map(_ankle, 1000, 2000, 0, 180);   
            }
        }
        #ifdef DEBUG
            Serial.print("Precision: ");
            Serial.println(_precision);
        #endif
    }

    void setTransTime(uint32_t transTime)
    {
        // Well... sets the transTime

        // transTime is actually the time you want to take for the complete movement
        // transTime can be 0
        // higher transTime ==> slower motion        
        _transTime = transTime;
        #ifdef DEBUG
            Serial.print("SetTransTime: ");
            Serial.println(_transTime);
        #endif
    }

    void move(uint8_t hipn, uint8_t kneen, uint8_t anklen)
    {
        // this function creates NO motion, just updates variables

        // precautions
        _hipn = constrain(hipn, 0, 180);
        _kneen = constrain(kneen, 0, 180);
        _anklen = constrain(anklen, 0, 180);
        
        if(_precision)
        {
            // if precise mode is active
            _hipn = map(hipn, 0, 180, 1000, 2000);
            _kneen = map(kneen, 0, 180, 1000, 2000);
            _anklen = map(anklen, 0, 180, 1000, 2000);
        }

        // determine servo step direction
        _hipd = (_hipn > _hip) ? false : true;
        _kneed = (_kneen > _knee) ? false : true;
        _ankled = (_anklen > _ankle) ? false : true;
        
        if(_transTime)
        {
            // direction 0 ==> new target > current pos
            if(_hip != _hipn)
                _hipTime = _transTime/((_hipd)?(_hip - _hipn):(_hipn - _hip));
            else
                _hipTime = 0;
            if(_knee != _kneen)
                _kneeTime = _transTime/((_kneed)?(_knee - _kneen):(_kneen - _knee));
            else
                _kneeTime = 0;
            if(_ankle != _anklen)
                _ankleTime = _transTime/((_ankled)?(_ankle - _anklen):(_anklen - _ankle));
            else
                _ankleTime = 0;
        }
        else
        {
            // transTime is 0
            _hipTime = 0;
            _kneeTime = 0;
            _ankleTime = 0;   
        }
        #ifdef DEBUG
            Serial.print(F("Diff : "));
            Serial.print(_hipn - _hip);
            Serial.print(F("\t"));
            Serial.print(_kneen - _knee);
            Serial.print(F("\t"));
            Serial.println(_anklen - _ankle);
            Serial.print(F("Time : "));
            Serial.print(_hipTime);
            Serial.print(F("\t"));
            Serial.print(_kneeTime);
            Serial.print(F("\t"));
            Serial.println(_ankleTime);
        #endif
    }

    void move(uint8_t hipn, uint8_t kneen, uint8_t anklen, uint32_t transTime)
    {
        // this function creates NO motion, just updates variables
        
        // precautions
        _hipn = constrain(hipn, 0, 180);
        _kneen = constrain(kneen, 0, 180);
        _anklen = constrain(anklen, 0, 180);

        if(_precision)
        {
            // if precise mode is active
            _hipn = map(hipn, 0, 180, 1000, 2000);
            _kneen = map(kneen, 0, 180, 1000, 2000);
            _anklen = map(anklen, 0, 180, 1000, 2000);
        }

        _hipd = (_hipn > _hip) ? false : true;
        _kneed = (_kneen > _knee) ? false : true;
        _ankled = (_anklen > _ankle) ? false : true;

        if(transTime)
        {
            // direction 0 ==> new target > current pos

            // also, there can be a case where target == current
            // we need to set the delay times to 0 explicitly otherwise the
            // delay time counter will max out and create a huge delay
            // because no divide by zero will be thrown

            if(_hip != _hipn)
                _hipTime = transTime/((_hipd)?(_hip - _hipn):(_hipn - _hip));
            else
                _hipTime = 0;
            if(_knee != _kneen)
                _kneeTime = transTime/((_kneed)?(_knee - _kneen):(_kneen - _knee));
            else
                _kneeTime = 0;
            if(_ankle != _anklen)
                _ankleTime = transTime/((_ankled)?(_ankle - _anklen):(_anklen - _ankle));
            else
                _ankleTime = 0;
        }
        else
        {
            // transTime is 0
            _hipTime = 0;
            _kneeTime = 0;
            _ankleTime = 0;   
        }
        #ifdef DEBUG
            Serial.print("TempTransTime: ");
            Serial.println(transTime);
            Serial.print(F("Diff2: "));
            Serial.print(_hipn - _hip);
            Serial.print(F("\t"));
            Serial.print(_kneen - _knee);
            Serial.print(F("\t"));
            Serial.println(_anklen - _ankle);
            Serial.print(F("Time2: "));
            Serial.print(_hipTime);
            Serial.print(F("\t"));
            Serial.print(_kneeTime);
            Serial.print(F("\t"));
            Serial.println(_ankleTime);
        #endif
    }

    bool update()
    {
        // this function needs to be called repeatedly to update ONE STEP of each individual servo

        // return true if the movement is complete
        if(_hipn == _hip && _kneen == _knee && _anklen == _ankle)
            return true;

        // let's not call millis() repeatedly and use one data per call
        uint32_t timeNow = millis();
        
        if(timeNow - prevHipUpdate > _hipTime && _hip != _hipn)
        {
            _hip = (_hipd) ? _hip - 1 : _hip + 1;
            prevHipUpdate = timeNow;
        }
        if(timeNow - prevKneeUpdate > _kneeTime && _knee != _kneen)
        {
            _knee = (_kneed) ? _knee - 1 : _knee + 1;
            prevKneeUpdate = timeNow;
        }
        if(timeNow - prevAnkleUpdate > _ankleTime && _ankle != _anklen)
        {
            _ankle = (_ankled) ? _ankle - 1 : _ankle + 1;
            prevAnkleUpdate = timeNow;
        }

        if(_precision)
        {
            // use a different method to write values
            Hip.writeMicroseconds(_hip);
            Knee.writeMicroseconds(_knee);
            Ankle.writeMicroseconds(_ankle);
        }
        else
        {
            Hip.write(_hip);
            Knee.write(_knee);
            Ankle.write(_ankle);
        }

        #ifdef DEBUG
            Serial.print(_hip);
            Serial.print(F("\t"));
            Serial.print(_knee);
            Serial.print(F("\t"));
            Serial.println(_ankle);
        #endif

        return false;
    }
};