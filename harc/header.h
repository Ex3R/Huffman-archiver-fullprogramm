#ifndef HEADER
#define HEADER
#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <sys\types.h>
#include <sys/stat.h>
#include "errorsAndWarnings.h"
#include <stdlib.h>
#include <time.h>
#include <io.h>
#define LENGTH_TMP_NAME 20
#define SizeOfBuf 1024
#define EXTENTION ".txt"
#define SIGNATURE 111
#define CRC 0xFFFF
#define SIZE_SIGNATURE sizeof(unsigned int)
#define SIZE_CHECKSUM sizeof(unsigned short)
#define SIZE_LENGTHNAME sizeof(char)
#define SIZE_FLAGS sizeof(char)
#define SIZE_SIZE sizeof(UINT64)
#define ZERO 0
#define DELETE "-d"
#define INTEGRITYCHECK "-t"
/*size*/
#define BYTE "Bytes"
#define KB "KB"
#define MB "MB"
#define GB "GB"
/***********************************************For huffman's algorithm****************************************************************************/
#define MAKELIST 0
#define MAKETREE 1
#define COMPRESSED 1
#define NOTCOMPRESSED 0
#define NODE -1
/*Ограничение до которого файл будет просто переписываться без сжатия*/
#define LIMIT_FOR_COMPRESSION 50
/*for accsess*/
#define ACCESSDENIED																	\
{																						\
	printf("В каталоге уже имеется файл с таким названием и он закрыт для записи:(");	\
	return 0;																			\
}

#define READING 4
#define WRITING 2
#define RANDW 6
#define EXIST 0
/*for func computeCRC*/
#define WITHSHIFT 1
#define NOSHIFT 0
typedef unsigned __int64 UINT64;
struct _stat64 size;
typedef struct Tree
{
	int symbol;
	UINT64 count;
	struct Tree *next, *left, *right;
} Tree;
/*compression*/
void createFrequencyArr(FILE *file, UINT64 *arr, UINT64 size);
void insert(Tree **head, UINT64 count, int symbol, char mode);
int makeHuffmanTree(Tree **head);
void CodeTable(Tree *root, char codes[256][256], char vrm[256],int flag);
void CharToString(char *SymBuf, char c);
UINT64 writeBits(FILE *file, int *position, unsigned char *buffer, char *value, unsigned short *crc);
void WriteTree(Tree* root, unsigned char *buffer, int *position, FILE *outputFile,unsigned short *crc, int flag);
UINT64 writeData(char codes[256][256], int *position, unsigned char *buffer, FILE *inputFile, FILE *outputFile, UINT64 size, unsigned short *crc);
void encode(FILE *inputFile, FILE *outputFile, UINT64 fileSize, unsigned short *crc);
void deleteNode(Tree* root);
/*decompression*/
char read_bit(FILE* in);
unsigned char read_char(FILE* in);
Tree *createNode(FILE *inputFile);
void decode(FILE *inputFile, FILE *outputFile, unsigned short *crc, UINT64 size);
/*************************************************************************************************************************************************/
typedef struct {
	unsigned short checkSum;
	char lengthName;
	char name[256];
	char flags;// 0 -если не сжатый
	char compression;
	UINT64 size;
} Info;
/*Список файлов для добавления/извлечения/удаления*/
typedef struct List {
	char *file;
	struct List *next;
} List;
void printHelp();
int toggleSwitch(char* operation, int amount, char *param[]);
//операции со списком
void adding(List **head, char *fileName);
void printLinkedList(List *head);
makeListOfFiles(int argc, char* argv[], List **head);
int deleteByValue(List **head, char *fileName);
//Вспомогательные функции архиватора
int fileExists(char * filename);
int accessRights(char *fileName, int mode);
char *uniqName();
UINT64 getSize(FILE* file);
char writeDataToFile(char *buf, FILE *fin, FILE *fout, unsigned short* crc, UINT64 amount);
char isEmptyFile(char* fileName);
char* shortNameOnly(char* name);
char compressOrNot(UINT64 size);
double compressionRatio(double firstSize, double lastSize);
void crc16(unsigned char * pcBlock, unsigned short len, unsigned short* crc);
char checkUssd(char* archiveName);
void computeCRC(char *buf, FILE *inputFile, unsigned short *crc, UINT64 size, char withShift);
//Основные функции архиватора
int addFiles(char *archiveName, char **fileNames, int *amountOfFiles, Info **ptrOnStruct);
char delete(char *archiveName, char *fileName, Info **ptrOnStruct);
void showInfo(char* archiveName, Info **ptrOnStruct);
char integrityСheck(char *archiveName, Info **ptrOnStruct, char **file);
int extractFiles(List **listOfFiles, FILE *archive);
#endif
