/*
 * FILE : shared_memory.h
 * PROJECT : SENG2030-22W-Sec1-System Programming - assignment # 3
 * Group: A03-17
 * PROGRAMMER : Gursharan Singh - Waqar Ali Saleemi
 * FIRST VERSION : 2022-03-08
 * DESCRIPTION :
 * Header file for shared functionality related to Shared Memory
 */

#include <time.h>

#define MAX_DC_ROLES 10
#define MAX_DC_RESPONSE_INTERVAL 35

typedef struct
{
    pid_t dcProcessID;
    time_t lastTimeHeardFrom;
} DCInfo;

typedef struct
{
    int msgQueueID;
    int numberOfDCs;
    DCInfo dc[MAX_DC_ROLES];
} MasterList;

#define SHARED_MEMORY_KEY_ID 16535
