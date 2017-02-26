#include "header.h"
#include "errorsAndWarnings.h"
int main(int argc, char *argv[]) {
	setlocale(LC_ALL, "RUS");
	Info *ptrOnStruct = NULL; //указатель на структуру
	if ((ptrOnStruct = (Info*)malloc(sizeof(Info))) == NULL)//выделение памяти под структуру
		ALLOC_MEMORY_ERR

		//addFiles("output/output.txt", "data/input2.txt", &ptrOnStruct, 1);
		//showInfo("output/output.txt");
	checkUssd("output/output.txt",111);
	//отчистка памяти под структуру, которую мы используем многократно
	free(ptrOnStruct);


	return 0;
	}
