
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "../inc/data_reader.h"
#include "../../Common/inc/message_queues.h"
#include "../../Common/inc/logger.h"
#include "../../Common/inc/common.h"

int main (void)
{
    return LaunchDataReader();
}

int LaunchDataReader(void)
{
    char logMsg[200] = "";

	key_t	 	message_key;
	pid_t		myPID;
	int 		mid; // message ID
	int 		exitDataReader = FALSE;

	int 		rc;	// return code from message processing
	char		buffer[100];
	MSGENVELOPE 	msg;

	/* data reader PID will be used in message queues */
	myPID = getpid();

	message_key = ftok (".", 'K');
    if (message_key == -1) 
	{ 
	  LogMessage(data_reader,"(Data Reader) Cannot allocate key\n");
	  return 1;
	}	/* endif */



    if ((mid = msgget (message_key, 0)) == -1) 
	{
		LogMessage(data_reader, "(Data Reader) No queue available, create!\n");

		/*
		 * create one message queue (user/group read/write perms)
		 */

		mid = msgget (message_key, IPC_CREAT | 0660);
		if (mid == -1) 
		{
			LogMessage(data_reader, "(Data Reader) Cannot allocate a new queue!\n");
			return 2;
		}
	}
    
    sprintf(logMsg, "(Data Reader) Message queue ID is %d\n", mid);
	LogMessage(data_reader, logMsg);

    // waiting for 15 seconds to let Data Creators create messages
    sleep(30);

    while(! exitDataReader)
	{
        rc = msgrcv (mid, (void *)&msg, sizeof (MSGCONTENT), 0, 0); // set type = 0 to get mesgs in FIFO
		if (rc == -1) break;

        sprintf(logMsg, "(Data Creator) Message Sent with status code: %d - %s\n", (int)msg.type, GetMessageString(msg.type));
        LogMessage(data_creator, logMsg);

        sleep(1.5);
    }

    LogMessage(data_reader, "(Data Reader) Exiting ... removing msgQ and leaving ...\n");
	msgctl (mid, IPC_RMID, (struct msqid_ds *)NULL);
    return 1;
}
