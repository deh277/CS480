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


void changeProcess(PCBType *PCBptr, double currentTime, FILE* file, ConfigDataType *configPtr, char* timeStr);

PCBType *fillPCB( ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr, PCBType *PCBptr);

void logHandling( PCBType *PCBptr, FILE* file, double currentTime, ConfigDataType *configPtr);

void setAllReady( PCBType *PCBptr , double currentTime, FILE* file, ConfigDataType *configPtr, char* timestr);

void runSim( ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr );

void* runTimerDev( void*  param );


#endif

