// Author: Fady Zaky

// header files
#include "datatypes.h"
#include "simulator.h"
#include "simtimer.c"
#include "stdbool.h"
#include "string.h"
#include "configops.h"
#include <pthread.h>
#include <string.h>

/*
 * Name: runSim
 * Process: primary Simulation driver
 * Function Input/Parameters: configuration data (ConfigDataType *)
 *                            metadata (OpCodeType *)
 * Function Output/Parameters: none
 * Function Output/Returned: none
 * Function Input/device: none
 * Function Output/Device: none
 * Dependencies: tbd
 */

    char timeRunning[8] = "0.000000";
    double quantumCycle;
    int wait = 0;
    int done = 0;
    bool timerlocked = 0;
    int loopCount = 0;
    bool blockedProcess[30];
    PCB processes[30];
    bool continueMain = 0;


void stringCat(char firstStr[], char secondStr[])
{
    
    int count1 = 0, count2 = 0;    
    while (firstStr[count1] != '\0')
    {

       firstStr[count2] = firstStr[count1];
       count1++;
       count2++;
    }


    count1 = 0;
    while (secondStr[count1] != '\0')
    {

       firstStr[count2] = secondStr[count1];
       count1++;
       count2++;
    }

    firstStr[count2] = '\0';
}


void *timeElapsed(void *value)
{
    double endTime = (double) quantumCycle;
    int pid = (int) processes[loopCount].pid;
    continueMain = 1;
    bool endOp = 0;
    //printf("-------------->%2.4f\n", endTime);

    blockedProcess[pid] = 1;
    while(!endOp)
    {
       if(!timerlocked)
       {
          if(accessTimer(LAP_TIMER, timeRunning) >= endTime)
          {
             endOp = 1;
          }
       }
    }
    blockedProcess[pid] = 0;

    wait--;
    return NULL;
}




void displaySimStart( ConfigDataType* configData, char* stringToWrite, char* stringToAdd, bool logToMon, bool logToBoth)
{

    if(logToMon || logToBoth)
    {

       printf(" %s, OS: Simulator Start\n", timeRunning);
    }

    if(!logToMon || logToBoth)
    {

       printf("\nSimulator running with output to file\n\n");
       storeConfigData(configData, stringToWrite);
       sprintf(stringToAdd, "\n %s, OS: Simulator Start\n", timeRunning);
       stringCat(stringToWrite, stringToAdd);
    }
}


void displayNewProcesses(PCB* processes, int count, char* stringToWrite, char* stringToAdd, bool logToMon, bool logToBoth)
{

    if(logToMon || logToBoth)
    {

       printf(" %s, OS: Process %d Set to READY state from NEW state\n", timeRunning, processes[count].pid);
    }

    if(!logToMon || logToBoth)
    {

       sprintf(stringToAdd, " %s, OS: Process %d Set to READY state from NEW state\n", timeRunning, processes[count].pid);
       stringCat(stringToWrite, stringToAdd);
    }
}


void displayMemoryInitialization(int processID, bool memInitialize, int memAvailable, char* stringToWrite, char* stringToAdd, bool logToMon, bool logToBoth)
{

    if(logToMon || logToBoth)
    {
       printf("--------------------------------------------\n");
       if(memInitialize)
       {
          printf("After memory initialization\n");
       }

       else
       {
          printf("After clear process %d success\n", processID);
       }

       printf("0 [ Open, P#: x, 0-0 ] %d\n", memAvailable);
       printf("--------------------------------------------\n");
    }

    if(!logToMon || logToBoth)
    {
       sprintf(stringToAdd, "--------------------------------------------\n");
       stringCat(stringToWrite, stringToAdd);
       if(memInitialize)
       {
          sprintf(stringToAdd, "After memory initialization\n");
       }

       else
       {
          sprintf(stringToAdd, "After clear process %d success\n", processID);
       }

       stringCat(stringToWrite, stringToAdd);
       sprintf(stringToAdd, "0 [ Open, P#: x, 0-0 ] %d\n", memAvailable);
       stringCat(stringToWrite, stringToAdd);
       sprintf(stringToAdd, "--------------------------------------------\n");
       stringCat(stringToWrite, stringToAdd);
    }
}


void displaySelectedProcess(PCB* processes, int count, char* stringToWrite, char* stringToAdd, bool logToMon, bool logToBoth)
{

    if(logToMon || logToBoth)
    {
       printf(" %s, OS: Process %d selected with %d ms remaining\n", timeRunning, processes[count].pid, processes[count].timeToExecute);
    }

    if(!logToMon || logToBoth)
    {
       sprintf(stringToAdd, " %s, OS: Process %d selected with %d ms remaining\n", timeRunning, processes[count].pid, processes[count].timeToExecute);
       stringCat(stringToWrite, stringToAdd);
    }
}



