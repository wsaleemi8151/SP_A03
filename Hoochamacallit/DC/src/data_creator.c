
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "../inc/data_creator.h"
#include "../../Common/inc/message_queues.h"
#include "../../Common/inc/logger.h"

int main (void)
{
	return LaunchDataCreator();
}

int LaunchDataCreator(void)
{
	key_t	 	message_key;
	pid_t		myPID;
	int 		mid; // message ID

	/* data creator PID will be used in message queues */
	myPID = getpid();

	message_key = ftok (".", 4578);
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
	LogMessage(data_creator, "(Data Creator) Checking for message queue ...\n");

	while ((mid = msgget (message_key, 0)) == -1) 
	{
		/*
		 * not available yet ... check in 10 seconds
		 */
		LogMessage(data_creator, "(Data Creator) Queue not available ... sleeping for 10 seconds ...\n");
		sleep (10);
	}	/* end while */

    // reach here if data creator found the message queue
    
    char message[200] = "";
    sprintf(message, "(Data Reader) Our message queue ID is %d\n", mid);
	LogMessage(data_reader, message);

}
