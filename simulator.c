#include "simulator.h"


//phase 2 notes

//key requirements:
//introducing process control blocks
    //contains: pointer, process state, and Pid
//manage and process multiple programs sequentially
//implement first come first served scheduling(queue)
//display and store output to monitor, file, or both
//implement POSIX threads for I/O operations
//track process states( New, Ready, Running, and Exiting)


void runSim( ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr )
{
  //open file 
  FILE* file = fopen( configPtr->logToFileName, "w");

  
  //initialize variables
  PCBType *PCBptr = NULL;
  double currentTime = 0.0;
  MemoryType *MemPtr = NULL;
  
  //create linked list
  PCBptr = fillPCB( configPtr, metaDataMstrPtr, PCBptr );
  

  char timestr[STD_STR_LEN];
  accessTimer(ZERO_TIMER,timestr);

  //print title
  if( configPtr->logToCode == LOGTO_MONITOR_CODE)
  {
    printf("Simulator Run\n");
    printf("-------------\n\n");
    printf(" %s, OS: Simulator Start\n", timestr );
  }
  else if( configPtr->logToCode == LOGTO_BOTH_CODE)
  {
    printf("Simulator Run\n");
    printf("-------------\n\n");
    printf(" %s, OS: Simulator Start\n", timestr );
    fprintf(file, "Simulator Run\n");
    fprintf(file, "-------------\n\n");
    fprintf(file, " %s, OS: Simulator Start\n", timestr );
  }
  else if( configPtr->logToCode ==  LOGTO_FILE_CODE)
  {
    fprintf(file, "Simulator Run\n");
    fprintf(file, "-------------\n\n");
    fprintf(file, " %s, OS: Simulator Start\n", timestr );
  }
  
  //set all proccesses to ready
  setAllReady( PCBptr , currentTime, file, configPtr, timestr);

  //memory start function
  initMem();

  //sort PCB list
  if( (configPtr->cpuSchedCode = CPU_SCHED_SFJ_N_CODE))
  {
    sortPCB( PCBptr );
  }
  //while op codes are available
  while( PCBptr != NULL)
  {

    // Select next process based on CPU scheduling strategy
    switch (configPtr->cpuSchedCode)
    {
      case CPU_SCHED_FCFS_P_CODE:
        PCBptr = getNextProcess_FCFSP(&PCBptr, NULL); 
        break;

      case CPU_SCHED_SRTF_P_CODE:
        PCBptr = getNextProcess_SRTFP(&PCBptr);
        break;

      case CPU_SCHED_RR_P_CODE:
        PCBptr = getNextProcess_RRP(&PCBptr, &PCBptr); 
        break;

      default: // fallback to current logic for FCFS-N or SJF-N
        break;
    }

    // Check if process selected
    if (PCBptr == NULL)
    {
      break;
    }
    //check log code for output
    if( configPtr->logToCode == LOGTO_MONITOR_CODE)
    {
      //output process selection
      printf(" %s, OS: Process %d selected with %d ms remaining\n", timestr, PCBptr->processID ,  (int) PCBptr->processTime);
    }
    else if(configPtr->logToCode == LOGTO_BOTH_CODE)
    {
      //print and save to file
      printf(" %s, OS: Process %d selected with %d ms remaining\n", timestr, PCBptr->processID ,  (int) PCBptr->processTime);
      fprintf(file, " %s, OS: Process %d selected with %d ms remaining\n", timestr, PCBptr->processID ,  (int) PCBptr->processTime);
    }
    else if( configPtr->logToCode == LOGTO_FILE_CODE )
    {
      //save to file
      fprintf(file, " %s, OS: Process %d selected with %d ms remaining\n", timestr, PCBptr->processID ,  (int) PCBptr->processTime);
    }
 
    
    //change process code
    changeProcess(PCBptr, currentTime, file, configPtr, timestr);
    
    //makes sure you aren;t at the end of the meta data
    while( compareString(PCBptr->metaHeadPtr->command , "app") != STR_EQ || compareString(PCBptr->metaHeadPtr->strArg1  , "end") != STR_EQ)
    {
      //handles process log/output
      logHandling( PCBptr, file , currentTime, configPtr, MemPtr);
      
      //move to next meta data ptr
      PCBptr->metaHeadPtr = PCBptr->metaHeadPtr->nextNode;
      
    }
  
    
    //set to exit
    changeProcess(PCBptr, currentTime, file, configPtr, timestr);
    //go to the next PCB
    PCBptr = PCBptr->nextPCB;
  }

  //check log codes
  if( configPtr->logToCode == LOGTO_MONITOR_CODE)
  {
    //print system stop and simulation end
    printf(" %s, OS: System stop\n", timestr);
    accessTimer(LAP_TIMER,timestr);
    printf(" %s, OS: Simulation End\n", timestr);
  }
  else if(configPtr->logToCode == LOGTO_BOTH_CODE)
  {
    //print and save to file
    printf(" %s, OS: System stop\n", timestr);
    fprintf( file, " %s, OS: System stop\n", timestr );
    accessTimer(LAP_TIMER,timestr);
    printf(" %s, OS: Simulation End\n", timestr);
    fprintf( file, " %s, OS: Simulation End\n", timestr);
  }
  else if( configPtr->logToCode == LOGTO_FILE_CODE )
  {
    //save to file
    fprintf( file, " %s, OS: System stop\n", timestr );
    accessTimer(LAP_TIMER,timestr);
    fprintf( file, " %s, OS: Simulation End\n", timestr);
  }

  //close file
  fclose(file);

  free( PCBptr );

}


