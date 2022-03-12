
#include <time.h>

typedef struct tagMSGContent
{
	pid_t dcProcessID;
	time_t timeStamp;
} MSGCONTENT;

typedef struct tagMSGENVELOPE
{
	long type;
	MSGCONTENT data;
} MSGENVELOPE;

char *GetMessageString(long status);

#define NUMBER_OF_STATUSES 7

#define EVERYTHING_OKAY 0					 // 0 : means Everything is OKAY
#define HYDRAULIC_PRESSURE_FAILURE 1		 // 1 : means Hydraulic Pressure Failure
#define SAFETY_BUTTON_FAILURE 2				 // 2 : means Safety Button Failure
#define NO_RAW_MATERIAL_IN_PROCESS 3		 // 3 : means No Raw Material in the Process
#define OPERATING_TEMPERATURE_OUT_OF_RANGE 4 // 4 : means Operating Temperature Out of Range
#define OPERATING_ERROR 5					 // 5 : means Operator Error
#define MACHINE_OFFLINE 6					 // 6 : means Machine is Off-line

#define MESSAGE_QUEUE_KEY_ID 4578