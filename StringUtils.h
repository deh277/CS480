#ifndef STRING_UTILS_H
#define STRING_UTILS_H


#include <stdbool.h>
#include <stdio.h>
#include "StandardConstants.h"

int compareString( const char *oneStr, const char *otherStr );


void concantenateString( char *destStr, const char *sourceStr);


void copyString( char *deststr, const char *sourceStr);


int findSubString( const char *testStr, const char *searchSubStr );


bool getStringConstrained(FILE *inStream, bool clearLeadingNonPrintable, 
                                          bool clearLeadingSpace, bool stopAtNonPrintable, 
                                                         char delimiter, char *capturedstring);
                                                         
                                                         
int getStringLength( const char *testStr );


bool getStringToDelimiter(FILE *inStream, char delimiter, char *capturedString);


bool getStringToLineEnd(FILE *inStream, char *capturedString);


void getSubString( char *destStr, const char *sourceStr, int startIndex, int endIndex);


void setStrToLowerCase( char *destStr, const char *sourceStr );


char toLowerCase( char testChar );



#endif