void initMem()
{
  printf("--------------------------------------------------\n");
  printf("After memory initalization\n");
  printf("0 [ Open , P#: x, 0-0 ] 11099\n");
  printf("--------------------------------------------------\n");
}


void sortPCB( PCBType *PCBptr)
{
  int outerIndex, innerIndex;
  int totalPCB;
  //PCBType *temp = PCBptr;

  totalPCB = findSize( PCBptr );

  for( outerIndex = 0; outerIndex < totalPCB - 1; outerIndex++)
  {
    for( innerIndex = 0; innerIndex < totalPCB - outerIndex - 1; innerIndex++)
    {
      if(PCBptr->processTime > PCBptr->nextPCB->processTime)
      {
        swapPCB( PCBptr, PCBptr->nextPCB);
      }
    }
  }

  //loop

  //TODO
}

int findSize(PCBType *PCBptr)
{
  int count = 0;
  PCBType *temp = PCBptr;

  while( temp != NULL)
  {
    temp = temp->nextPCB;

    count++;
  }

  free(temp);

  return count;
}

void swapPCB( PCBType *PCBptr, PCBType *PCBcopy)
{
  PCBType temp;

  copyPCB(&temp, *PCBptr);

  copyPCB(PCBptr, *PCBcopy);

  copyPCB( PCBcopy, temp);

}

void copyPCB( PCBType *PCBptr, PCBType PCBcopy)
{
  PCBptr->processID = PCBcopy.processID;
  PCBptr->processTime = PCBcopy.processTime;
  PCBptr->processState = PCBcopy.processState;
  PCBptr->metaHeadPtr = PCBcopy.metaHeadPtr;

}

