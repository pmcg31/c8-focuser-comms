#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "msgid.hpp"
#include "HostComms.hpp"
#include "HostCommsListener.hpp"
#include "CommsWriter.hpp"

#define CALL_MEMBER_FN(object, ptrToMember) ((object).*(ptrToMember))

namespace ELS
{

    static char __msg[100];
    static const int __maxComponents = 3;

    HCDecodeMethodPtr HostComms::g_decodersById[] = {
        &HostComms::decodeMovingRelMessage,
        &HostComms::decodeMovingAbsMessage,
        &HostComms::decodeStoppedMessage,
        &HostComms::decodeMotorEnabledMessage,
        &HostComms::decodeZeroedMessage,
        &HostComms::decodePositionMessage,
        &HostComms::decodeMicrostepsMessage,
        &HostComms::decodeMaxPosMessage,
        &HostComms::decodeSpeedMessage,
        &HostComms::decodeBacklashEnabledMessage,
        &HostComms::decodeBacklashStepsMessage};

    HostComms::HostComms(CommsWriter *writer,
                         HostCommsListener *listener)
        : _writer(writer),
          _listener(listener)
    {
    }

    HostComms::~HostComms()
    {
        close();
    }

    void HostComms::close()
    {
        if (_writer != 0)
        {
            _writer->close();
        }
    }

    void HostComms::processLine(char *line)
    {
        char *saveptr;
        const char *components[__maxComponents + 1];
        int compCnt;
        char *str = line;

        for (compCnt = 0; compCnt < (__maxComponents + 1); compCnt++)
        {
            components[compCnt] = strtok_r(str, " ", &saveptr);
            str = NULL;

            if (components[compCnt] == NULL)
            {
                break;
            }
        }

        int msgId = atoi(components[0]);

        CALL_MEMBER_FN(*this, g_decodersById[msgId - 1])
        (compCnt, components);
    }

    bool HostComms::focusRel(FocusDirection dir, uint32_t steps)
    {
        sprintf(__msg, "%02d %d %u",
                HFMI_FOCUS_REL,
                dir, steps);

        return _writer->writeLine(__msg);
    }

    bool HostComms::focusAbs(uint32_t position)
    {
        sprintf(__msg, "%02d %u",
                HFMI_FOCUS_ABS,
                position);

        return _writer->writeLine(__msg);
    }

    bool HostComms::focusAbort()
    {
        sprintf(__msg, "%02d",
                HFMI_FOCUS_ABORT);

        return _writer->writeLine(__msg);
    }

    bool HostComms::enableMotor(bool isEnabled)
    {
        sprintf(__msg, "%02d %d",
                HFMI_ENABLE_MOTOR,
                isEnabled ? 1 : 0);

        return _writer->writeLine(__msg);
    }

    bool HostComms::zero()
    {
        sprintf(__msg, "%02d",
                HFMI_ZERO);

        return _writer->writeLine(__msg);
    }

    bool HostComms::getPos()
    {
        sprintf(__msg, "%02d",
                HFMI_GET_POS);

        return _writer->writeLine(__msg);
    }

    bool HostComms::setMicrostep(Microsteps ms)
    {
        sprintf(__msg, "%02d %d",
                HFMI_SET_MICROSTEP,
                ms);

        return _writer->writeLine(__msg);
    }

    bool HostComms::getMicrostep()
    {
        sprintf(__msg, "%02d",
                HFMI_GET_MICROSTEP);

        return _writer->writeLine(__msg);
    }

    bool HostComms::getMotorEnabled()
    {
        sprintf(__msg, "%02d",
                HFMI_GET_MOTOR_ENABLED);

        return _writer->writeLine(__msg);
    }

    bool HostComms::getMotorMoving()
    {
        sprintf(__msg, "%02d",
                HFMI_GET_MOTOR_MOVING);

        return _writer->writeLine(__msg);
    }

    bool HostComms::getMaxPos()
    {
        sprintf(__msg, "%02d",
                HFMI_GET_MAX_POS);

        return _writer->writeLine(__msg);
    }

    bool HostComms::setSpeed(FocusSpeed speed)
    {
        sprintf(__msg, "%02d %d",
                HFMI_SET_SPEED,
                speed);

        return _writer->writeLine(__msg);
    }

    bool HostComms::getSpeed()
    {
        sprintf(__msg, "%02d",
                HFMI_GET_SPEED);

        return _writer->writeLine(__msg);
    }

    bool HostComms::enableBacklash(bool isEnabled)
    {
        sprintf(__msg, "%02d %d",
                HFMI_ENABLE_BACKLASH,
                isEnabled ? 1 : 0);

        return _writer->writeLine(__msg);
    }

    bool HostComms::getBacklashEnabled()
    {
        sprintf(__msg, "%02d",
                HFMI_GET_BACKLASH_ENABLED);

        return _writer->writeLine(__msg);
    }

