
enum AppTypesEnum
{
    data_creator,
    data_reader,
    data_corruptor
};

void LogMessage(enum AppTypesEnum loggerType, char *message);
int WriteLogToFile(char *fileName, char *message);


#define BUF_LEN 256