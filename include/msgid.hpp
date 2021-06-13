#pragma once

namespace ELS
{

    enum HostToFocuserMsgId
    {
        HFMI_FOCUS_REL = 1,
        HFMI_FOCUS_ABS = 2,
        HFMI_FOCUS_ABORT = 3,
        HFMI_ENABLE_MOTOR = 4,
        HFMI_ZERO = 5,
        HFMI_GET_POS = 6,
        HFMI_SET_MICROSTEP = 7,
        HFMI_GET_MICROSTEP = 8,
        HFMI_GET_MOTOR_ENABLED = 9,
        HFMI_GET_MOTOR_MOVING = 10,
        HFMI_GET_MAX_POS = 11,
        HFMI_SET_SPEED = 12,
        HFMI_GET_SPEED = 13,
        HFMI_ENABLE_BACKLASH = 14,
        HFMI_GET_BACKLASH_ENABLED = 15,
        HFMI_SET_BACKLASH_STEPS = 16,
        HFMI_GET_BACKLASH_STEPS = 17
    };

    enum FocuserToHostMsgId
    {
        FHMI_MOVING_REL = 1,
        FHMI_MOVING_ABS = 2,
        FHMI_STOPPED = 3,
        FHMI_MOTOR_ENABLED = 4,
        FHMI_ZEROED = 5,
        FHMI_POSITION = 6,
        FHMI_MICROSTEPS = 7,
        FHMI_MAX_POS = 8,
        FHMI_SPEED = 9,
        FHMI_BACKLASH_ENABLED = 10,
        FHMI_BACKLASH_STEPS = 11
    };

    enum Microsteps
    {
        MS_X8 = 1,
        MS_X16 = 2,
        MS_X32 = 3,
        MS_X64 = 4
    };

    enum FocusDirection
    {
        FD_FOCUS_INWARD = 1,
        FD_FOCUS_OUTWARD = 2
    };

    enum FocusSpeed
    {
        FS_NORMAL = 1,
        FS_X3 = 2
    };

}