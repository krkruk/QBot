#ifndef ORION_H_
#define ORION_H_

#include "Arduino.h"
#include <ArduinoJson.h>
#include "Tools.h"


template<class _Motor, char ID, uint16_t BUFFER = 256>
class Orion
{
    constexpr static unsigned long FEEDBACK_TIMEOUT_MS = 5000;
    _Motor motor;
    StaticJsonBuffer<BUFFER> jsonBuffer;
    unsigned long timeIt = millis();
public:
    Orion()
    {
    }

    void init()
    {
        Serial.begin(115200);
        while(!Serial);

        // kind of lazy initialization I think...
        motor = _Motor();
    }

    void run()
    {
        if(Serial.available())
        {
          JsonObject &root = parse_cmd();
          apply_cmd(root);
        }

        SregLock lock;
        unsigned long now = millis();
        unsigned long timeDiff = now - timeIt;
        if(timeDiff >= FEEDBACK_TIMEOUT_MS)
        {
          print_feedback();
          timeIt = millis();
        }
        unused(lock);
    }

    _Motor &getMotor()
    {
        return motor;
    }

private:
    JsonObject &parse_cmd()
    {
        char buffer[BUFFER];
        memset(buffer, '\0', BUFFER);
        Serial.readBytesUntil('\n', buffer, BUFFER);
        jsonBuffer.clear();
        JsonObject &root = jsonBuffer.parseObject(buffer);
        if(!root.success())
        {
            Serial.println("{\"ERR\":\"CMD NOT PARSED\"}");
        }
        return root;
    }

    void apply_cmd(JsonObject &root)
    {
        if (root.success())
        {
            int16_t speed = root[String(ID)]["PWM"];
            motor.setSpeed(speed);
        }
    }

    void print_feedback()
    {
        jsonBuffer.clear();
        const int16_t speed = motor.getSpeed();
        const uint16_t current = motor.getMilliamps();
        JsonObject &root = jsonBuffer.createObject();
        JsonObject &feedback = root.createNestedObject(String(ID));
        feedback["ROT"] = speed;
        feedback["CUR"] = current;
        feedback["ERR"] = motor.STATE.error();
        root.printTo(Serial);
        Serial.println();
    }
};


#endif /* ORION_H_ */
