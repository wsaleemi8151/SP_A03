/*
 * FILE : message_queues.c
 * PROJECT : SENG2030-22W-Sec1-System Programming - assignment # 3
 * Group: A03-17
 * PROGRAMMER : Gursharan Singh - Waqar Ali Saleemi
 * FIRST VERSION : 2022-03-08
 * DESCRIPTION :
 * Source code file for shared message queues functionality
 */

#include "../inc/message_queues.h"

char *GetMessageString(long status)
{
    switch (status)
    {
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
