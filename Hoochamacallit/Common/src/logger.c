/*
 * FILE : logger.c
 * PROJECT : SENG2030-22W-Sec1-System Programming - assignment # 3
 * Group: A03-17
 * PROGRAMMER : Gursharan Singh - Waqar Ali Saleemi
 * FIRST VERSION : 2022-03-08
 * DESCRIPTION :
 * Source code file for shared logging machenism
 */

#include <stdio.h>
#include "../inc/logger.h"
#include <time.h>

/*
 * FUNCTION : LogMessage
 *
 * DESCRIPTION : This function is used to parse loggin message based on App Type
 *                  then pass this message to another function to write it to log file
 *
 * PARAMETERS : enum AppTypesEnum appType   -   App type on which function decide which file path to be used to write the log
 *              char *message               -   Actual Log message sent by the application
 *
 * Returns : void
 */
void LogMessage(enum AppTypesEnum appType, char *message)
{
    char logMsg[250];
    int writingLogResult = 0;
    char logFileName[200];

    // Time operation Ref:
    // https://en.wikipedia.org/wiki/C_date_and_time_functions#:~:text=The%20C%20date%20and%20time,and%20formatted%20output%20to%20strings.
    // https://zetcode.com/articles/cdatetime/

    time_t current_time;
    // char *c_time_string;
    char buf[BUF_LEN] = {0};

    /* Obtain current time. */
    current_time = time(NULL);

    struct tm *ptm = localtime(&current_time);
    strftime(buf, BUF_LEN, "%Y-%m-%d %H:%M:%S", ptm);

    /* Convert to local time format. */
    // c_time_string = ctime(&current_time);

    // adding timestamp to the message
    sprintf(logMsg, "%s : %s", buf, message);

    switch (appType)
    {
    case data_creator:
        sprintf(logFileName, "/tmp/dataCreator.log");
        break;

    case data_reader:
        sprintf(logFileName, "/tmp/dataMonitor.log");
        break;

    case data_corruptor:
        sprintf(logFileName, "/tmp/dataCorruptor.log");
        break;
    }

    if (WriteLogToFile(logFileName, logMsg) == -1)
    {
        printf("Error writing log file: %s", logFileName);
    }
}

/*
 * FUNCTION : WriteLogToFile
 *
 * DESCRIPTION : This function is used to write log entry to the file provided path
 *
 * PARAMETERS : char *fileName      -   File name with complete path in which logs will be written
 *              char *message       -   updated message having timestamp
 *
 * Returns : return 1 if success, else return -1 if error occurred
 */
int WriteLogToFile(char *fileName, char *message)
{
    printf("%s", message);

    FILE *fp = NULL;

    fp = fopen(fileName, "a+"); // file open using write option
    if (fp == NULL)
    {
        printf("Error: can't open file1.info for writing\n");
        return -1; // returns -1 if writing log failed
    }

    // First line contains "This is the first line.\n"
    // Second line contains "This is the second line.\n"
    // Third line contains your full name, followed by a '\n'.
    if (fprintf(fp, "%s", message) < 0)
    {
        // if fprintf() returns less than 0, something went wrong
        printf("Can't write line to file\n");
        return -1; // returns -1 if writing log failed
    }

    if (fclose(fp) != 0)
    {
        printf("Error closing file4.note file\n");
        return -1; // returns -1 if writing log failed
    }

    return 1;
}