PCBType *fillPCB( ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr, PCBType *PCBptr)
{
  PCBType *headPCB = NULL;
  PCBType *temp;
  int pid = 0;

  //iterate through meta data 
  while( compareString(metaDataMstrPtr->command , "sys") != STR_EQ || compareString(metaDataMstrPtr->strArg1 , "end") != STR_EQ)
  {

    
    //initialize PCB variables
    PCBptr = (PCBType *)malloc(sizeof( PCBType ) );
    PCBptr->processID = pid;
    PCBptr->nextPCB = NULL;
    PCBptr->processState = NEW;
    PCBptr->processTime = 0;
    PCBptr->metaHeadPtr = metaDataMstrPtr;
    
    while( compareString(metaDataMstrPtr->command , "app") != STR_EQ || compareString(metaDataMstrPtr->strArg1 , "end") != STR_EQ )
    {

      //check for dev/cpu
      if( compareString(metaDataMstrPtr->command , "dev") == STR_EQ)
      {
        //add process time
        PCBptr->processTime += configPtr->ioCycleRate * metaDataMstrPtr->intArg2;
      }
      else if( compareString(metaDataMstrPtr->command , "cpu") == STR_EQ)
      {
        //add process time
        PCBptr->processTime += configPtr->procCycleRate * metaDataMstrPtr->intArg2;
        //runTimer( PCBptr->processTime);
      }

      
      //move to next meta data node
      metaDataMstrPtr = metaDataMstrPtr->nextNode;
    }
    
    //set head ptr
    if(headPCB == NULL )
      {
        headPCB = PCBptr;
      }
      else
      {
        //move head otr to next 
        //LL logic...
        temp = headPCB;
        while( temp->nextPCB != NULL )
        {
          temp = temp->nextPCB;
        }
        
        temp->nextPCB = PCBptr;
        //end LL logic
      } 
    
    //itterate through PCB
    PCBptr =  PCBptr->nextPCB;

    //edit pid
    //yeah idk where the 6 comes from either
    pid++;


    //move to next metadata node
    metaDataMstrPtr = metaDataMstrPtr->nextNode;
  }
  free(temp);
  //return PCB head
  return headPCB;
   
}


void setAllReady( PCBType *PCBptr , double currentTime, FILE* file, ConfigDataType *configPtr, char* timestr)
{
  //initialize variabls
  PCBType *temp = PCBptr;
    
  //loop through all PCBs
  while( temp != NULL )
  {
    accessTimer( LAP_TIMER, timestr);
      //check log code
    if( configPtr->logToCode == LOGTO_MONITOR_CODE)
    {
      //print ready state
      printf(" %s, OS: Process %d set to READY state from NEW state\n", timestr , temp->processID);
    }
    else if(configPtr->logToCode == LOGTO_BOTH_CODE)
    {
      //print/log ready state
      printf(" %s, OS: Process %d set to READY state from NEW state\n", timestr , temp->processID);
      fprintf(file, " %s, OS: Process %d set to READY state from NEW state\n", timestr , temp->processID);
    }
    else if( configPtr->logToCode == LOGTO_FILE_CODE )
    {
      //log ready state
      fprintf(file, " %s, OS: Process %d set to READY state from NEW state\n", timestr, PCBptr->processID);
    }
    //set process state to ready
    temp->processState = READY;
   
    //move to next PCB
    temp = temp->nextPCB;
  }
  
  free(temp);
}




void changeProcess(PCBType *PCBptr, double currentTime, FILE* file, ConfigDataType *configPtr, char* timeStr)
{
  accessTimer(LAP_TIMER,timeStr);
  //check process type for next process
  if(PCBptr->processState == READY)
  {
    //set to ruinning if ready
    PCBptr->processState = RUNNING;
    //check log code
    if( configPtr->logToCode == LOGTO_MONITOR_CODE)
    {
      printf(" %s, OS: Process %d set from READY to RUNNING\n\n", timeStr, PCBptr->processID);
    }
    else if( configPtr->logToCode == LOGTO_BOTH_CODE)
    {
      printf(" %s, OS: Process %d set from READY to RUNNING\n\n", timeStr, PCBptr->processID);
      fprintf(file, " %s, OS: Process %d set from READY to RUNNING\n\n", timeStr, PCBptr->processID);
    }
    else if( configPtr->logToCode == LOGTO_FILE_CODE)
    {
      fprintf(file, " %s, OS: Process %d set from READY to RUNNING\n\n", timeStr, PCBptr->processID);
    }
  }
  else if( PCBptr->processState == RUNNING )
  {
    //set to ext if ready
    PCBptr->processState = EXITING;
    //check log code
    if( configPtr->logToCode == LOGTO_MONITOR_CODE)
    {
      printf("\n %s, OS: Process %d ended\n", timeStr, PCBptr->processID);
      accessTimer(LAP_TIMER,timeStr);
      printf(" %s, OS: Process %d set to EXIT\n", timeStr, PCBptr->processID);
    }
    else if( configPtr->logToCode == LOGTO_BOTH_CODE)
    {
      printf("\n %s, OS: Process %d ended\n", timeStr, PCBptr->processID);
      printf(" %s, OS: Process %d set to EXIT\n", timeStr, PCBptr->processID);
      accessTimer(LAP_TIMER,timeStr);
      fprintf(file,"\n %s, OS: Process %d ended\n", timeStr, PCBptr->processID);
      fprintf(file, " %s, OS: Process %d set to EXIT\n", timeStr, PCBptr->processID);

    }
    else if(configPtr->logToCode == LOGTO_FILE_CODE)
    {
      fprintf(file,"\n %s, OS: Process %d ended\n", timeStr, PCBptr->processID);
      accessTimer(LAP_TIMER,timeStr);
      fprintf(file, " %s, OS: Process %d set to EXIT\n", timeStr, PCBptr->processID);
    }
  }

}





