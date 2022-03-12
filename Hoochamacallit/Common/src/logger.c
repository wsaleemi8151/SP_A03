
#include <stdio.h>
#include "../inc/logger.h"

void LogMessage(enum AppTypesEnum loggerType, char *message)
{
    printf("%s", message);
    switch (loggerType)
    {
    case data_creator:
        /* code */
        break;

    case data_reader:
        /* code */
        break;

    case data_corruptor:
        /* code */
        break;
    }
}