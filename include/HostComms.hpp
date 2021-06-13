#pragma once

#include <inttypes.h>
#include "msgid.hpp"

namespace ELS
{

    class CommsWriter;
    class HostCommsListener;

    class HostComms;
    typedef bool (HostComms::*HCDecodeMethodPtr)(int compCnt,
                                                 const char **components);

    class HostComms
    {
    public:
        HostComms(CommsWriter *writer,
                  HostCommsListener *listener);
        ~HostComms();

    public:
        void close();

        void processLine(char *line);

        bool focusRel(FocusDirection dir, uint32_t steps);
        bool focusAbs(uint32_t position);
        bool focusAbort();
        bool enableMotor(bool isEnabled);
        bool zero();
        bool getPos();
        bool setMicrostep(Microsteps ms);
        bool getMicrostep();
        bool getMotorEnabled();
        bool getMotorMoving();
        bool getMaxPos();
        bool setSpeed(FocusSpeed speed);
        bool getSpeed();
        bool enableBacklash(bool isEnabled);
        bool getBacklashEnabled();
        bool setBacklashSteps(uint32_t steps);
        bool getBacklashSteps();

    private:
        bool decodeMovingRelMessage(int compCnt, const char **components);
        bool decodeMovingAbsMessage(int compCnt, const char **components);
        bool decodeStoppedMessage(int compCnt, const char **components);
        bool decodeMotorEnabledMessage(int compCnt, const char **components);
        bool decodeZeroedMessage(int compCnt, const char **components);
        bool decodePositionMessage(int compCnt, const char **components);
        bool decodeMicrostepsMessage(int compCnt, const char **components);
        bool decodeMaxPosMessage(int compCnt, const char **components);
        bool decodeSpeedMessage(int compCnt, const char **components);
        bool decodeBacklashEnabledMessage(int compCnt, const char **components);
        bool decodeBacklashStepsMessage(int compCnt, const char **components);

    private:
        CommsWriter *_writer;
        HostCommsListener *_listener;

    private:
        static HCDecodeMethodPtr g_decodersById[];
    };

}