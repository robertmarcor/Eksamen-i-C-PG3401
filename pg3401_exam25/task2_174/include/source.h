#ifndef SOURCE_H
#define SOURCE_H

#include <stdio.h>

int Task2_SizeAndSumOfCharacters(FILE *fFileDescriptor, int *piSizeOfFile, int *piSumOfChars);
int Task2_SimpleDjb2Hash(FILE *fFileDescriptor, unsigned int *piHash);
int Task2_CountEachCharacter(FILE *fFileDescriptor, char aCountArray[26]);

#endif