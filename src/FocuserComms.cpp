#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "msgid.hpp"
#include "FocuserComms.hpp"
#include "FocuserCommsListener.hpp"
#include "CommsWriter.hpp"

#define CALL_MEMBER_FN(object, ptrToMember) ((object).*(ptrToMember))

namespace ELS
{

    static char __msg[100];
    static const int __maxComponents = 3;

    FCDecodeMethodPtr FocuserComms::g_decodersById[] = {
        &FocuserComms::decodeFocusRelMessage,
        &FocuserComms::decodeFocusAbsMessage,
        &FocuserComms::decodeFocusAbortMessage,
        &FocuserComms::decodeEnableMotorMessage,
        &FocuserComms::decodeZeroMessage,
        &FocuserComms::decodeGetPosMessage,
        &FocuserComms::decodeSetMicrostepMessage,
        &FocuserComms::decodeGetMicrostepMessage,
        &FocuserComms::decodeGetMotorEnabledMessage,
        &FocuserComms::decodeGetMotorMovingMessage,
        &FocuserComms::decodeGetMaxPosMessage,
        &FocuserComms::decodeSetSpeedMessage,
        &FocuserComms::decodeGetSpeedMessage};

    FocuserComms::FocuserComms(CommsWriter *writer,
                               FocuserCommsListener *listener)
        : _writer(writer),
          _listener(listener)
    {
    }

    FocuserComms::~FocuserComms()
    {
        close();
    }

    void FocuserComms::close()
    {
        if (_writer != 0)
        {
            _writer->close();
        }
    }

    void FocuserComms::processLine(char *line)
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

        if (compCnt > 0)
        {
            int msgId = atoi(components[0]);

            dispatch(msgId, compCnt, components);
        }
    }

    void FocuserComms::dispatch(int msgId, int compCnt, const char *components[])
    {
        CALL_MEMBER_FN(*this, g_decodersById[msgId - 1])
        (compCnt, components);
    }

    bool FocuserComms::movingRel(FocusDirection dir, uint32_t steps)
    {
        sprintf(__msg, "%02d %d %u",
                FHMI_MOVING_REL,
                dir, steps);

        return _writer->writeLine(__msg);
    }

    bool FocuserComms::movingAbs(uint32_t oldPosition, uint32_t newPosition)
    {
        sprintf(__msg, "%02d %u %u",
                FHMI_MOVING_ABS,
                oldPosition, newPosition);

        return _writer->writeLine(__msg);
    }

    bool FocuserComms::stopped(uint32_t position)
    {
        sprintf(__msg, "%02d %u",
                FHMI_STOPPED,
                position);

        return _writer->writeLine(__msg);
    }

    bool FocuserComms::motorEnabled(bool isEnabled)
    {
        sprintf(__msg, "%02d %d",
                FHMI_MOTOR_ENABLED,
                isEnabled ? 1 : 0);

        return _writer->writeLine(__msg);
    }

    bool FocuserComms::zeroed()
    {
        sprintf(__msg, "%02d",
                FHMI_ZEROED);

        return _writer->writeLine(__msg);
    }

    bool FocuserComms::position(uint32_t position)
    {
        sprintf(__msg, "%02d %u",
                FHMI_POSITION,
                position);

        return _writer->writeLine(__msg);
    }

    bool FocuserComms::microsteps(Microsteps ms)
    {
        sprintf(__msg, "%02d %d",
                FHMI_MICROSTEPS,
                ms);

        return _writer->writeLine(__msg);
    }

    bool FocuserComms::maxPos(uint32_t position)
    {
        sprintf(__msg, "%02d %u",
                FHMI_MAX_POS,
                position);

        return _writer->writeLine(__msg);
    }

    bool FocuserComms::speed(FocusSpeed speed)
    {
        sprintf(__msg, "%02d %d",
                FHMI_SPEED,
                speed);

        return _writer->writeLine(__msg);
    }

    bool FocuserComms::decodeFocusRelMessage(int compCnt,
                                             const char **components)
    {
        // FOCUS_REL <dir-enum> <amount>
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

        _listener->focusRel((FocusDirection)dirEnumIdx, amount);

        return true;
    }

    bool FocuserComms::decodeFocusAbsMessage(int compCnt,
                                             const char **components)
    {
        // FOCUS_ABS <position>
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

        _listener->focusAbs(position);

        return true;
    }

    bool FocuserComms::decodeFocusAbortMessage(int compCnt,
                                               const char ** /* components */)
    {
        // FOCUS_ABORT
        if (compCnt != 1)
        {
            return false;
        }

        _listener->focusAbort();

        return true;
    }

    bool FocuserComms::decodeEnableMotorMessage(int compCnt,
                                                const char **components)
    {
        // ENABLE_MOTOR isEnabled
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

        _listener->enableMotor(isEnabled == 1);

        return true;
    }

    bool FocuserComms::decodeZeroMessage(int compCnt,
                                         const char ** /* components */)
    {
        // ZERO
        if (compCnt != 1)
        {
            return false;
        }

        _listener->zero();

        return true;
    }

    bool FocuserComms::decodeGetPosMessage(int compCnt,
                                           const char ** /* components */)
    {
        // GET_POS
        if (compCnt != 1)
        {
            return false;
        }

        _listener->getPos();

        return true;
    }

    bool FocuserComms::decodeSetMicrostepMessage(int compCnt,
                                                 const char **components)
    {
        // SET_MICROSTEP <ms-enum>
        if (compCnt != 2)
        {
            return false;
        }

        int msEnumIdx = atoi(components[1]);

        if (msEnumIdx == 0)
        {
            return false;
        }

        _listener->setMicrostep((Microsteps)msEnumIdx);

        return true;
    }

    bool FocuserComms::decodeGetMicrostepMessage(int compCnt,
                                                 const char ** /* components */)
    {
        // GET_MICROSTEP
        if (compCnt != 1)
        {
            return false;
        }

        _listener->getMicrostep();

        return true;
    }

    bool FocuserComms::decodeGetMotorEnabledMessage(int compCnt,
                                                    const char ** /* components */)
    {
        // GET_MOTOR_ENABLED
        if (compCnt != 1)
        {
            return false;
        }

        _listener->getMotorEnabled();

        return true;
    }

    bool FocuserComms::decodeGetMotorMovingMessage(int compCnt,
                                                   const char ** /* components */)
    {
        // GET_MOTOR_MOVING
        if (compCnt != 1)
        {
            return false;
        }

        _listener->getMotorMoving();

        return true;
    }

    bool FocuserComms::decodeGetMaxPosMessage(int compCnt,
                                              const char ** /* components */)
    {
        // GET_MAX_POS
        if (compCnt != 1)
        {
            return false;
        }

        _listener->getMaxPos();

        return true;
    }

    bool FocuserComms::decodeSetSpeedMessage(int compCnt,
                                             const char **components)
    {
        // SET_SPEED <speed-enum>
        if (compCnt != 2)
        {
            return false;
        }

        int speedEnumIdx = atoi(components[1]);

        if (speedEnumIdx == 0)
        {
            return false;
        }

        _listener->setSpeed((FocusSpeed)speedEnumIdx);

        return true;
    }

    bool FocuserComms::decodeGetSpeedMessage(int compCnt,
                                             const char ** /* components */)
    {
        // GET_SPEED
        if (compCnt != 1)
        {
            return false;
        }

        _listener->getSpeed();

        return true;
    }

}