#include "header.h"
//где operation - argv[1];
//0 -без ошибок
//1- знак ошибки
int toggleSwitch(char* operation, int amount, char *param[])
{
	if (amount<3)
	{
		printf("[ERROR]Количество параметром не может быть меньше 3\n");
		return 0;
	}
	//поместить файл(ы) в архив
	if (!strcmp(param[1], "-a"))
	{
		Info *ptrOnStruct = NULL; //указатель на структуру
		if ((ptrOnStruct = (Info*)malloc(sizeof(Info))) == NULL)//выделение памяти под структуру
			ALLOC_MEMORY_ERR
		/* код функции проверяющий исходные данные и возвращающий список неповторяющихся файлов*/
		int dubl = 0;
		FILE* tryOpen = NULL;
		char **strarray = NULL;
		int strcount = 0;
		for (int i = 3; i != amount; i++)
		{
			if ((tryOpen = fopen(param[i], "rb")) == NULL)
				//переходим на следующий
				continue;
			else
			{
				//сравнение на повтор
				for (int j = 3; (j < i); j++)
				{
					//если совпал, то выводим сообщение и игнорим его
					if (!strcmp(param[i], param[j]))
					{
						printf("[WARNING]Несколько раз встретился файл %s\n", param[i]);
						dubl = 1;
						break;
					}
				}
				if (dubl == 1)
				{
					dubl = 0;
					continue;
					//также пропускаем его
				}
				//добавление имени в массив
				strarray = (char **)realloc(strarray, (strcount + 1) * sizeof(char *));
				strarray[strcount++] = strdup(param[i]);
			}
		}
		/* print the array of strings*/
		//for (int i = 0; i < strcount; i++)
		//	printf("strarray[%d] == %s\n", i, strarray[i]);

		if (strcount > 0) addFiles(param[2], strarray, strcount, &ptrOnStruct);
		else printf("[ERROR] No files for adding");
		//free all
		for (int i = 0; i < strcount; i++)
			free(strarray[i]);
		free(strarray);
		free(ptrOnStruct);
		return 0;
	}

	//извлечь файл(ы) из архива
	if (!strcmp(param[1], "-x"))
	{
		printf("1\n");
		return 0;
	}

	//вывести информацию о файлах
	if (!strcmp(param[1], "-l"))
	{
		if (amount != 3)
		{
			printf("Неверное количество параметров для опции -l");
			return 1;
		}

		//проверка сигнатуры и расширения
		if (!checkUssd(param[2], 111))
		{
			showInfo(param[2]);
		}
		return 0;
	}
	//удалить файл из архива
	if (!strcmp(param[1], "-d"))
	{
		printf("1\n");
		return 0;
	}

	//проверить целостность архива
	if (!strcmp(param[1], "-t"))
	{
		printf("1\n");
		return 0;
	}

	//вывести help

	if (!strcmp(param[1], "-help"))
	{
		printf("1\n");
		return 0;
	}

	return 1;
}