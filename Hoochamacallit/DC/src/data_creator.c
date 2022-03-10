
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

int main (void)
{
	return LaunchDataCreator();
}

int LaunchDataCreator(void)
{
	key_t	 	    message_key;
	pid_t		    myPID;
	int 		    mid; // message ID
	char 		    exitDataCreator  = FALSE;
	MSGENVELOPE 	msg;

	int	 	        rc;	// return code from message processing

	/* data creator PID will be used in message queues */
	myPID = getpid();

	message_key = ftok (".", 'K');
    if (message_key == -1) 
	{ 
	  LogMessage(data_creator,"(Data Creator) Cannot allocate key\n");
	  return 1;
	}	/* endif */


	/*
	 * the data creator will wait around 10 seconds of checking
	 * for the message queue (to give the server time to start)
     * and keep in this loop until it found message queue
	 */

    char message[200] = "";
    sprintf(message, "(Data Creator) Checking for message queue ...with ID %d\n", message_key);
	LogMessage(data_creator, message);

	while ((mid = msgget (message_key, 0)) == -1) 
	{
		/*
		 * not available yet ... check in 10 seconds
		 */
		LogMessage(data_creator, "(Data Creator) Queue not available ... sleeping for 10 seconds ...\n");
		sleep (10);
	}	/* end while */

    // reach here if data creator found the message queue
    
    sprintf(message, "(Data Creator) Our message queue ID is %d\n", mid);
	LogMessage(data_creator, message);

    msg.type = EVERYTHING_OKAY;
    msg.data.dcProcessID = myPID;
    msg.data.timeStamp = time(NULL);

    while(! exitDataCreator)
	{
        // loop until shutdown status is sent to the data reader
        msg.type = rand() % NUMBER_OF_STATUSES;
        msg.data.timeStamp = time(NULL);
        msg.data.dcProcessID = myPID;

        if(msg.type == MACHINE_OFFLINE)
        {
            exitDataCreator = TRUE;
        }

        rc = msgsnd (mid, (void *)&msg, sizeof (MSGCONTENT), 0);
        if (rc == -1) 
        {
	        LogMessage(data_creator, "(Data Creator) Send error!\n");
            return 4;
        }
        
        sprintf(message, "(Data Creator) Message Sent with status code: %d - %s\n", (int)msg.type, GetMessageString(msg.type));
        LogMessage(data_creator, message);
        sleep (10);
    }

    // data creator is done and exiting
    return 1;
}
