
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include "../inc/data_reader.h"

int main(void)
{
	return LaunchDataReader();
}

int LaunchDataReader(void)
{
	char logMsg[200] = "";

	key_t message_key;

	int mid; // message ID
	int exitDataReader = FALSE;

	int rc; // return code from message processing
	MSGENVELOPE msg;

	MasterList *lstMaster;

	int shmid;
	key_t shmem_key;

	// ---------------------------- Message Queue -----------------------------------
	// used /temp folder to get the same message key for data reader and data creator
	message_key = ftok("/tmp", MESSAGE_QUEUE_KEY_ID);
	if (message_key == -1)
	{
		LogMessage(data_reader, "DR - Cannot allocate key\n");
		return 1;
	} /* endif */

	if ((mid = msgget(message_key, 0)) == -1)
	{
		LogMessage(data_reader, "DR - No queue available, create!\n");

		/*
		 * create one message queue (user/group read/write perms)
		 */

		mid = msgget(message_key, IPC_CREAT | 0660);
		if (mid == -1)
		{
			LogMessage(data_reader, "DR - Cannot allocate a new queue!\n");
			return 2;
		}
	}

	sprintf(logMsg, "DR - Message queue ID is %d\n", mid);
	LogMessage(data_reader, logMsg);

	// -------------------------------------------------------------------------

	// ------------------------------ Shared Memeory ---------------------------
	/* get the secret key_code to the area of shared memory we will allocate */
	shmem_key = ftok(".", SHARED_MEMORY_KEY_ID);
	if (shmem_key == -1)
	{
		LogMessage(data_reader, "DR - Cannot allocate key\n");
		return 1;
	}

	/*
	 * if shared mem exists, use it, otherwise allocate the block of memory
	 */

	if ((shmid = shmget(shmem_key, sizeof(MasterList), 0)) == -1)
	{
		/*
		 * nope, let's create one (user/group read/write perms)
		 */

		LogMessage(data_reader, "DR - No Shared-Memory currently available - so create!\n");
		shmid = shmget(shmem_key, sizeof(MasterList), IPC_CREAT | 0660);
		if (shmid == -1)
		{
			LogMessage(data_reader, "DR - Cannot allocate a new memory!\n");
			return 2;
		}
	}

	sprintf(logMsg, "DR- Shared-Memory ID is %d\n", shmid);
	LogMessage(data_reader, logMsg);

	/* now allow the Data Reader (server) to attach to our shared memory and begin
	   producing data to be read ... */

	lstMaster = (MasterList *)shmat(shmid, NULL, 0);
	if (lstMaster == NULL)
	{
		LogMessage(data_reader, "DR - Cannot attach to shared memory!\n");
		return 3;
	}

	// setting message queue id in the shared memory master list
	lstMaster->msgQueueID = message_key;

	// setting number of DC to 0 on start
	lstMaster->numberOfDCs = 0;
	// ----------------------------------------------------------------------------------

	// waiting for 15 seconds to let Data Creators create messages
	sleep(15);

	while (!exitDataReader)
	{
		// sprintf(logMsg, "Here------------NoOfDCs: %d\n\n", lstMaster->numberOfDCs);
		// LogMessage(data_reader, logMsg);

		rc = msgrcv(mid, (void *)&msg, sizeof(MSGCONTENT), 0, 0); // set type = 0 to get mesgs in FIFO
		if (rc == -1)
		{
			break;
		}

		// Process msg queue mesg
		ProcessMessage(&msg, lstMaster);

		// checking if any DC machine is not responding
		Check_DC_Machines_Status(lstMaster);

		// checking if all DC machines goes offline than exit the main loop
		if (lstMaster->numberOfDCs == 0)
		{
			break;
		}
		sleep(1.5);
	}

	LogMessage(data_reader, "DR - Exiting ... removing msgQ and leaving ...\n");
	msgctl(mid, IPC_RMID, (struct msqid_ds *)NULL);

	/*
	 * detach and clean up our resources
	 */

	LogMessage(data_reader, "DR - Detaching from Shared-Memory\n");
	shmdt(lstMaster);

	LogMessage(data_reader, "DR - Removing the Shared-Memory resource\n");
	shmctl(shmid, IPC_RMID, 0);

	return 1;
}

void ProcessMessage(MSGENVELOPE *msg, MasterList *lstMaster)
{
	char logMsg[200];

	sprintf(logMsg, "DR - Message Received with status code: %d - %s\n", (int)msg->type, GetMessageString(msg->type));
	LogMessage(data_reader, logMsg);

	int dcMachineIndex = GetMachineIndex(lstMaster, msg->data.dcProcessID);

	if (msg->type == EVERYTHING_OKAY && dcMachineIndex == -1)
	{
		sprintf(logMsg, "DC-%d [%d] added to the master list - NEW DC - Status %d (%s)\n", lstMaster->numberOfDCs + 1, (int)msg->data.dcProcessID, (int)msg->type, GetMessageString(msg->type));
		LogMessage(data_reader, logMsg);

		lstMaster->dc[lstMaster->numberOfDCs].dcProcessID = msg->data.dcProcessID;
		lstMaster->dc[lstMaster->numberOfDCs].lastTimeHeardFrom = msg->data.timeStamp;
		if (lstMaster->numberOfDCs == -1)
		{
			lstMaster->numberOfDCs = 1;
		}
		else
		{
			lstMaster->numberOfDCs += 1;
		}
	}
	else if (msg->type == MACHINE_OFFLINE)
	{
		sprintf(logMsg, "DC-%d [%d] has gone OFFLINE - removing from master-list\n", dcMachineIndex + 1, (int)msg->data.dcProcessID);
		LogMessage(data_reader, logMsg);

		lstMaster->numberOfDCs -= 1;
	}
	else
	{
		sprintf(logMsg, "DC-%d [%d] updated in the master list - MSG RECEIVED - Status %d (%s)\n", dcMachineIndex + 1, (int)msg->data.dcProcessID, (int)msg->type, GetMessageString(msg->type));
		LogMessage(data_reader, logMsg);

		lstMaster->dc[dcMachineIndex].lastTimeHeardFrom = msg->data.timeStamp;
	}
}

int GetMachineIndex(MasterList *lstMaster, pid_t dcProcessID)
{
	if (lstMaster->numberOfDCs > 0)
	{
		for (size_t i = 0; i < lstMaster->numberOfDCs; i++)
		{
			if (lstMaster->dc[i].dcProcessID == dcProcessID)
			{
				return i;
			}
		}
	}
	return -1;
}

void Check_DC_Machines_Status(MasterList *lstMaster)
{
	char logMsg[200];

	for (size_t i = 0; i < lstMaster->numberOfDCs; i++)
	{
		time_t now;
		time(&now);

		double diffInSeconds = difftime(now, lstMaster->dc[i].lastTimeHeardFrom);

		sprintf(logMsg, "diffInSeconds is:%0.3f \n", diffInSeconds);
		// LogMessage(data_reader, logMsg);

		if (diffInSeconds > MAX_DC_RESPONSE_INTERVAL)
		{
			sprintf(logMsg, "DC-%d [%d] has gone OFFLINE - removing from master-list\n", (int)i, (int)lstMaster->dc[i].dcProcessID);
			LogMessage(data_reader, logMsg);
		}
	}
}
