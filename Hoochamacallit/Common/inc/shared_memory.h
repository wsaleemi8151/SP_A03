
#include <time.h>

#define MAX_DC_ROLES 10

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
