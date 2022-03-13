
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
        LogMessage(data_creator, "Cannot allocate key\n");
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
        LogMessage(data_creator, "Queue not available ... sleeping for 10 seconds ...\n");
        sleep(10);
    } /* end while */

    // reach here if data creator found the message queue

    sprintf(logMsg, "Message queue ID is %d\n", mid);
    LogMessage(data_creator, logMsg);

    while (!exitDataCreator)
    {

        if (isFirstTime == TRUE)
        {
            time_t startTime;
            time(&startTime);

            msg.type = 7; // rand() % NUMBER_OF_STATUSES;
            msg.data.timeStamp = startTime;
            msg.data.dcProcessID = myPID;

            rc = msgsnd(mid, (void *)&msg, sizeof(MSGCONTENT), 0);
            if (rc == -1)
            {
                LogMessage(data_creator, "Send error!\n");
                return 4;
            }

            sprintf(logMsg, "Message Sent with status code: %d - %s\n", (int)msg.type, GetMessageString(msg.type));
            LogMessage(data_creator, logMsg);

            isFirstTime = FALSE;
            int interval = (rand() % 21) + 10;
            sleep(interval);
        }

        time_t now;
        time(&now);

        // loop until shutdown status is sent to the data reader
        // as we are sending mesg in msg type, so type 0 causes error
        msg.type = (rand() % NUMBER_OF_STATUSES) + 1;
        msg.data.timeStamp = now;
        msg.data.dcProcessID = myPID;

        rc = msgsnd(mid, (void *)&msg, sizeof(MSGCONTENT), 0);
        if (rc == -1)
        {
            LogMessage(data_creator, "Send error!\n");

            sprintf(logMsg, "Message Sent with status code: %d - %s\n", (int)msg.type, GetMessageString(msg.type));
            LogMessage(data_creator, logMsg);

            return 4;
        }

        sprintf(logMsg, "Message Sent with status code: %d - %s\n", (int)msg.type, GetMessageString(msg.type));
        LogMessage(data_creator, logMsg);

        if (msg.type == MACHINE_OFFLINE)
        {
            exitDataCreator = TRUE;
        }
        else
        {
            // The %21, gives you a number between 0 and 20. Adding 10, gives you a number between 10 and 30.
            // Reference: https://stackoverflow.com/questions/17909215/c-random-numbers-between-10-and-30
            int interval = (rand() % 21) + 10;
            sleep(interval);
        }
    }

    // data creator is done and exiting
    return 1;
}
