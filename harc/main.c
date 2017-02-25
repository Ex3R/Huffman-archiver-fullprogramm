#include "header.h"
#include "errorsAndWarnings.h"
int main(int argc, char *argv[]) {
	setlocale(LC_ALL, "RUS");
	FILE *fin  = NULL, *fout = NULL;
	Info *ptrOnStruct = NULL; //указатель на структуру
	if ((ptrOnStruct = (Info*)malloc(sizeof(Info))) == NULL)//выделение памяти под структуру
		ALLOC_MEMORY_ERR

		//addFiles("output.txt", "input2.txt", &ptrOnStruct, fin, fout, 1);
		showInfo("output.txt");

	//отчистка памяти под структуру, которую мы используем многократно
	free(ptrOnStruct);


	return 0;
	}
/*if ((argc != 3) || (!strcmp(argv[1], "help"))) {
printf("Description");
return 1;
}*/
