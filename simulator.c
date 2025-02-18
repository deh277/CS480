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
  //while op codes are available
  while( PCBptr != NULL)
  {

    //get process
      //scheduler( SJF and FCFS )
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
      logHandling( PCBptr, file , currentTime, configPtr);
      
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





void logHandling( PCBType *PCBptr, FILE* file, double currentTime, ConfigDataType *configPtr)
{
  PCBType *temp = PCBptr;
  char timestring[20];
  int timeRunner;
  pthread_t tid = PCBptr->processID;
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
      else
      {
        timeRunner =(temp->metaHeadPtr->intArg2)*(configPtr->ioCycleRate);

        printf("  %s, Process: %d, %s %sput Operation start \n",timestring, temp->processID, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->inOutArg);
        printf("  %s, Process: %d, %s %sput Operation end \n",timestring, temp->processID, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->inOutArg);
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
      else
      {
        pthread_attr_init(&attr);
        timeRunner =(temp->metaHeadPtr->intArg2)*(configPtr->ioCycleRate);
        pthread_create(&tid, &attr, runTimerDev, timestring);
        pthread_join(tid, NULL);
        fprintf( file, " %s, Process: %d, %s %sput Operation start \n",timestring, temp->processID, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->inOutArg);
        printf(" %s, Process: %d, %s %sput Operation start \n",timestring, temp->processID, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->inOutArg);
        pthread_attr_init(&attr);
        timeRunner =(temp->metaHeadPtr->intArg2)*(configPtr->ioCycleRate);
        pthread_create(&tid, &attr, runTimerDev, timestring);
        pthread_join(tid, NULL);
        fprintf( file, " %s, Process: %d, %s %sput Operation end \n",timestring, temp->processID, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->inOutArg);
        printf(" %s, Process: %d, %s %sput Operation end \n",timestring, temp->processID, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->inOutArg);
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
      else
      {
        fprintf( file, "  %f, Process: %d, %s %sput Operation start \n",currentTime, temp->processID, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->inOutArg);
        fprintf( file, "  %f, Process: %d, %s %sput Operation end \n",currentTime, temp->processID, temp->metaHeadPtr->strArg1, temp->metaHeadPtr->inOutArg);
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
  struct timeval startTime, endTime;
  int startSec, startUSec, endSec, endUSec;
  int uSecDiff, mSecDiff, secDiff, timeDiff;

  gettimeofday( &startTime, NULL );

  startSec = startTime.tv_sec;
  startUSec = startTime.tv_usec;

  timeDiff = 0;

  while( timeDiff > 0)
     {
      gettimeofday( &endTime, NULL );

      endSec = endTime.tv_sec;
      endUSec = endTime.tv_usec;
      uSecDiff = endUSec - startUSec;

      if( uSecDiff < 0 )
         {
          uSecDiff = uSecDiff + 1000000;

          endSec = endSec - 1;
         }

      mSecDiff = uSecDiff / 1000;
      secDiff = ( endSec - startSec ) * 1000;
      timeDiff = secDiff + mSecDiff;
     }

     return param;
 }


