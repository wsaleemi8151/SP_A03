
#include "../../Common/inc/message_queues.h"
#include "../../Common/inc/logger.h"
#include "../../Common/inc/common.h"
#include "../../Common/inc/shared_memory.h"

int LaunchDataReader(void);
int GetMachineIndex(MasterList *lstMaster, pid_t dcProcessID);
int Check_DC_Machines_Status(MasterList *lstMaster);
