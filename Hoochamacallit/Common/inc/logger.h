/*
 * FILE : logger.h
 * PROJECT : SENG2030-22W-Sec1-System Programming - assignment # 3
 * Group: A03-17
 * PROGRAMMER : Gursharan Singh - Waqar Ali Saleemi
 * FIRST VERSION : 2022-03-08
 * DESCRIPTION :
 * Header file for shared logging machenism
 */

enum AppTypesEnum
{
    data_creator,
    data_reader,
    data_corruptor
};

void LogMessage(enum AppTypesEnum loggerType, char *message);
int WriteLogToFile(char *fileName, char *message);

#define BUF_LEN 256