void logHandling( PCBType *PCBptr, FILE* file, double currentTime, ConfigDataType *configPtr, MemoryType *MemPtr)
{
  PCBType *temp = PCBptr;
  char timestring[20];
  int timeRunner;
  pthread_t tid;
  pthread_attr_t attr;

  if( configPtr->logToCode == LOGTO_MONITOR_CODE)
  {
    if( compareString(temp->metaHeadPtr->strArg1, "start") != STR_EQ)
    {  

      if( compareString(temp->metaHeadPtr->command, "cpu") == STR_EQ )
      {
        timeRunner =(temp->metaHeadPtr->intArg2)*(configPtr->procCycleRate);

        accessTimer(LAP_TIMER,timestring);
        printf(" %s, Process: %d, %s %s %s Operation Start \n",timestring, temp->processID, temp->metaHeadPtr->command, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->inOutArg);
        runTimer(timeRunner);
        accessTimer(LAP_TIMER,timestring);
        printf(" %s, Process: %d, %s %s %s Operation end \n",timestring, temp->processID, temp->metaHeadPtr->command, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->inOutArg);
      }
      else if(( compareString(temp->metaHeadPtr->command, "dev") == STR_EQ ))
      {
        timeRunner =(temp->metaHeadPtr->intArg2)*(configPtr->ioCycleRate);

        printf(" %s, Process: %d, %s %sput Operation start \n",timestring, temp->processID, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->inOutArg);
        printf(" %s, Process: %d, %s %sput Operation end \n",timestring, temp->processID, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->inOutArg);
      }
      else
      {
        printf(" %s0, Process: %d, %s %s request ( %d, %d )\n",timestring, temp->processID, temp->metaHeadPtr->command, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->intArg2, temp->metaHeadPtr->intArg3);

        printf("--------------------------------------------------\n");
        //good luck figuring out why this just doesn't feel like working 
        if( (compareString(temp->metaHeadPtr->strArg1, "access") == STR_EQ) )
        {
          printf("I get reached");
          if(verifyAccess(MemPtr, temp))
          {
            printf("After access success\n");
            printf("0 [Used, P#: %d, %d-%d ] %d", temp->processID, MemPtr->lowerBound, MemPtr->upperBound, MemPtr->totalMem);
            printf("%d [Open, P#: x, %d-%d ] 11099", (MemPtr->upperBound + 1),  MemPtr->lowerBound, MemPtr->upperBound);
            //TODO
          }
          else
          {
            printf("After access failure");
            printf("0 [Used, P#: %d, %d-%d ] %d", temp->processID, MemPtr->lowerBound, MemPtr->upperBound, MemPtr->totalMem);
          }
        }
        else if( compareString(temp->metaHeadPtr->strArg1, "allocate") == STR_EQ )
        {
          printf("I get reached");
          if( verifyAvailability(MemPtr, temp))
          {
            printf("After allocate success\n");
            printf("0 [Used, P#: %d, %d-%d ] %d", temp->processID, MemPtr->lowerBound, MemPtr->upperBound, MemPtr->totalMem);
          }
          else
          {
            printf("After allocate overlap failure");
            printf("0 [Used, P#: %d, %d-%d ] %d", temp->processID, MemPtr->lowerBound, MemPtr->upperBound, MemPtr->totalMem);
          }
          
        }

        printf("--------------------------------------------------\n");
      }
    }
  }
  else if( configPtr->logToCode == LOGTO_BOTH_CODE)
  {
    if( compareString(temp->metaHeadPtr->strArg1, "start") != STR_EQ)
    {  

      if( compareString(temp->metaHeadPtr->command, "cpu") == STR_EQ )
      {
        timeRunner =(temp->metaHeadPtr->intArg2)*(configPtr->procCycleRate);

        accessTimer(LAP_TIMER,timestring);
        fprintf( file, " %s, Process: %d, %s %s %s Operation Start \n",timestring, temp->processID, temp->metaHeadPtr->command, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->inOutArg);
        printf(" %s, Process: %d, %s %s %s Operation Start \n",timestring, temp->processID, temp->metaHeadPtr->command, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->inOutArg);
        runTimer(timeRunner);
        accessTimer(LAP_TIMER,timestring);
        fprintf( file," %s, Process: %d, %s %s %s Operation end \n",timestring, temp->processID, temp->metaHeadPtr->command, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->inOutArg);
        printf(" %s, Process: %d, %s %s %s Operation end \n",timestring, temp->processID, temp->metaHeadPtr->command, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->inOutArg);
      }
      else if( ( compareString(temp->metaHeadPtr->command, "dev") == STR_EQ ))
      {
        accessTimer(LAP_TIMER,timestring);
        fprintf( file, " %s, Process: %d, %s %sput Operation start \n",timestring, temp->processID, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->inOutArg);
        printf(" %s, Process: %d, %s %sput Operation start \n",timestring, temp->processID, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->inOutArg);
        pthread_attr_init(&attr);
        timeRunner =(temp->metaHeadPtr->intArg2)*(configPtr->ioCycleRate);
    
        pthread_create(&tid, &attr, runTimerDev, &timeRunner);
        pthread_join(tid, NULL);
        accessTimer(LAP_TIMER,timestring);
        fprintf( file, " %s, Process: %d, %s %sput Operation end \n",timestring, temp->processID, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->inOutArg);
        printf(" %s, Process: %d, %s %sput Operation end \n",timestring, temp->processID, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->inOutArg);
      }
      else
      {
        printf(" %s0, Process: %d, %s %s request ( %d, %d )\n",timestring, temp->processID, temp->metaHeadPtr->command, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->intArg2, temp->metaHeadPtr->intArg3);
        fprintf( file, "  %s, Process: %d, %s %s request ( %d, %d )\n",timestring, temp->processID, temp->metaHeadPtr->command, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->intArg2, temp->metaHeadPtr->intArg3);
        
      }
    }
  }
  else if( configPtr->logToCode == LOGTO_FILE_CODE )
  {
    if( compareString(temp->metaHeadPtr->strArg1, "start") != STR_EQ)
    {  

      if( compareString(temp->metaHeadPtr->command, "cpu") == STR_EQ )
      {
        timeRunner =(temp->metaHeadPtr->intArg2)*(configPtr->procCycleRate);
        accessTimer(LAP_TIMER,timestring);
        fprintf( file, "  %f, Process: %d, %s %s %s Operation Start \n",currentTime, temp->processID, temp->metaHeadPtr->command, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->inOutArg);
        runTimer(timeRunner);
        accessTimer(LAP_TIMER,timestring);
        fprintf( file,"  %f, Process: %d, %s %s %s Operation end \n",currentTime, temp->processID, temp->metaHeadPtr->command, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->inOutArg);
      }
      else if(compareString(temp->metaHeadPtr->command, "dev") == STR_EQ)
      {
        fprintf( file, "  %f, Process: %d, %s %sput Operation start \n",currentTime, temp->processID, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->inOutArg);
        fprintf( file, "  %f, Process: %d, %s %sput Operation end \n",currentTime, temp->processID, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->inOutArg);
      }
      else
      {
        fprintf( file, "  %s, Process: %d, %s %s request ( %d, %d )\n",timestring, temp->processID, temp->metaHeadPtr->command, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->intArg2, temp->metaHeadPtr->intArg3);
      }
    }
  }
  else
  {
    printf("Logging error\n");
  }
  

}





