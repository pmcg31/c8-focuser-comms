#pragma once

#include <inttypes.h>
#include "msgid.hpp"

namespace ELS
{

    class FocuserCommsListener
    {
    public:
        virtual void focusRel(FocusDirection dir, uint32_t steps) = 0;
        virtual void focusAbs(uint32_t position) = 0;
        virtual void focusAbort() = 0;
        virtual void enableMotor(bool isEnabled) = 0;
        virtual void zero() = 0;
        virtual void getPos() = 0;
        virtual void setMicrostep(Microsteps ms) = 0;
        virtual void getMicrostep() = 0;
        virtual void getMotorEnabled() = 0;
        virtual void getMotorMoving() = 0;
        virtual void getMaxPos() = 0;
        virtual void setSpeed(FocusSpeed speed) = 0;
        virtual void getSpeed() = 0;
        virtual void enableBacklash(bool isEnabled) = 0;
        virtual void getBacklashEnabled() = 0;
        virtual void setBacklashSteps(uint32_t steps) = 0;
        virtual void getBacklashSteps() = 0;
    };

}