#include "header.h"
#include "errorsAndWarnings.h"
int main(int argc, char *argv[]) {
	setlocale(LC_ALL, "RUS");
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

		if (toggleSwitch(argv[1], argc, argv))
		{
			printf("[ERROR] Была введена неизвестная опция");
			return 0;
		}
	
	return 0;
	}