void* runTimerDev( void*  param )
{
  int paramInt = *(int*) param;
  runTimer(paramInt);
  return NULL;
}

bool verifyAccess( MemoryType *memPtr, PCBType *temp)
{
  printf("I get reached");
  bool verified = true;

  if( memPtr == NULL)
  {
    verified = false;
  }
  else
  {
    while( memPtr != NULL)
    {
      if(temp->metaHeadPtr->intArg2 < memPtr->lowerBound || temp->metaHeadPtr->intArg3 > memPtr->upperBound)
      {
        verified = false;
      }
      memPtr = memPtr->nextMem;
    }
  }

  return verified;

}


bool verifyAvailability( MemoryType *memPtr,  PCBType *temp)
{

  printf("I get reached");
  if( memPtr == NULL)
  {
    memPtr = addMem(memPtr, temp);
  }

  while( memPtr != NULL)
  {
    if( (temp->metaHeadPtr->intArg2 < memPtr->lowerBound && temp->metaHeadPtr->intArg3 > memPtr->lowerBound) || 
                                                   (temp->metaHeadPtr->intArg3 > memPtr->upperBound && temp->metaHeadPtr->intArg2 < memPtr->upperBound))
    {
      return false;
    }
    memPtr = memPtr->nextMem;
  }

  memPtr->nextMem = addMem(memPtr->nextMem, temp);

  return true;

}



