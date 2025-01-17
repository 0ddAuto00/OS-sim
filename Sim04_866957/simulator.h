// Author: Fady Zaky

// Avoid compiling multiple times
#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stdbool.h>


typedef struct {
	int pid;
	int timeToExecute;
	double quantumCycleStart;
	double quantumCycleEnd;
        bool wait;
} PCB;


typedef struct {
	OpCodeType *processesPtr;
         bool memDisplay;
         int memAvailable;
         ConfigDataType *configPtr;
} threadVariables;


// function prototypes
void runSim(ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr);

void stringCat(char firstStr[], char secondStr[]);

bool equalStr(char* a, char* b, int size);
 

#endif // SIMULATOR_H
