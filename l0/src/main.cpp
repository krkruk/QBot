#include <Arduino.h>
#include "MC33926.h"
#include "Orion.h"

namespace
{
    constexpr char ID = '0';               // ID of the device
    constexpr uint8_t DIRECTION_PIN = 7;
    constexpr uint8_t CURRENT_FEEDBACK = A7;
}

using Motor = drv::MC33926
<           // mapping matches Arduino NANO pinout
            // PWM pin: D6, (AVR: PD7)
        DIRECTION_PIN,
        CURRENT_FEEDBACK
        ,drv::MC33926_Enable<0, 8>
>;

namespace
{
    Orion<Motor, ID> orion;
}

void setup()
{
    orion.init();
}

void loop()
{
    orion.run();
}