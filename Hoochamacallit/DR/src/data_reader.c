
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
	key_t	 	message_key;
	pid_t		myPID;
	int 		mid; // message ID
	int 		exitServer = FALSE;

	/* data reader PID will be used in message queues */
	myPID = getpid();

	message_key = ftok (".", 'K');
    if (message_key == -1) 
	{ 
	  LogMessage(data_reader,"(Data Reader) Cannot allocate key\n");
	  return 1;
	}	/* endif */


    char message[200] = "";
    sprintf(message, "(Data Reader) Checking for message queue ...with ID %d\n", message_key);
	LogMessage(data_reader, message);

    if ((mid = msgget (message_key, 0)) == -1) 
	{
		LogMessage(data_reader, "(Data Reader) No queue available, create!\n");

		/*
		 * nope, let's create one (user/group read/write perms)
		 */

		mid = msgget (message_key, IPC_CREAT | 0660);
		if (mid == -1) 
		{
			LogMessage(data_reader, "(Data Reader) Cannot allocate a new queue!\n");
			return 2;
		}
	}
    
    sprintf(message, "(Data Reader) Our message queue ID is %d\n", mid);
	LogMessage(data_reader, message);

    while(! exitServer)
	{
        sleep(30);
    }
}