void displayProcessRunning(PCB* processes, int count, char* stringToWrite, char* stringToAdd, bool logToMon, bool logToBoth)
{
    if(logToMon || logToBoth)
    {
       printf(" %s, OS: Process %d set from READY to RUNNING\n\n", timeRunning, processes[count].pid);
    }	

    if(!logToMon || logToBoth)
    {
       sprintf(stringToAdd, " %s, OS: Process %d set from READY to RUNNING\n\n", timeRunning, processes[count].pid);
       stringCat(stringToWrite, stringToAdd);
    }
}



void displayProcessBlocked(PCB* processes, int count, char* stringToWrite, char* stringToAdd, bool logToMon, bool logToBoth, char* inOut)
{
    if(logToMon || logToBoth)
    {

       printf("\n %s, OS: Process %d blocked for %s operation\n", timeRunning, processes[count].pid, inOut);
       printf(" %s, OS: Process %d set from RUNNING to BLOCKED\n\n", timeRunning, processes[count].pid);
    }	

    if(!logToMon || logToBoth)
    {
       sprintf(stringToAdd, "\n %s, OS: Process %d blocked for %s operation\n", timeRunning, processes[count].pid, inOut);
       stringCat(stringToWrite, stringToAdd);
       sprintf(stringToAdd, " %s, OS: Process %d set from RUNNING to BLOCKED\n\n", timeRunning, processes[count].pid);
       stringCat(stringToWrite, stringToAdd);
    }
}




void displayOperationStart(bool memDisplay, bool processWord, PCB* processes, int count, char* inOut, OpCodeType* metaDataMstrPtr, char* stringToWrite, char* stringToAdd, bool logToMon, bool logToBoth)
{

    if(memDisplay)
    {

       if(logToMon || logToBoth)
       {
          printf(" %s, Process: %d, %s %s (%d, %d)\n", timeRunning, processes[count].pid, metaDataMstrPtr->command, metaDataMstrPtr->strArg1, metaDataMstrPtr->intArg2,
                     metaDataMstrPtr->intArg3);
       }

       if(!logToMon || logToBoth)
       {
          sprintf(stringToAdd, " %s, Process: %d, %s %s (%d, %d)\n", timeRunning, processes[count].pid, metaDataMstrPtr->command, metaDataMstrPtr->strArg1,
                     metaDataMstrPtr->intArg2, metaDataMstrPtr->intArg3);
          stringCat(stringToWrite, stringToAdd);
       }
    }

    else if(!processWord)
    {

       if(logToMon || logToBoth)
       {
          printf(" %s, Process: %d, %s %soperation start\n", timeRunning, processes[count].pid, metaDataMstrPtr->strArg1, inOut);
       }

       if(!logToMon || logToBoth)
       {
          sprintf(stringToAdd, " %s, Process: %d, %s %soperation start\n", timeRunning, processes[count].pid, metaDataMstrPtr->strArg1, inOut);
          stringCat(stringToWrite, stringToAdd);
       }
    }

    else
    {

       if(logToMon || logToBoth)
       {
           printf(" %s, Process: %d, cpu process %soperation start\n", timeRunning, processes[count].pid, inOut);
       }

       if(!logToMon || logToBoth)
       {
           sprintf(stringToAdd, " %s, Process: %d, cpu process %soperation start\n", timeRunning, processes[count].pid, inOut);
           stringCat(stringToWrite, stringToAdd);
       }
    }
}





void displayOperationEnd(bool memDisplay, bool processWord, PCB* processes, int loopCount, char* inOut, OpCodeType* processSpecificPtr, char* stringToWrite, char* stringToAdd, bool logToMon, bool logToBoth)
{

    if(!processWord && !memDisplay)
    {

       if(logToMon || logToBoth)
       {
          printf(" %s, Process: %d, %s %soperation end\n", timeRunning, processes[loopCount].pid, processSpecificPtr->strArg1, inOut);
       }

       if(!logToMon || logToBoth)
       {
                sprintf(stringToAdd, " %s, Process: %d, %s %soperation end\n", timeRunning, processes[loopCount].pid, processSpecificPtr->strArg1, inOut);
                stringCat(stringToWrite, stringToAdd);
       }
    }

    else if(!memDisplay)
    {

       if(logToMon || logToBoth)
       {
          printf(" %s, Process: %d, cpu process %soperation end\n", timeRunning, processes[loopCount].pid, inOut);
       }

       if(!logToMon || logToBoth)
       {			
          sprintf(stringToAdd, " %s, Process: %d, cpu process %soperation end\n", timeRunning, processes[loopCount].pid, inOut);
          stringCat(stringToWrite, stringToAdd);
       }
    }
}


