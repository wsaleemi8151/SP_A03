
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <time.h>

#include <unistd.h>
#include<signal.h>


#include "../../Common/inc/message_queues.h"
#include "../../Common/inc/logger.h"
#include "../../Common/inc/common.h"
#include "../../Common/inc/shared_memory.h"

int LaunchDataCorruptor(void);
void SpinTheWheelOfDistruction(MasterList *lstMaster, key_t message_key);
void KillProcess(MasterList *lstMaster, int indexOfDC, int wheelCounter);

#define RETRY_LIMIT 100
