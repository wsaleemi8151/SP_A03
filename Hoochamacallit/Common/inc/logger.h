
enum LoggerTypesEnum
{
    data_creator,
    data_reader,
    data_corruptor
};

void LogMessage(enum LoggerTypesEnum loggerType, char* message);
