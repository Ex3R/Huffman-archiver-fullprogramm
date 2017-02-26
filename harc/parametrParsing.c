#include "header.h"
void basicCheckParam(int count, char *param[])
{
	if (count < 3 || (!strcmp(param[1], "help")))
	{
		printf("Программа архиватор");
		//вставить код, копирующий из текстового файла будущее описание программы
		return 0;
	}
}


//где operation - argv[1];
//0 -без ошибок
//1- знак ошибки
int toggleSwitch(char* operation, int amountOfParam, char *param[])
{
	if (amountOfParam < 2)
	{
		printf("Число входных параметров не может быть меньше 2\n");
		return 1;
	}

	//поместить файл(ы) в архив
	if (!strcmp(operation, "-a"))
	{
		printf("1\n");
		return 0;
	}

	//извлечь файл(ы) из архива
	if (!strcmp(operation, "-x"))
	{
		printf("1\n");
		return 0;
	}

	//вывести информацию о файлах
	if (!strcmp(operation, "-l"))
	{
		if (amountOfParam != 3)
		{
			printf("Неверное количество параметров для опции -l");
			return 1;
		}
		
		//проверка сигнатуры и расширения
		if (!checkUssd)
		return 0;
	}
	//удалить файл из архива
	if (!strcmp(operation, "-d"))
	{
		printf("1\n");
		return 0;
	}

	//проверить целостность архива
	if (!strcmp(operation, "-t"))
	{
		printf("1\n");
		return 0;
	}

	//вывести help

	if (!strcmp(operation, "-help"))
	{
		printf("1\n");
		return 0;
	}
	return 1;
}