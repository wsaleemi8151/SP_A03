/*
 * FILE : data_creator.c
 * PROJECT : SENG2030-22W-Sec1-System Programming - assignment # 3
 * Group: A03-17
 * PROGRAMMER : Gursharan Singh - Waqar Ali Saleemi
 * FIRST VERSION : 2022-03-08
 * DESCRIPTION :
 * Source code file for data creator application
 * Function in this file are used to handle the functionality for DC application
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>

#include "../inc/data_creator.h"
#include "../../Common/inc/message_queues.h"
#include "../../Common/inc/logger.h"
#include "../../Common/inc/common.h"

int main(void)
{
    return LaunchDataCreator();
}

// srand help retrieved from:
// https://www.geeksforgeeks.org/rand-and-srand-in-ccpp/
// srand(time(NULL));

/*
 * FUNCTION : LaunchDataCreator
 *
 * DESCRIPTION : This is the main function which handles all logic for DC application
 *
 * PARAMETERS : void
 *
 * Returns : int    -   this return value is used in the main method to return the exit status
 */
int LaunchDataCreator(void)
{
    char logMsg[200] = "";
    int isFirstTime = TRUE;

    key_t message_key;
    pid_t myPID;
    int mid; // message ID
    char exitDataCreator = FALSE;
    MSGENVELOPE msg;

    int rc; // return code from message processing

    /* data creator PID will be used in message queues */
    myPID = getpid();

    // used /temp folder to get the same message key for data reader and data creator
    message_key = ftok("/tmp", MESSAGE_QUEUE_KEY_ID);
    if (message_key == -1)
    {
        sprintf(logMsg, "DC [%d] - Cannot allocate key\n", myPID);
        LogMessage(data_creator, logMsg);
        return 1;
    } /* endif */

    /*
     * the data creator will wait around 10 seconds of checking
     * for the message queue (to give the server time to start)
     * and keep in this loop until it found message queue
     */

    while ((mid = msgget(message_key, 0)) == -1)
    {
        /*
         * not available yet ... check in 10 seconds
         */
        sprintf(logMsg, "DC [%d] - Queue not available ... sleeping for 10 seconds ...\n", myPID);
        LogMessage(data_creator, logMsg);
        sleep(10);
    } /* end while */

    // reach here if data creator found the message queue

    sprintf(logMsg, "DC [%d] - Message queue ID is %d\n", myPID, mid);
    LogMessage(data_creator, logMsg);

    while (!exitDataCreator)
    {

        if (isFirstTime == TRUE)
        {
            time_t startTime;
            time(&startTime);

            msg.type = EVERYTHING_OKAY;
            msg.data.timeStamp = startTime;
            msg.data.dcProcessID = myPID;

            rc = msgsnd(mid, (void *)&msg, sizeof(MSGCONTENT), 0);
            if (rc == -1)
            {
                sprintf(logMsg, "DC [%d] - Send message error!\n", myPID);
                LogMessage(data_creator, logMsg);
                return 4;
            }

            sprintf(logMsg, "DC [%d] - Message Sent with status code: %d - %s\n", myPID, (int)msg.type, GetMessageString(msg.type));
            LogMessage(data_creator, logMsg);

            isFirstTime = FALSE;
            srand(time(NULL)); // set new seed for rand()
            int interval = (rand() % 21) + 10;
            sleep(interval);
        }

        time_t now;
        time(&now);

        // loop until shutdown status is sent to the data reader
        // as we are sending mesg in msg type, so type 0 causes error
        srand(time(NULL)); // set new seed for rand()
        msg.type = (rand() % NUMBER_OF_STATUSES) + 1;
        msg.data.timeStamp = now;
        msg.data.dcProcessID = myPID;

        // if for some reasons, message queue is deleted,
        // DC will exit after logging the issue
        if ((mid = msgget(message_key, 0)) == -1)
        {
            sprintf(logMsg, "DC [%d] - Queue is unavailable ... exiting DC machine ...\n", myPID);
            LogMessage(data_creator, logMsg);
            break;
        }

        rc = msgsnd(mid, (void *)&msg, sizeof(MSGCONTENT), 0);
        if (rc == -1)
        {
            sprintf(logMsg, "DC [%d] - Send message error!\n", myPID);
            LogMessage(data_creator, logMsg);

            sprintf(logMsg, "DC [%d] - Message Sent with status code: %d - %s\n", myPID, (int)msg.type, GetMessageString(msg.type));
            LogMessage(data_creator, logMsg);

            return 4;
        }

        sprintf(logMsg, "DC [%d] - Message Sent with status code: %d - %s\n", myPID, (int)msg.type, GetMessageString(msg.type));
        LogMessage(data_creator, logMsg);

        if (msg.type == MACHINE_OFFLINE)
        {
            exitDataCreator = TRUE;
        }
        else
        {
            // The %21, gives you a number between 0 and 20. Adding 10, gives you a number between 10 and 30.
            // Reference: https://stackoverflow.com/questions/17909215/c-random-numbers-between-10-and-30
            srand(time(NULL)); // set new seed for rand()
            int interval = (rand() % 21) + 10;
            sleep(interval);
        }
    }

    // data creator is done and exiting
    return 1;
}
