
#include "../inc/data_corruptor.h"

int main(void)
{
    return LaunchDataCorruptor();
}

int LaunchDataCorruptor(void)
{
    char logMsg[200] = "";

    key_t mid;
    int retryCounterOnStart = 0;
    int exitDataCorruptor = FALSE;
    int rc; // return code from message processing
    MSGENVELOPE msg;

    MasterList *lstMaster;

    int shmid;
    key_t shmem_key;

    // ------------------------------ Shared Memeory ---------------------------
    /* get the secret key_code to the area of shared memory we will allocate */
    shmem_key = ftok(".", SHARED_MEMORY_KEY_ID);
    if (shmem_key == -1)
    {
        LogMessage(data_corruptor, "Cannot allocate key\n");
        return 1;
    }

    /*
     * if shared mem exists, use it, otherwise allocate the block of memory
     */

    while ((shmid = shmget(shmem_key, sizeof(MasterList), 0)) == -1)
    {
        /*
         * nope, let's create one (user/group read/write perms)
         */

        LogMessage(data_corruptor, "No Shared-Memory currently available - sleeping for 10 seconds ...\n");

        if (retryCounterOnStart >= RETRY_LIMIT)
        {
            // if retry counter reaches to 100, exit data corruptor
            LogMessage(data_corruptor, "Tried 100 times, still Data Reader is not up running, Data corruptor is exiting ...\n");
            return 1;
        }
        sleep(10);

        retryCounterOnStart += 1;
    }

    sprintf(logMsg, "DX - Shared-Memory ID is %d\n", shmid);
    LogMessage(data_corruptor, logMsg);

    /* now allow the Data Reader (server) to attach to our shared memory and begin
       producing data to be read ... */

    lstMaster = (MasterList *)shmat(shmid, NULL, 0);
    if (lstMaster == NULL)
    {
        LogMessage(data_corruptor, "Cannot attach to shared memory!\n");
        return 3;
    }

    // setting message queue id
    mid = lstMaster->msgQueueID;

    while (!exitDataCorruptor)
    {
        srand(time(0)); // set new seed for rand()
        int interval = (rand() % 21) + 10;
        sleep(interval);

        // checking if message queue exists
        if ((mid = msgget(mid, 0)) == -1)
        {
            exitDataCorruptor = TRUE;
            LogMessage(data_corruptor, "DX detected that msgQ is gone - assuming DR/DCs done");
            break;
        }

        SpinTheWheelOfDistruction(lstMaster, mid);
    }

    return 1;
}

void SpinTheWheelOfDistruction(MasterList *lstMaster, key_t mid)
{
    // The %21, gives you a number between 0 and 20. Adding 10, gives you a number between 10 and 30.
    // Reference: https://stackoverflow.com/questions/17909215/c-random-numbers-between-10-and-30
    srand(time(0)); // set new seed for rand()
    int wheelOption = (rand() % 21);
    switch (wheelOption)
    {
    case 0:
    case 8:
    case 19:
        /* Do Nothing */
        break;

    case 10:
    case 17:
        /* delete the message queue being used between DCs and DR */
        LogMessage(data_reader, "DX deleted the msgQ - the DR/DCs can't talk anymore - exiting\n");
        msgctl(mid, IPC_RMID, (struct msqid_ds *)NULL);
        break;

    case 1:
    case 4:
    case 11:
        /* Kill DC-01 */
        KillProcess(lstMaster, 1, wheelOption);
        break;

    case 3:
    case 6:
    case 13:
        /* Kill DC-02 */
        KillProcess(lstMaster, 2, wheelOption);
        break;

    case 2:
    case 5:
    case 15:
        /* Kill DC-03 */
        KillProcess(lstMaster, 3, wheelOption);
        break;

    case 7:
        /* Kill DC-04 */
        KillProcess(lstMaster, 4, wheelOption);
        break;

    case 9:
        /* Kill DC-05 */
        KillProcess(lstMaster, 5, wheelOption);
        break;

    case 12:
        /* Kill DC-06 */
        KillProcess(lstMaster, 6, wheelOption);
        break;

    case 14:
        /* Kill DC-07 */
        KillProcess(lstMaster, 7, wheelOption);
        break;

    case 16:
        /* Kill DC-08 */
        KillProcess(lstMaster, 8, wheelOption);
        break;

    case 18:
        /* Kill DC-09 */
        KillProcess(lstMaster, 9, wheelOption);
        break;

    case 20:
        /* Kill DC-10 */
        KillProcess(lstMaster, 10, wheelOption);
        break;
    }
}

void KillProcess(MasterList *lstMaster, int indexOfDC, int wheelCounter)
{
    char logMsg[200];
    if (indexOfDC <= lstMaster->numberOfDCs)
    {
        // ref: How to use signal handlers in C language?
        // URL: https://linuxhint.com/signal_handlers_c_programming_language/

        if (kill(lstMaster->dc[indexOfDC].dcProcessID, SIGHUP) != -1)
        {
            sprintf(logMsg, "WOD Action %d  DC-%d [%d] TERMINATED\n", wheelCounter, indexOfDC, (int)lstMaster->dc[indexOfDC].dcProcessID);
            LogMessage(data_corruptor, logMsg);
        }
        else
        {
            sprintf(logMsg, "WOD Action FAILED: %d  DC-%d [%d] TERMINATION FAILED\n", wheelCounter, indexOfDC, lstMaster->dc[indexOfDC].dcProcessID);
            LogMessage(data_corruptor, logMsg);
        }
    }
}
