class Leg
{
private:
    // we need to store data about every servo - the pin, the position, the step direction, and the transition time
    uint8_t _hipPin, _kneePin, _anklePin;
    uint8_t _hip, _knee, _ankle;
    // 0 = increment; 1 = decrement
    bool hipd, kneed, ankled;
    // the time is actually the delay that needs to be elapsed after every update
    uint32_t _hipTime = 2, _kneeTime = 2, _ankleTime = 2;
    // we'll also need variables for the next position
    uint8_t _hipn, _kneen, _anklen;
    // and for updating the servos
    uint32_t prevHipUpdate = 0, prevKneeUpdate = 0, prevAnkleUpdate = 0;
    // and for storing the transition time
    uint32_t _transTime = 0;

public:

    Leg(uint8_t hipPin, uint8_t kneePin, uint8_t anklePin);
    {
        // default constructor with basic pin initialisations 
        _hipPin = hipPin;
        _kneePin = kneePin;
        _anklePin = anklePin;
    }

    Leg(uint8_t hipPin, uint8_t kneePin, uint8_t anklePin, uint16_t transTime);
    {
        // transTime is actually the time you want to take for the complete movement
        // transTime can be 0
        // higher transTime ==> slower motion
        _hipPin = hipPin;
        _kneePin = kneePin;
        _anklePin = anklePin;
        _transTime = transTime;
    }
    
    Leg();
    {
        // not being used for now, I have unstructured plans for this
        _hip = hip;
        _knee = knee;
        _ankle = ankle;
    }

    ~Leg();

    void initialise(uint8_t hip, uint8_t knee, uint8_t ankle)
    {
        // The initialise is required because the starting position values are needed
        // Also, the starting position might not be (0,0,0)
        // The values of hip, knee and ankle will serve as the default value of a straight leg
        pinMode(_hipPin, OUTPUT);
        pinMode(_anklePin, OUTPUT);
        pinMode(_kneePin, OUTPUT);
        _hip = hip;
        _knee = knee;
        _ankle = ankle;
        // initialisation should be as fast as possible
        analogWrite(_hipPin, hip);
        analogWrite(_kneePin, knee);
        analogWrite(_anklePin, ankle);
    }

    void move(uint8_t hipn, uint8_t kneen, uint8_t anklen)
    {
        // this function creates NO motion, just updates variables
        
        hipd = ((_hipn - _hip) > 0) ? false : true;
        kneed = ((_kneen - _knee) > 0) ? false : true;
        ankled = ((_anklen - _ankle) > 0) ? false : true;
        if(!_transTime)
        {
            _hipTime = transTime/abs(_hipn - _hip);
            _kneeTime = transTime/abs(_kneen - _knee);
            _ankleTime = transTime/abs(_anklen - _ankle);
        }
        else
        {
            _hipTime = 0;
            _kneeTime = 0;
            _ankleTime = 0;   
        }
    }

    void move(uint8_t hipn, uint8_t kneen, uint8_t anklen, uint32_t transTime)
    {
        // this function creates NO motion, just updates variables
        
        hipd = ((_hipn - _hip) > 0) ? false : true;
        kneed = ((_kneen - _knee) > 0) ? false : true;
        ankled = ((_anklen - _ankle) > 0) ? false : true;
        if(!transTime)
        {
            _hipTime = transTime/abs(_hipn - _hip);
            _kneeTime = transTime/abs(_kneen - _knee);
            _ankleTime = transTime/abs(_anklen - _ankle);
        }
        else
        {
            _hipTime = 0;
            _kneeTime = 0;
            _ankleTime = 0;   
        }
    }

    boolean update()
    {
        // this function needs to be called repeatedly to update ONE STEP of each individual servo

        // return true if the movement is complete
        if(_hipn == _hip && _kneen == _knee && _anklen == _ankle)
            return true;

        // let's not call millis() repeatedly and use one data per call
        uint32_t timeNow = millis();
        
        if(timeNow > _hipTime && _hip != _hipn)
        {
            _hip = (hipd) ? _hip + 1 : _hip - 1;
            prevHipUpdate = timeNow;
        }
        if(timeNow > _kneeTime && _knee != _kneen)
        {
            _knee = (kneed) ? _knee + 1 : _knee - 1;
            prevKneeUpdate = timeNow;
        }
        if(timeNow > _ankleTime && _ankle != _anklen)
        {
            _ankle = (ankled) ? _ankle + 1 : _ankle - 1;
            prevAnkleUpdate = timeNow;
        }

        analogWrite(_hipPin, _hip);
        analogWrite(_kneePin, _knee);
        analogWrite(_anklePin, _ankle);

        return false;
    }
};