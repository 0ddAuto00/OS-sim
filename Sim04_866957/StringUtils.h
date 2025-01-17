// Author: Fady Zaky

// Preprocessor directive
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

// header files
#include <stdbool.h>
#include <stdio.h>
#include "StandardConstants.h"

//Prototypes

bool getStringToLineEnd(FILE *inStream, char *capturedString);

bool getStringConstrained(FILE *inStream, bool clearLeadingNonprintable,
                          bool clearLeadingSpace, bool stopAtNonPrintable,
                          char delimiter, char *capturedString);

bool getStringToDelimiter(FILE *inStream, char delimiter, char *capturedString);

char toLowerCase(char testChar);

void setStrToLowerCase(char *destStr, const char *sourceStr);

int findSubString(const char *testStr, const char *searchSubStr);

void getSubString(char *destStr, const char *sourceStr,
                  int startIndex, int endIndex);

int compareString(const char *oneStr, const char *otherStr);

void concatenateString(char *destStrm, const char *sourceStr);

void copyString(char *destStr, const char *sourceStr);

int getStringLength(const char *testStr);

#endif // STRING_UTILS_H
