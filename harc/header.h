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
#define JOIN(i) output/ ## i
#define PATH "output/"
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
/*For huffman's algorithm*/
#define MAKELIST 0
#define MAKETREE 1
#define COMPRESSED 1
#define NOTCOMPRESSED 0
#define NODE -1
#define LIMIT_FOR_COMPRESSION 50
typedef unsigned __int64 UINT64;
struct _stat64 size;
typedef struct Tree
{
	int symbol;
	UINT64 count;
	struct Tree *next, *left, *right;
} Tree;
/*compression*/
void deleteNode(Tree* root);
void createFrequencyArr(FILE *file, UINT64 *arr, UINT64 size);
void insert(Tree **head, UINT64 count, int symbol, char mode);
void makeHuffmanTree(Tree **head);
void CodeTable(Tree *root, char codes[256][256], char vrm[256]);
void CharToString(char *SymBuf, char c);
UINT64 writeBits(FILE *file, int *position, unsigned char *buffer, char *value, unsigned short *crc);
void WriteTree(Tree* root, unsigned char *buffer, int *position, FILE *outputFile,unsigned short *crc);
UINT64 writeData(char codes[256][256], int *position, unsigned char *buffer, FILE *inputFile, FILE *outputFile, UINT64 size, unsigned short *crc);
void encode(FILE *inputFile, FILE *outputFile, UINT64 fileSize, unsigned short *crc);
/*decompression*/
char read_bit(FILE* in);
unsigned char read_char(FILE* in);
Tree *createNode(FILE *inputFile);
void decode(FILE *inputFile, FILE *outputFile, unsigned short *crc);
/************************************/
typedef struct {
	unsigned short checkSum;
	char lengthName;
	char name[256];
	char flags;// 0 -если не сжатый
	char compression;
	UINT64 size;
} Info;

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
//добавление
int fileExists(char * filename);
int accessRights(char *fileName);
char *uniqName();
UINT64 getSize(FILE* file);
char writeDataToFile(char *buf, FILE *fin, FILE *fout, unsigned short* crc, UINT64 amount);
char isEmptyFile(char* fileName);
char* shortNameOnly(char* name);
char compressOrNot(UINT64 size);
double compressionRatio(double firstSize, double lastSize);
int addFiles(char *archiveName, char **fileNames, int *amountOfFiles, Info **ptrOnStruct);
void crc16(unsigned char * pcBlock, unsigned short len, unsigned short* crc);
//извлечение и удаление
char checkUssd(char* archiveName);
char delete(char *archiveName, char *fileName, Info **ptrOnStruct);
//вывод информации о файлах
void showInfo(char* archiveName, Info **ptrOnStruct);
//проверка архива на целостность
char integrityСheck(char *archiveName, Info **ptrOnStruct, char **file);
char readDataFromFile(char *buf, FILE *fin, unsigned short* crc, UINT64 amount);
#endif
