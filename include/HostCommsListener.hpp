#pragma once

#include <inttypes.h>
#include "msgid.hpp"

namespace ELS
{

    class HostCommsListener
    {
    public:
        virtual void movingRel(FocusDirection dir, uint32_t steps) = 0;
        virtual void movingAbs(uint32_t fromPosition, uint32_t toPosition) = 0;
        virtual void stopped(uint32_t position) = 0;
        virtual void motorEnabled(bool isEnabled) = 0;
        virtual void zeroed() = 0;
        virtual void position(uint32_t position) = 0;
        virtual void microsteps(Microsteps ms) = 0;
        virtual void maxPos(uint32_t position) = 0;
        virtual void speed(FocusSpeed speed) = 0;
        virtual void backlashEnabled(bool isEnabled) = 0;
        virtual void backlashSteps(uint32_t steps) = 0;
    };
}