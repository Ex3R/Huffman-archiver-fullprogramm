#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <sys\types.h>
#include <sys/stat.h>//для стата
#include "errorsAndWarnings.h"

#define SizeOfBuf 1024

/*заголовок(шапка)*/
typedef struct {
	unsigned short checkSum;
	char lengthName;
	char name[256];
	char flags;// 0 -если не сжатый
	unsigned int size;
} Info;
//char** checkValidFiles(int amount, char * param[], int* strcount); не рабочая
int isEmptyFile(char* fileName);
int addFiles(char *archiveName, char **fileNames, int *amountOfFiles, Info **ptrOnStruct);
int checkUssd(char* archiveName, const unsigned int ussd);
char* makeNameShorter(char* name);
unsigned short crc16(unsigned char * pcBlock, unsigned short len);
void showInfo(char* archiveName);
