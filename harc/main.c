#include "header.h"
#include "errorsAndWarnings.h"
int main(int argc, char *argv[]) {
	setlocale(LC_ALL, "RUS");
	//удалить infoAboutFiles в начале работы программы
	Info *ptrOnStruct = NULL; //указатель на структуру
	if ((ptrOnStruct = (Info*)malloc(sizeof(Info))) == NULL)//выделение памяти под структуру
		ALLOC_MEMORY_ERR

		//addFiles("output/output.txt", "data/input2.txt", &ptrOnStruct, 1);
		//showInfo("output/output.txt");
		//checkUssd("output/output.txt",111);

		//toggleSwitch(argv[1], argc, &argv);

	if (argc < 2)
	{
		printf("Число входных параметров не может быть меньше 2\n");
		return 1;
	}

	//поместить файл(ы) в архив
	if (!strcmp(argv[1], "-a"))
	{
		printf("1\n");
		return 0;
	}

	//извлечь файл(ы) из архива
	if (!strcmp(argv[1], "-x"))
	{
		printf("1\n");
		return 0;
	}

	//вывести информацию о файлах
	if (!strcmp(argv[1], "-l"))
	{
		if (argc != 3)
		{
			printf("Неверное количество параметров для опции -l");
			return 1;
		}

		//проверка сигнатуры и расширения
		if (!checkUssd(argv[2],111))
		{
			showInfo(argv[2]);
		}
			return 0;
	}
	//удалить файл из архива
	if (!strcmp(argv[1], "-d"))
	{
		printf("1\n");
		return 0;
	}

	//проверить целостность архива
	if (!strcmp(argv[1], "-t"))
	{
		printf("1\n");
		return 0;
	}

	//вывести help

	if (!strcmp(argv[1], "-help"))
	{
		printf("1\n");
		return 0;
	}
	//если команда неизвестна
	if (1)
	{
		printf("Неизвестная опция");
		return 0;
	}

	//отчистка памяти под структуру, которую мы используем многократно
	free(ptrOnStruct);
	return 0;
	}
