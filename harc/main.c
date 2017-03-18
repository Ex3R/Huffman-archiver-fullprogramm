#include "header.h"
int main(int argc, char *argv[]) {
	setlocale(LC_ALL, "RUS");
		if (toggleSwitch(argv[1], argc, argv))
		{
			printf("[ERROR:] Была введена неизвестная опция\n");
			return 0;
		}
	return 0;
	}
//удалить infoAboutFiles в начале работы программы
//Info *ptrOnStruct = NULL; //указатель на структуру
//if ((ptrOnStruct = (Info*)malloc(sizeof(Info))) == NULL)//выделение памяти под структуру
//	ALLOC_MEMORY_ERR

//addFiles("output/output.txt", "data/input2.txt", &ptrOnStruct, 1);
//showInfo("output/output.txt");
//checkUssd("output/output.txt",111);
//int* strcount = (int *)malloc(sizeof(int));

//char** s= checkValidFiles(argc, argv, strcount);
//int k = *strcount;
/*unsigned short crc = 0xFFFF;
FILE *in, *out;
if ((in = fopen("data/input1.txt", "rb")) == NULL)
OPEN_ERR
int m= ftell(in);
if ((out = fopen("data/out.txt", "wb")) == NULL)
OPEN_ERR

ftell(out);
char *buffer = NULL;
if (!(buffer = (char*)malloc(SizeOfBuf)))
ALLOC_MEMORY_ERR

writeDataToFile(buffer,in,out, &crc);
m = ftell(out);
m = fseek(out, -2, SEEK_CUR);
m = ftell(out);

free(buffer);*/

/*char *TMP2name = malloc(7);
printf("%s",tmpName(argv[0],&TMP2name, length));*/

//List *head = NULL;
///*for (int i=0;i<argc;i++)
//	adding(&head, argv[i]);*/
//makeListOfFiles(argc, argv, &head);
//printLinkedList(head);
//for (int i = 3; i<argc; i++)
//	deleteByValue(&head, argv[i]);

/*printf("[WARNING:]В архиве сожержится лишь сигнатура, хотите удалить её? Y/N");
char answer;
answer = getchar();
printf("%c", answer);
if (answer == 'Y') printf("Y");
else printf("N");*/