void displayProcessEnd(PCB* processes, int loopCount, char* stringToWrite, char* stringToAdd, bool logToMon, bool logToBoth)
{

    if(logToMon || logToBoth)
    {
       printf("\n %s, OS: Process %d ended\n", timeRunning, processes[loopCount].pid);
    }

    if(!logToMon || logToBoth)
    {
       sprintf(stringToAdd, "\n %s, OS: Process %d ended\n", timeRunning, processes[loopCount].pid);
       stringCat(stringToWrite, stringToAdd);
    }
}


void displayProcessExit(PCB* processes, int loopCount, char* stringToWrite, char* stringToAdd, bool logToMon, bool logToBoth)
{

    if(logToMon || logToBoth)
    {
       printf(" %s, OS: Process %d set to EXIT\n", timeRunning, processes[loopCount].pid);
    }

    if(!logToMon || logToBoth)
    {
       sprintf(stringToAdd, " %s, OS: Process %d set to EXIT\n", timeRunning, processes[loopCount].pid);
       stringCat(stringToWrite, stringToAdd);
    }
}



void displayQuantumTimeout(int pid, char* stringToWrite, char* stringToAdd, bool logToMon, bool logToBoth)
{
    if(logToMon || logToBoth)
    {
        printf("\n %s, OS: Process %d quantum time out, cpu operation end\n\n", timeRunning, pid);
    }
    if(!logToMon || logToBoth)
    {
        sprintf(stringToAdd, "\n %s, OS: Process %d quantum time out, cpu operation end\n\n", timeRunning, pid);
        stringCat(stringToWrite, stringToAdd);
    }        
}




void orderBySJF(PCB* processes, int count)
{

    PCB temp;
    int loopCountOuter, loopCountInner;

    
    for(loopCountOuter = 0; loopCountOuter < count; loopCountOuter++)
    {

       for(loopCountInner = loopCountOuter + 1; loopCountInner < count; loopCountInner++)
       {

          if(processes[loopCountInner].timeToExecute < processes[loopCountOuter].timeToExecute)
          {

             temp = processes[loopCountOuter];
             processes[loopCountOuter] = processes[loopCountInner];
             processes[loopCountInner] = temp;
          }
       }
    }
}


bool equalStr(char* firstStr, char* secondStr, int charArrSize)
{

    int count;
    for(count = 0; count < charArrSize; count++)
    {

       if(firstStr[count] != secondStr[count])
       {
          return false;
       }

    }
    return true;
}


