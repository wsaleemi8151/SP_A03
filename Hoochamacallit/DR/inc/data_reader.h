/*
 * FILE : data_reader.h
 * PROJECT : SENG2030-22W-Sec1-System Programming - assignment # 3
 * Group: A03-17
 * PROGRAMMER : Gursharan Singh - Waqar Ali Saleemi
 * FIRST VERSION : 2022-03-08
 * DESCRIPTION :
 * Header file for data reader application
 */

#include "../../Common/inc/message_queues.h"
#include "../../Common/inc/logger.h"
#include "../../Common/inc/common.h"
#include "../../Common/inc/shared_memory.h"

int LaunchDataReader(void);
void ProcessMessage(MSGENVELOPE *msg, MasterList *lstMaster);
int GetMachineIndex(MasterList *lstMaster, pid_t dcProcessID);
void Check_DC_Machines_Status(MasterList *lstMaster);
