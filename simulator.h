#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "datatypes.h"
#include "StandardConstants.h"
#include "StringUtils.h"
#include "configops.h"
#include "metadataops.h"
#include "simtimer.h"
#include <pthread.h>

typedef enum{EXITING,
             NEW,
             READY,
             RUNNING} ProcessStates;


typedef struct ProcessStruct
{
  int processState;
  int processID;
  double processTime;
  struct MetaDataStruct *metaHeadPtr;
  struct ProcessStruct *nextPCB;

} PCBType;

typedef struct MemoryStruct
{
  int lowerBound;
  int upperBound;
  int totalMem;
  struct MemoryStruct *nextMem;
} MemoryType;


MemoryType *addMem(MemoryType *memPtr, PCBType *temp);

void changeProcess(PCBType *PCBptr, double currentTime, FILE* file, ConfigDataType *configPtr, char* timeStr);

void copyPCB( PCBType *PCBptr, PCBType PCBcopy);

//MemoryType createMem( MemoryType *memPtr, PCBType *temp);

PCBType *fillPCB( ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr, PCBType *PCBptr);

int findSize(PCBType *PCBptr);

void initMem();

void logHandling( PCBType *PCBptr, FILE* file, double currentTime, ConfigDataType *configPtr, MemoryType *MemPtr);

void runSim( ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr );

void* runTimerDev( void*  param );

void setAllReady( PCBType *PCBptr , double currentTime, FILE* file, ConfigDataType *configPtr, char* timestr);

void sortPCB( PCBType *PCBptr);

void swapPCB( PCBType *PCBptr, PCBType *PCBcopy);

bool verifyAccess(MemoryType *MemPtr,  PCBType *temp);

bool verifyAvailability(MemoryType *MemPtr, PCBType *temp);

#endif