MemoryType *addMem(MemoryType *memPtr, PCBType *temp)
{
  memPtr = (MemoryType *)malloc(sizeof(MemoryType));
  memPtr->lowerBound = temp->metaHeadPtr->intArg2;
  memPtr->upperBound = temp->metaHeadPtr->intArg3 + memPtr->lowerBound - 1;
  memPtr->totalMem = memPtr->lowerBound + memPtr->upperBound;
  memPtr->nextMem = NULL;

  return memPtr;
}


PCBType* popPCB(PCBType** head)
{
    if (*head == NULL)
    {
      return NULL;
    }

    PCBType* temp = *head;
    *head = (*head)->nextPCB;
    temp->nextPCB = NULL;
    return temp;
}

// Insert a process at the end of the list
void appendPCB(PCBType** head, PCBType* process)
{
    if (*head == NULL) 
    {
        *head = process;
        process->nextPCB = NULL;
    } 
    else 
    {
        PCBType* temp = *head;
        while (temp->nextPCB != NULL)
            temp = temp->nextPCB;
        temp->nextPCB = process;
        process->nextPCB = NULL;
    }
}

// FCFS-P
PCBType* getNextProcess_FCFSP(PCBType** head, PCBType* blockedQueue)
{
    // just use popPCB for FCFS preemptive
    return popPCB(head);
}

// SRTF-P
PCBType* getNextProcess_SRTFP(PCBType** head)
{
    PCBType *shortest = *head, *prev = NULL, *curr = *head;
    PCBType *prevShortest = NULL;

    if (*head == NULL)
    {
        return NULL;
    }

    while (curr != NULL) 
    {
        if (curr->processTime < shortest->processTime)
        {
            shortest = curr;
            prevShortest = prev;
        }
        prev = curr;
        curr = curr->nextPCB;
    }

    if (prevShortest != NULL) 
    {
        prevShortest->nextPCB = shortest->nextPCB;
    } 
    else 
    {
        *head = shortest->nextPCB;
    }
    shortest->nextPCB = NULL;
    return shortest;
}

// RR-P
PCBType* getNextProcess_RRP(PCBType** head, PCBType** rrQueueTail)
{
    PCBType* next = popPCB(head);
    if (next != NULL)
    {
        appendPCB(rrQueueTail, next); // move to end
    }

    return next;
}
