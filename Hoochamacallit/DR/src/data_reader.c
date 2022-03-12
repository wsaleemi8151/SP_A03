
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
	pid_t myPID;
	int mid; // message ID
	int exitDataReader = FALSE;

	int rc; // return code from message processing
	char buffer[100];
	MSGENVELOPE msg;

	MasterList *lstMaster;

	int shmid;
	key_t shmem_key;

	/* data reader PID will be used in message queues */
	myPID = getpid();

	// ---------------------------- Message Queue -----------------------------------
	// used /temp folder to get the same message key for data reader and data creator
	message_key = ftok("/tmp", MESSAGE_QUEUE_KEY_ID);
	if (message_key == -1)
	{
		LogMessage(data_reader, "(Data Reader) Cannot allocate key\n");
		return 1;
	} /* endif */

	if ((mid = msgget(message_key, 0)) == -1)
	{
		LogMessage(data_reader, "(Data Reader) No queue available, create!\n");

		/*
		 * create one message queue (user/group read/write perms)
		 */

		mid = msgget(message_key, IPC_CREAT | 0660);
		if (mid == -1)
		{
			LogMessage(data_reader, "(Data Reader) Cannot allocate a new queue!\n");
			return 2;
		}
	}

	sprintf(logMsg, "(Data Reader) Message queue ID is %d\n", mid);
	LogMessage(data_reader, logMsg);

	// -------------------------------------------------------------------------

	// ------------------------------ Shared Memeory ---------------------------
	/* get the secret key_code to the area of shared memory we will allocate */
	shmem_key = ftok(".", SHARED_MEMORY_KEY_ID);
	if (shmem_key == -1)
	{
		printf("(Data Reader) Cannot allocate key\n");
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

		printf("(Data Reader) No Shared-Memory currently available - so create!\n");
		shmid = shmget(shmem_key, sizeof(MasterList), IPC_CREAT | 0660);
		if (shmid == -1)
		{
			printf("(Data Reader) Cannot allocate a new memory!\n");
			return 2;
		}
	}

	printf("(Data Reader) Our Shared-Memory ID is %d\n", shmid);

	/* now allow the Data Reader (server) to attach to our shared memory and begin
	   producing data to be read ... */

	lstMaster = (MasterList *)shmat(shmid, NULL, 0);
	if (lstMaster == NULL)
	{
		printf("(Data Reader) Cannot attach to shared memory!\n");
		return 3;
	}

	// setting message queue id
	lstMaster->msgQueueID = mid;

	// setting number of DC to 0 on start
	lstMaster->numberOfDCs = 0;
	// ----------------------------------------------------------------------------------

	// waiting for 15 seconds to let Data Creators create messages
	sleep(15);

	while (!exitDataReader)
	{
		rc = msgrcv(mid, (void *)&msg, sizeof(MSGCONTENT), 0, 0); // set type = 0 to get mesgs in FIFO
		if (rc == -1)
			break;

		sprintf(logMsg, "(Data Reader) Message Received with status code: %d - %s\n", (int)msg.type, GetMessageString(msg.type));
		LogMessage(data_reader, logMsg);

		if (msg.type == EVERYTHING_OKAY && GetMachineIndex(lstMaster, msg.data.dcProcessID) == -1)
		{
			sprintf(logMsg, "DC-%d [%d] added to the master list - NEW DC - Status %d (%s)\n", lstMaster->numberOfDCs, (int)msg.data.dcProcessID, (int)msg.type, GetMessageString(msg.type));
			LogMessage(data_reader, logMsg);

			lstMaster->dc[lstMaster->numberOfDCs].dcProcessID = msg.data.dcProcessID;
			lstMaster->dc[lstMaster->numberOfDCs].lastTimeHeardFrom = msg.data.timeStamp;
			lstMaster->numberOfDCs += 1;
		}
		else if (msg.type == MACHINE_OFFLINE)
		{
			sprintf(logMsg, "DC-%d [%d] has gone OFFLINE - removing from master-list\n", GetMachineIndex(lstMaster, msg.data.dcProcessID), (int)msg.data.dcProcessID);
			LogMessage(data_reader, logMsg);

			lstMaster->dc[lstMaster->numberOfDCs].lastTimeHeardFrom = msg.data.timeStamp;
		}
		else
		{
			sprintf(logMsg, "DC-%d [%d] updated in the master list - MSG RECEIVED - Status %d (%s)\n", GetMachineIndex(lstMaster, msg.data.dcProcessID), (int)msg.data.dcProcessID, (int)msg.type, GetMessageString(msg.type));
			LogMessage(data_reader, logMsg);

			lstMaster->dc[lstMaster->numberOfDCs].lastTimeHeardFrom = msg.data.timeStamp;
		}

		sleep(1.5);
	}

	LogMessage(data_reader, "(Data Reader) Exiting ... removing msgQ and leaving ...\n");
	msgctl(mid, IPC_RMID, (struct msqid_ds *)NULL);

	/*
	 * detach and clean up our resources
	 */

	printf("(Data Reader) Detaching from Shared-Memory\n");
	shmdt(lstMaster);

	printf("(Data Reader) Removing the Shared-Memory resource\n");
	shmctl(shmid, IPC_RMID, 0);

	return 1;
}

int GetMachineIndex(MasterList *lstMaster, pid_t dcProcessID)
{
	for (size_t i = 0; i < MAX_DC_ROLES; i++)
	{
		if (lstMaster->dc[i].dcProcessID == dcProcessID)
		{
			return i;
		}
	}
	return -1;
}