bool memManagement(int (*MMU)[2], int processNum, int memAvailable, int size, OpCodeType *processSpecificPtr, PCB* processes,
                                            bool memDisplay, char* stringToWrite, char* stringToAdd, bool logToMon, bool logToBoth)
{

    char allocate[8] = "allocate";
    char access[6] = "access";
    int loopCount = 0;
    bool allocation = true;
    bool accession = true;
    int BASE = 0, OFFSET = 1;
    int memUsed = 0;


    // Check for allocation or accession faults
    for(loopCount = 0; loopCount < size; loopCount++)
    {

       // 8 = size of allocate char array
       if(equalStr(processSpecificPtr->strArg1, allocate, 8))
       {
          
          if( ((processSpecificPtr->intArg2 + processSpecificPtr->intArg3 < MMU[loopCount][OFFSET])
                 && (processSpecificPtr->intArg2 + processSpecificPtr->intArg3 > MMU[loopCount][BASE]))
                 || ((processSpecificPtr->intArg2 > MMU[loopCount][BASE])
                 && (processSpecificPtr->intArg2 < MMU[loopCount][OFFSET])) 
                 || (processSpecificPtr->intArg2 + processSpecificPtr->intArg3 > memAvailable) )
          {
             allocation = false;
          }
       }

       // 6 = size of access char array
       if(equalStr(processSpecificPtr->strArg1, access, 6))
       {

          if( ((processSpecificPtr->intArg2 + processSpecificPtr->intArg3 > MMU[loopCount][OFFSET]))
                 || ((processSpecificPtr->intArg2 < MMU[loopCount][BASE]))
                 || (processSpecificPtr->intArg2 + processSpecificPtr->intArg3 > memAvailable) )
          {
             accession = false;
          }
       }


       memUsed += MMU[loopCount][OFFSET] - MMU[loopCount][BASE];

    }


    // 8 = size of allocate char array
       // Calculate memory used if currently set to mem allocate
    if(equalStr(processSpecificPtr->strArg1, allocate, 8))
    {
       size++;
       MMU[size-1][BASE] = processSpecificPtr->intArg2;
       MMU[size-1][OFFSET] = processSpecificPtr->intArg3 + MMU[size-1][BASE];
       memUsed += MMU[size - 1][OFFSET] - MMU[size - 1][BASE]; 
    }
    

    // Check if memUsed exceeds memAvailable
    if (memUsed > memAvailable)
    {
       allocation = false;
    }


    if((logToMon || logToBoth) && (memDisplay))
    {
       printf("--------------------------------------------\n");

       if(allocation && accession)
       {
          printf("Memory %s success\n", processSpecificPtr->strArg1);
       }

       else
       {
          printf("Memory %s failed\n", processSpecificPtr->strArg1);
       }

       printf("0 [ Used, P#: %d, %d-%d ] %d\n", processNum, MMU[size - 1][BASE], MMU[size - 1][OFFSET], MMU[size - 1][OFFSET] - MMU[size - 1][BASE]);
       printf("%d [ Open, P#: x, 0-0 ] %d\n", memUsed, memAvailable);
       printf("--------------------------------------------\n");
    }


    if((!logToMon || logToBoth) && (memDisplay))
    {
       sprintf(stringToAdd, "--------------------------------------------\n");
       stringCat(stringToWrite, stringToAdd);

       if(allocation && accession)
       {
          sprintf(stringToAdd, "Memory %s success\n", processSpecificPtr->strArg1);
       }

       else
       {
          sprintf(stringToAdd, "Memory %s failed\n", processSpecificPtr->strArg1);
       }

       stringCat(stringToWrite, stringToAdd);
       sprintf(stringToAdd, "0 [ Used, P#: %d, %d-%d ] %d\n", processNum, MMU[size - 1][BASE], MMU[size - 1][OFFSET], MMU[size - 1][OFFSET] - MMU[size - 1][BASE]);
       stringCat(stringToWrite, stringToAdd);
       sprintf(stringToAdd, "%d [ Open, P#: x, 0-0 ] %d\n", memUsed, memAvailable);
       stringCat(stringToWrite, stringToAdd);
       sprintf(stringToAdd, "--------------------------------------------\n");
       stringCat(stringToWrite, stringToAdd);
    }


    // Update timer
    timerlocked = 1;
    accessTimer(LAP_TIMER, timeRunning);
    timerlocked = 0;


    // Output memory segmentation fault
    if(!(allocation && accession))
    {

       if(logToMon || logToBoth)
       {
          printf(" %s, OS: Memory segmentation fault\n", timeRunning);
       }

       if(!logToMon || logToBoth)
       {
          sprintf(stringToAdd, " %s, OS: Memory segmentation fault\n", timeRunning);
          stringCat(stringToWrite, stringToAdd);
       }
    }


    return (allocation && accession);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void interruptManager(int count, char* stringToWrite, char* stringToAdd, bool logToMon, bool logToBoth)
{
   int innerLoop = 0;

   // Update timer
   timerlocked = 1;
   accessTimer(LAP_TIMER, timeRunning);
   timerlocked = 0;

   if(logToMon || logToBoth)
   {
      printf(" %s, OS: CPU idle, all active processes blocked\n", timeRunning);
   }

   if(!logToMon || logToBoth)
   {
      sprintf(stringToAdd, " %s, OS: CPU idle, all active processes blocked\n", timeRunning);
      stringCat(stringToWrite, stringToAdd);
   }
   while(wait >= count)
   {
      // wait for a process to unblock
   }


   // Update timer
   timerlocked = 1;
   accessTimer(LAP_TIMER, timeRunning);
   timerlocked = 0;

   while(blockedProcess[innerLoop])
   {

      innerLoop++;
   }

   if(logToMon || logToBoth)
   {
      printf(" %s, OS: Interrupted by process %d\n", timeRunning, innerLoop);
   }

   if(!logToMon || logToBoth)
   {
      sprintf(stringToAdd, " %s, OS: Interrupted by process %d\n", timeRunning, innerLoop);
      stringCat(stringToWrite, stringToAdd);
   }
}



void runSim(ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr)
{
    // Declaration of variables and arrays
    FILE * fptr;
    OpCodeType *processSpecificPtr[30];
    char displayString[STD_STR_LEN];
    char fcfs[STD_STR_LEN] = "FCFS";
    char sjf[STD_STR_LEN] = "SJF";
    char srtf[STD_STR_LEN] = "SRTF-P";
    char rr[STD_STR_LEN] = "RR-P";
    pthread_t thread;
    char* inOut;
    char* proc = "process";
    char stringToWrite[20000], stringToAdd[100];
    bool processWord;
    int cpuScheduling = 0;
    bool preemptive = 0;
    bool logToMon = true;
    bool logToBoth = true;
    bool memInitialize = true;
    bool memSegmentationFault = false;
    char monWord[7] = "Monitor";
    char bothWord[4] = "Both";
    char app[3] = "app";
    char start[5] = "start";
    char mem[3] = "mem";
    int count = 0;
    int memManagementUnit[10][2];
    int memSize = 0;
    OpCodeType *processesPtr = metaDataMstrPtr;
    bool memDisplay = configPtr->memDisplay;
    int memAvailable = configPtr->memAvailable - 1;
    double cpuProcTime[30];


    // Output Simulator start
    printf("Simulator Run\n");
    printf("-------------\n");



    // Check CPU Scheduling in metaData mdf file
    configCodeToString(configPtr->cpuSchedCode, displayString);

    // 4 = size of 'fcfs' char array, 3 = size of 'sjf' char array, 6 = size of 'srtf' char array, 4 = size of 'rr' char array
    if((!equalStr(displayString, fcfs, 4)) && (!equalStr(displayString, sjf, 3)) && (!equalStr(displayString, srtf, 6)) && (!equalStr(displayString, rr, 4)))
    {

       printf("\n--------------------------------------------");
       printf("\nNOTE: CPU SCHEDULE SELECTION HAS BEEN RECONFIGURED FROM '%s' TO 'FCFS-N'.\n", displayString);
       printf("--------------------------------------------\n\n");
       cpuScheduling = 0;
       preemptive = 0;
    }

    printf(" ----> %s\n", displayString);

    // 0 = FCFS, 1 = SJF, 2 = RR
       // 4 = size of 'fcfs' char array
    if(equalStr(displayString, fcfs, 4))
    {

       cpuScheduling = 0;
       if(displayString[5] == 'P')
       {
          preemptive = 1;
       }

    }

    // 6 = size of 'srtf' char array
    else if(equalStr(displayString, srtf, 6))
    {

       cpuScheduling = 1;
       if(displayString[5] == 'P')
       {
          preemptive = 1;
       }

    }

    else if(equalStr(displayString, sjf, 3))
    {

       cpuScheduling = 1;
       if(displayString[4] == 'P')
       {
          preemptive = 1;
       }

    }
    else if(equalStr(displayString, rr, 4))
    {

       cpuScheduling = 2;
       preemptive = 1;
    }



    // Start timer
    runTimer(99);



    // Update timer
    accessTimer(ZERO_TIMER, timeRunning);



    // Check whether to log on monitor, file, or both
    configCodeToString(configPtr->logToCode, displayString);

    // 7 = size of monWord, 4 = size of bothWord
    if(!equalStr(displayString, monWord, 7))
    {

       logToMon = false;
    }

    if(!equalStr(displayString, bothWord, 4))
    {

       logToBoth = false;
    }


	
    // Output according to what logTo states in the config file
    displaySimStart(configPtr, stringToWrite, stringToAdd, logToMon, logToBoth);



    // Loop through all the lines in metaData mdf file
    while(processesPtr != NULL)
    {

       // If pointer points at the words 'app start', indicating new process
          // 3 = size of char array 'app', 5 = size of char array 'start'
       if( (equalStr(processesPtr->command, app, 3)) && (equalStr(processesPtr->strArg1, start, 5)) )
       {

          processes[count].pid = count;
          processes[count].timeToExecute = 0;
          processSpecificPtr[count] = processesPtr->nextNode;
          blockedProcess[count] = 0;
          cpuProcTime[count] = 0;


          // Update timer
          accessTimer(LAP_TIMER, timeRunning);



          // Output according to what logTo states in config file
          displayNewProcesses(processes, count, stringToWrite, stringToAdd, logToMon, logToBoth);



          // Set metaData pointer to the current pointer
          if(count == 0)
          {

             metaDataMstrPtr = processesPtr->nextNode;
          }

          count++;
       }


       // Calculate execution time of current process pointed to
          // 3 = size of mem char array
       else if((count > 0) && (!equalStr(processesPtr->command, mem, 3)))
       {
          
          processes[count-1].timeToExecute = processes[count-1].timeToExecute + (processesPtr->intArg2 * configPtr->ioCycleRate);
       }
       


       // Move to next line in metaData mdf file
       processesPtr = processesPtr->nextNode;
    }


    // Check if memory display is set to on
    if(memDisplay) {


       // Output according to what logTo states in config file
          // We write 0 because no processes are defined yet
       displayMemoryInitialization(0, memInitialize, memAvailable, stringToWrite, stringToAdd, logToMon, logToBoth);
       memInitialize = false;
    }


    // Order processes by timeToExecute if cpu schedule is SJF
       // cpuScheduling->0 = FCFS, cpuScheduling->1 = SJF
    if(cpuScheduling == 1)
    {

       orderBySJF(processes, count);
    }


//===============================================================================================================================================================================================================
    // Loop through all the processes
    if(preemptive)
    {
 
       while(done < count)
       {
          if(wait >= count)
          {
             interruptManager(count, stringToWrite, stringToAdd, logToMon, logToBoth);
          }
          for(loopCount = 0; loopCount < count; loopCount++)
          {


           if(!blockedProcess[processes[loopCount].pid])
           {
                  // All timerlocked uses are to ensure there is no race condition occuring (still happens anyways, atleast no deadlocks)
             // Update timer
             timerlocked = 1;
             accessTimer(LAP_TIMER, timeRunning);
             timerlocked = 0;


             // Output according to what logTo states in config file
             if(cpuProcTime[processes[loopCount].pid] <= 0)
             {
                 displaySelectedProcess(processes, loopCount, stringToWrite, stringToAdd, logToMon, logToBoth);
             }


             // Update timer
             timerlocked = 1;
             accessTimer(LAP_TIMER, timeRunning);
             timerlocked = 0;


             // Output according to what logTo states in config file
             if(cpuProcTime[processes[loopCount].pid] <= 0)
             {
                 displayProcessRunning(processes, loopCount, stringToWrite, stringToAdd, logToMon, logToBoth);
             }

             if(!equalStr(processSpecificPtr[processes[loopCount].pid]->command, app, 3))
             {
                if(processSpecificPtr[processes[loopCount].pid]->inOutArg[0] == 'i')
                {

                    inOut = "input ";
                }

                else if(processSpecificPtr[processes[loopCount].pid]->inOutArg[0] == 'o')
                {

                   inOut = "output ";
                }
          
                else
                {

                   inOut = "";
                }



                // Check whether current operation is 'cpu process' or other
                   // 7 = size of 'proc' char array -> "process"
                processWord = equalStr(processSpecificPtr[processes[loopCount].pid]->strArg1, proc, 7);



                // Output according to what logTo states in config file
                displayOperationStart(equalStr(processSpecificPtr[processes[loopCount].pid]->command, mem, 3), processWord, processes, loopCount,
                                        inOut, processSpecificPtr[processes[loopCount].pid], stringToWrite, stringToAdd, logToMon, logToBoth);


                // Display process getting blocked if I/O operation is to be done
                if(cpuProcTime[processes[loopCount].pid] <= 0)
                {
                    displayProcessBlocked(processes, loopCount, stringToWrite, stringToAdd, logToMon, logToBoth, inOut);
                }


                // Calculate time needed for operation
                timerlocked = 1;
                processes[loopCount].quantumCycleStart = accessTimer(LAP_TIMER, timeRunning);
                timerlocked = 0;
                if(processWord)
                {
                   quantumCycle = processes[loopCount].quantumCycleStart  + (double) ((double)processSpecificPtr[processes[loopCount].pid]->intArg2 / ((double)1000) * configPtr->procCycleRate);
                }
                else
                {
                   quantumCycle = processes[loopCount].quantumCycleStart  + (double) ((double)processSpecificPtr[processes[loopCount].pid]->intArg2 / ((double)1000) * configPtr->ioCycleRate);
                }
                processes[loopCount].quantumCycleEnd = quantumCycle;



                // If the current operation is a cpu process operation
                if(processWord)
                {

                     // Calculate cpu process time
                     if(cpuProcTime[processes[loopCount].pid] <= 0)
                     {
                        cpuProcTime[processes[loopCount].pid] = processes[loopCount].quantumCycleEnd - processes[loopCount].quantumCycleStart;
                     }
                     


                     // Loop cpu process operation if cpu scheduling is FCFS-P
                     do
                     {

                        if(configPtr->quantumCycles * configPtr->procCycleRate < cpuProcTime[processes[loopCount].pid] * 1000)
                        {
                           quantumCycle = accessTimer(LAP_TIMER, timeRunning) + ((double)(configPtr->quantumCycles * configPtr->procCycleRate) / (double)1000);
                           while(accessTimer(LAP_TIMER, timeRunning) < quantumCycle)
                           {
                               
                           }
                           displayQuantumTimeout(processes[loopCount].pid, stringToWrite, stringToAdd, logToMon, logToBoth);
                           cpuProcTime[processes[loopCount].pid] -= ((double)(configPtr->quantumCycles * configPtr->procCycleRate) / (double)1000);
                        }
                        else
                        {

                           quantumCycle = accessTimer(LAP_TIMER, timeRunning) + cpuProcTime[processes[loopCount].pid];
                           while(accessTimer(LAP_TIMER, timeRunning) < quantumCycle)
                           {
                               
                           }

                           displayOperationEnd(equalStr(processSpecificPtr[processes[loopCount].pid]->command, mem, 3), processWord, processes, loopCount,
                                inOut, processSpecificPtr[processes[loopCount].pid], stringToWrite, stringToAdd, logToMon, logToBoth);
                           cpuProcTime[processes[loopCount].pid] = 0;
                        } 

                        if(cpuScheduling == 0)
                        {
                           displayOperationStart(equalStr(processSpecificPtr[processes[loopCount].pid]->command, mem, 3), processWord, processes, loopCount,
                                        inOut, processSpecificPtr[processes[loopCount].pid], stringToWrite, stringToAdd, logToMon, logToBoth);
                        }
                     } while((cpuScheduling == 0) && (cpuProcTime[processes[loopCount].pid] > 0));
                }


                // If the current operation is an I/O operation
                else if(!equalStr(processSpecificPtr[processes[loopCount].pid]->command, mem, 3))
                {
                         wait++;
                         continueMain = 0;
 
                         // Update ms remaining
                         processes[loopCount].timeToExecute = processes[loopCount].timeToExecute - (processSpecificPtr[processes[loopCount].pid]->intArg2 * configPtr->ioCycleRate);

                         // Run I/O in detached thread
                         pthread_create(&thread, NULL, timeElapsed, &quantumCycle);
                         pthread_detach(thread);


                         while(!continueMain)
                         {
                            // wait for critical section to finish
                         }
                }

                // Else perform memory management
                else
                {
                   memSegmentationFault = !memManagement(memManagementUnit, processes[loopCount].pid, memAvailable, memSize, processSpecificPtr[processes[loopCount].pid], processes,
                                                  memDisplay, stringToWrite, stringToAdd, logToMon, logToBoth);
             
      
                   memManagementUnit[memSize][0] = processSpecificPtr[processes[loopCount].pid]->intArg2;
                   memManagementUnit[memSize][1] = memManagementUnit[memSize][0] + processSpecificPtr[processes[loopCount].pid]->intArg3;
                   memSize++;
                }


                if(cpuProcTime[processes[loopCount].pid] <= 0)
                {
                    processSpecificPtr[processes[loopCount].pid] = processSpecificPtr[processes[loopCount].pid]->nextNode;

                }


                // If cpu scheduling is set as SJF-P
                if(cpuScheduling == 1)
                {
                   orderBySJF(processes, count);
                   loopCount = 0;
                }


             }



             if(equalStr(processSpecificPtr[processes[loopCount].pid]->command, app, 3))
             {

                // Output according to what logTo states in config file
                displayProcessExit(processes, loopCount, stringToWrite, stringToAdd, logToMon, logToBoth);



                blockedProcess[processes[loopCount].pid] = 1;
                done++;
             }

           }
          }

       }
    }
    else
   {
    for(loopCount = 0; loopCount < count; loopCount++)
    {

       // Update timer
       accessTimer(LAP_TIMER, timeRunning);


       // Output according to what logTo states in config file
       displaySelectedProcess(processes, loopCount, stringToWrite, stringToAdd, logToMon, logToBoth);


       // Update timer
       accessTimer(LAP_TIMER, timeRunning);


       // Output according to what logTo states in config file
       displayProcessRunning(processes, loopCount, stringToWrite, stringToAdd, logToMon, logToBoth);


       // Loop while metaData mdf points at operations of process
          // 3 = size of 'app' char array
       while(!equalStr(processSpecificPtr[processes[loopCount].pid]->command, app, 3))
       {


          // Update timer
          accessTimer(LAP_TIMER, timeRunning);


          // Check operation argument (in or out)
          if(processSpecificPtr[processes[loopCount].pid]->inOutArg[0] == 'i')
          {

             inOut = "input ";
          }

          else if(processSpecificPtr[processes[loopCount].pid]->inOutArg[0] == 'o')
          {

             inOut = "output ";
          }
          
          else
          {

             inOut = "";
          }



          // Check whether current operation is 'cpu process' or other
             // 7 = size of 'proc' char array -> "process"
          processWord = equalStr(processSpecificPtr[processes[loopCount].pid]->strArg1, proc, 7);



          // Output according to what logTo states in config file
          displayOperationStart(equalStr(processSpecificPtr[processes[loopCount].pid]->command, mem, 3), processWord, processes, loopCount,
                                  inOut, processSpecificPtr[processes[loopCount].pid], stringToWrite, stringToAdd, logToMon, logToBoth);		



          // Calculate when process/io operation will stop

          processes[loopCount].quantumCycleStart = accessTimer(LAP_TIMER, timeRunning);
          if(processWord)
          {
             quantumCycle = processes[loopCount].quantumCycleStart  + (double) ((double)processSpecificPtr[processes[loopCount].pid]->intArg2 / ((double)1000) * configPtr->procCycleRate);
          }
          else
          {
             quantumCycle = processes[loopCount].quantumCycleStart  + (double) ((double)processSpecificPtr[processes[loopCount].pid]->intArg2 / ((double)1000) * configPtr->ioCycleRate);
          }
          processes[loopCount].quantumCycleEnd = quantumCycle;



          // Run process operation in thread if not memory allocation/accession
             // 3 = size of mem char array
          if(!equalStr(processSpecificPtr[processes[loopCount].pid]->command, mem, 3))
          {
                   pthread_create(&thread, NULL, timeElapsed, &processes[loopCount].quantumCycleEnd);
                   pthread_join(thread, NULL);
          }

          // Else perform memory management
          else
          {
             memSegmentationFault = !memManagement(memManagementUnit, processes[loopCount].pid, memAvailable, memSize, processSpecificPtr[processes[loopCount].pid], processes,
                                            memDisplay, stringToWrite, stringToAdd, logToMon, logToBoth);
             

             memManagementUnit[memSize][0] = processSpecificPtr[processes[loopCount].pid]->intArg2;
             memManagementUnit[memSize][1] = memManagementUnit[memSize][0] + processSpecificPtr[processes[loopCount].pid]->intArg3;
             memSize++;
          }


          // Output operation end
             // Output according to what logTo states in config file
          displayOperationEnd(equalStr(processSpecificPtr[processes[loopCount].pid]->command, mem, 3), processWord, processes, loopCount,
                                inOut, processSpecificPtr[processes[loopCount].pid], stringToWrite, stringToAdd, logToMon, logToBoth);


          // Point to next line in metaData mdf file
          processSpecificPtr[processes[loopCount].pid] = processSpecificPtr[processes[loopCount].pid]->nextNode;


          // Break while loop (AKA end process) if memory segmentation fault occured
          if (memSegmentationFault)
          {
             break;
          }

       }
      


       // Update timer
       accessTimer(LAP_TIMER, timeRunning);


       // Output according to what logTo states in config file
       displayProcessEnd(processes, loopCount, stringToWrite, stringToAdd, logToMon, logToBoth);
       // Reset memory size
       memSize = 0;



       // Check if memory display is set to on
       if(configPtr->memDisplay)
       {

          // Output according to what logTo states in config file
          displayMemoryInitialization(processes[loopCount].pid, memInitialize, memAvailable, stringToWrite, stringToAdd, logToMon, logToBoth);
       }



       // Update timer
       accessTimer(LAP_TIMER, timeRunning);



       // Output according to what logTo states in config file
       displayProcessExit(processes, loopCount, stringToWrite, stringToAdd, logToMon, logToBoth);



       // Jump two lines in the metaData mdf file, to the next process
       metaDataMstrPtr = metaDataMstrPtr->nextNode;
       metaDataMstrPtr = metaDataMstrPtr->nextNode;
    }



     // Update timer
     accessTimer(LAP_TIMER, timeRunning);



     // Output according to what logTo states in config file
     if(logToMon || logToBoth)
     {

        printf(" %s, OS: System stop\n", timeRunning);

        if(configPtr->memDisplay)
        {

           printf("--------------------------------------------\n");
           printf("After clear all process success\n");
           printf("No memory configured\n");
           printf("--------------------------------------------\n");
       }
    }
   }

	

    // Update timer
    accessTimer(LAP_TIMER, timeRunning);

	
    // Output according to what logTo states in config file
    if(logToMon || logToBoth)
    {
		printf(" %s, OS: Simulator End\n", timeRunning);
    }


    if(!logToMon)
    {
       fptr = fopen(configPtr->logToFileName, "w");
       fputs(stringToWrite, fptr);
       fclose(fptr);
    }
}
