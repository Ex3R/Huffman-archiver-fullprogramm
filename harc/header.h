#ifndef HEADER
#define HEADER
#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <sys\types.h>
#include <sys/stat.h>//äëÿ ñòàòà
#include "errorsAndWarnings.h"
#include <stdlib.h>

#define SizeOfBuf 1024
#define EXTENTION ".txt"
#define SIGNATURE 111
#define CRC 0xFFFF
#define SIZE_SIGNATURE sizeof(unsigned int)
#define SIZE_CHECKSUM sizeof(unsigned short)
#define SIZE_LENGTHNAME sizeof(char)
#define SIZE_FLAGS sizeof(char)
#define SIZE_SIZE sizeof(unsigned __int64)
#define ZERO 0
#define DELETE "-d"
#define INTEGRITYCHECK "-t"
typedef struct {
	unsigned short checkSum;
	char lengthName;
	char name[256];
	char flags;// 0 -åñëè íå ñæàòûé
	unsigned __int64 size;
} Info;

typedef struct List {
	char *file;
	struct List *next;
} List;
void adding(List **head, char *fileName);
void printLinkedList(List *head);
makeListOfFiles(int argc, char* argv[], List **head);
int  deleteByValue(List **head, char *fileName);

unsigned __int64 getSize(FILE* file);
char writeDataToFile(char *buf, FILE *fin, FILE *fout, unsigned short* crc, unsigned __int64 amount);
char isEmptyFile(char* fileName);
int addFiles(char *archiveName, char **fileNames, int *amountOfFiles, Info **ptrOnStruct);
char checkUssd(char* archiveName, unsigned int ussd);
char* shortNameOnly(char* name);
void crc16(unsigned char * pcBlock, unsigned short len, unsigned short* crc);
void showInfo(char* archiveName);
char delete(char *archiveName, char *fileName, Info **ptrOnStruct);
char integrityÑheck(char *archiveName, Info **ptrOnStruct, char* file);
char *tmpName(char* NameHHH, int length);
char readDataFromFile(char *buf, FILE *fin, unsigned short* crc, unsigned __int64 amount);

#endif