    bool HostComms::setBacklashSteps(uint32_t steps)
    {
        sprintf(__msg, "%02d %u",
                HFMI_SET_BACKLASH_STEPS,
                steps);

        return _writer->writeLine(__msg);
    }

    bool HostComms::getBacklashSteps()
    {
        sprintf(__msg, "%02d",
                HFMI_GET_BACKLASH_STEPS);

        return _writer->writeLine(__msg);
    }

    bool HostComms::decodeMovingRelMessage(int compCnt,
                                           const char **components)
    {
        // MOVING_REL <dir-enum> <amount>
        if (compCnt != 3)
        {
            return false;
        }

        int dirEnumIdx = atoi(components[1]);
        uint32_t amount = strtoul(components[2], 0, 10);

        if ((dirEnumIdx == 0) || (amount == 0))
        {
            return false;
        }

        _listener->movingRel((FocusDirection)dirEnumIdx, amount);

        return true;
    }

    bool HostComms::decodeMovingAbsMessage(int compCnt,
                                           const char **components)
    {
        // MOVING_ABS <from-position> <to-position>
        if (compCnt != 3)
        {
            return false;
        }

        char *endptr = 0;
        uint32_t fromPosition = strtoul(components[1], &endptr, 10);
        if (endptr == components[1])
        {
            return false;
        }

        uint32_t toPosition = strtoul(components[2], &endptr, 10);
        if (endptr == components[2])
        {
            return false;
        }

        _listener->movingAbs(fromPosition, toPosition);

        return true;
    }

    bool HostComms::decodeStoppedMessage(int compCnt,
                                         const char **components)
    {
        // STOPPED <position>
        if (compCnt != 2)
        {
            return false;
        }

        char *endptr = 0;
        uint32_t position = strtoul(components[1], &endptr, 10);

        if (endptr == components[1])
        {
            return false;
        }

        _listener->stopped(position);

        return true;
    }

    bool HostComms::decodeMotorEnabledMessage(int compCnt,
                                              const char **components)
    {
        // MOTOR_ENABLED <isEnabled>
        if (compCnt != 2)
        {
            return false;
        }

        char *endptr = 0;
        uint32_t isEnabled = strtoul(components[1], &endptr, 10);

        if (endptr == components[1])
        {
            return false;
        }

        _listener->motorEnabled(isEnabled == 1);

        return true;
    }

    bool HostComms::decodeZeroedMessage(int compCnt,
                                        const char ** /* components */)
    {
        // ZEROED
        if (compCnt != 1)
        {
            return false;
        }

        _listener->zeroed();

        return true;
    }

    bool HostComms::decodePositionMessage(int compCnt,
                                          const char **components)
    {
        // POSITION <position>
        if (compCnt != 2)
        {
            return false;
        }

        char *endptr = 0;
        uint32_t position = strtoul(components[1], &endptr, 10);

        if (endptr == components[1])
        {
            return false;
        }

        _listener->position(position);

        return true;
    }

    bool HostComms::decodeMicrostepsMessage(int compCnt,
                                            const char **components)
    {
        // MICROSTEPS <ms-enum>
        if (compCnt != 2)
        {
            return false;
        }

        int msEnumIdx = atoi(components[1]);

        if (msEnumIdx == 0)
        {
            return false;
        }

        _listener->microsteps((Microsteps)msEnumIdx);

        return true;
    }

    bool HostComms::decodeMaxPosMessage(int compCnt,
                                        const char **components)
    {
        // MAX_POS <position>
        if (compCnt != 2)
        {
            return false;
        }

        char *endptr = 0;
        uint32_t position = strtoul(components[1], &endptr, 10);

        if (endptr == components[1])
        {
            return false;
        }

        _listener->maxPos(position);

        return true;
    }

    bool HostComms::decodeSpeedMessage(int compCnt,
                                       const char **components)
    {
        // SPEED <speed-enum>
        if (compCnt != 2)
        {
            return false;
        }

        int speedEnumIdx = atoi(components[1]);

        if (speedEnumIdx == 0)
        {
            return false;
        }

        _listener->speed((FocusSpeed)speedEnumIdx);

        return true;
    }

    bool HostComms::decodeBacklashEnabledMessage(int compCnt,
                                                 const char **components)
    {
        // BACKLASH_ENABLED <isEnabled>
        if (compCnt != 2)
        {
            return false;
        }

        char *endptr = 0;
        uint32_t isEnabled = strtoul(components[1], &endptr, 10);

        if (endptr == components[1])
        {
            return false;
        }

        _listener->backlashEnabled(isEnabled == 1);

        return true;
    }

    bool HostComms::decodeBacklashStepsMessage(int compCnt,
                                               const char **components)
    {
        // BACKLASH_STEPS <steps>
        if (compCnt != 2)
        {
            return false;
        }

        char *endptr = 0;
        uint32_t steps = strtoul(components[1], &endptr, 10);

        if (endptr == components[1])
        {
            return false;
        }

        _listener->backlashSteps(steps);

        return true;
    }

}