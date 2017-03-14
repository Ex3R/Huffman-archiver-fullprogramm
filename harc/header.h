#ifndef HEADER
#define HEADER
#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <sys\types.h>
#include <sys/stat.h>//для стата
#include "errorsAndWarnings.h"
#include <stdlib.h>

#define SizeOfBuf 1024
#define EXTENTION ".txt"
#define SIGNATURE 111
#define SIZE_SIGNATURE sizeof(unsigned int)
#define SIZE_CHECKSUM sizeof(unsigned short)
#define SIZE_LENGTHNAME sizeof(char)
#define SIZE_FLAGS sizeof(char)
#define SIZE_SIZE sizeof(unsigned __int64)
#define ZERO 0
#define DELETE "-d"
typedef struct {
	unsigned short checkSum;
	char lengthName;
	char name[256];
	char flags;// 0 -если не сжатый
	unsigned __int64 size;
} Info;

unsigned __int64 getSize(FILE* file);
char writeDataToFile(char *buf, FILE *fin, FILE *fout, unsigned short* crc);
char isEmptyFile(char* fileName);
int addFiles(char *archiveName, char **fileNames, int *amountOfFiles, Info **ptrOnStruct);
char checkUssd(char* archiveName, unsigned int ussd);
char* shortNameOnly(char* name);
void crc16(unsigned char * pcBlock, unsigned short len, unsigned short* crc);
void showInfo(char* archiveName);
char delete(char *archiveName, char *fileName, Info **ptrOnStruct);

#endif
