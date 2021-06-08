#pragma once

#include <inttypes.h>
#include "msgid.hpp"

namespace ELS
{

    class CommsWriter;
    class FocuserCommsListener;

    class FocuserComms;
    typedef bool (FocuserComms::*FCDecodeMethodPtr)(int compCnt,
                                                    const char **components);

    class FocuserComms
    {
    public:
        FocuserComms(CommsWriter *writer,
                     FocuserCommsListener *listener);
        ~FocuserComms();

    public:
        void close();

        void processLine(char *line);

        bool movingRel(FocusDirection dir, uint32_t steps);
        bool movingAbs(uint32_t oldPosition, uint32_t newPosition);
        bool stopped(uint32_t position);
        bool motorEnabled(bool isEnabled);
        bool zeroed();
        bool position(uint32_t position);
        bool microsteps(Microsteps ms);
        bool maxPos(uint32_t position);
        bool speed(FocusSpeed speed);

    private:
        void dispatch(int msgId, int compCnt, const char *components[]);
        bool decodeFocusRelMessage(int compCnt, const char **components);
        bool decodeFocusAbsMessage(int compCnt, const char **components);
        bool decodeFocusAbortMessage(int compCnt, const char **components);
        bool decodeEnableMotorMessage(int compCnt, const char **components);
        bool decodeZeroMessage(int compCnt, const char **components);
        bool decodeGetPosMessage(int compCnt, const char **components);
        bool decodeSetMicrostepMessage(int compCnt, const char **components);
        bool decodeGetMicrostepMessage(int compCnt, const char **components);
        bool decodeGetMotorEnabledMessage(int compCnt, const char **components);
        bool decodeGetMotorMovingMessage(int compCnt, const char **components);
        bool decodeGetMaxPosMessage(int compCnt, const char **components);
        bool decodeSetSpeedMessage(int compCnt, const char **components);
        bool decodeGetSpeedMessage(int compCnt, const char **components);

    private:
        CommsWriter *_writer;
        FocuserCommsListener *_listener;

    private:
        static FCDecodeMethodPtr g_decodersById[];
    };

}