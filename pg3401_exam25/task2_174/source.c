#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "source.h"

#pragma pack(push, 1)
struct TASK2_FILE_METADATA
{
    char szFileName[32];
    int iFileSize;
    char byHash[4];
    int iSumOfChars;
    char aAlphaCount[26];
};
#pragma pack(pop)

struct TASK2_FILE_METADATA metadata = {0};

int readFile()
{
    char *filename = "pgexam25_test.txt";
    FILE *file;

    // Not really neccecary as we are hardcoding the filename, but could be useful if we were taking an argument
    if (strlen(filename) >= sizeof(metadata.szFileName))
    {
        printf("Error: Input filename is too long (max %zu characters)\n", sizeof(metadata.szFileName) - 1);
        return 1;
    }

    file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: Could not open file %s\n", filename);
        return 1;
    }

    strcpy(metadata.szFileName, filename);
    Task2_SizeAndSumOfCharacters(file, &metadata.iFileSize, &metadata.iSumOfChars);
    unsigned int hash = 0;
    Task2_SimpleDjb2Hash(file, &hash);
    memcpy(metadata.byHash, &hash, sizeof(metadata.byHash));
    Task2_CountEachCharacter(file, metadata.aAlphaCount);
    fclose(file);

    return 0;
}

int writeOutput()
{
    char *fileOutputName = "pgexam25_output.bin";
    FILE *outputFile;
    int output;
    outputFile = fopen(fileOutputName, "wb");
    if (outputFile == NULL)
    {
        printf("Error: Could not create output file %s\n", fileOutputName);
        return 1;
    }

    output = fwrite(&metadata, sizeof(struct TASK2_FILE_METADATA), 1, outputFile);
    fclose(outputFile);

    if (output != 1)
    {
        printf("Error: Failed to write data to output file\n");
        return 1;
    }

    printf("Saved output to file %s\n", fileOutputName);
    return 0;
}

int main()
{
    int result;

    result = readFile();
    if (result != 0)
    {
        return result;
    }

    result = writeOutput();
    return result;
}
