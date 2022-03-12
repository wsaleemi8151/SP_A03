
#include "../inc/message_queues.h"

char *GetMessageString(long status)
{
    switch (status)
    {
    case EVERYTHING_OKAY_ON_START:
        return "Everything is OKAY";
        break;

    case EVERYTHING_OKAY:
        return "Everything is OKAY";
        break;

    case HYDRAULIC_PRESSURE_FAILURE:
        return "Hydraulic Pressure Failure";
        break;

    case SAFETY_BUTTON_FAILURE:
        return "Safety Button Failure";
        break;

    case NO_RAW_MATERIAL_IN_PROCESS:
        return "No Raw Material in the Process";
        break;

    case OPERATING_TEMPERATURE_OUT_OF_RANGE:
        return "Operating Temperature Out of Range";
        break;

    case OPERATING_ERROR:
        return "Operator Error";
        break;

    case MACHINE_OFFLINE:
        return "Machine is Off-line";
        break;
    }
}
