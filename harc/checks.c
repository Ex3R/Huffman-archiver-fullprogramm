#include "header.h"
/*Проверка архива(сигнатура и расширение*/
/*
0- без ошибок
1 - Данный файл имеет отличное от архивного расширения
2- Данный файл имеет отличную от архивной сигнатуру
*/
char checkUssd(char* archiveName)
{
	//проверка расширения
	int i = strlen(archiveName);
	for (i; ((archiveName[i] != '.') && (i + 1)); i--);
	char* tmp = NULL;
	tmp = &archiveName[i++];
	if (strcmp(tmp, EXTENTION))
	{
		printf("[ERROR:]Файл %s имеет отличное от архивного расширения %s\n", archiveName, EXTENTION);
		return 1;
	}
	FILE* archive = NULL;
	if ((archive = fopen(archiveName, "rb")) == NULL)
		OPEN_ERR
		//проверка сигнатуры
		unsigned int curUssd = 0;
	if (fread(&curUssd, SIZE_SIGNATURE, 1, archive) != 1)
		READING_DATA_ERR
		if (curUssd != SIGNATURE)
		{
			printf("[ERROR:]Данный файл имеет отличную от архивной сигнатуру\n");
			return 2;
		}
	if (fclose(archive) == -1)
		CLOSING_FILE_ERR
		return 0;
}

/*Функция будет определять размер файла
0 - если не удалось открыть
1- если удалось открыть, но он пуст
2- если не пуст

*/
char isEmptyFile(char* fileName)
{
	FILE* file = NULL;
	if ((file = fopen(fileName, "rb")) == NULL)
	{
		return 0;
	}
	//конец файла
	UINT64 endOFFile = getSize(file);
	if (fclose(file) == -1)
		CLOSING_FILE_ERR
		if (endOFFile == 0)
		{
			return 1;
		}
		else return 2;
}
/*
0 - not compress
1 - compress
*/
char compressOrNot(UINT64 size)
{
	if (size > LIMIT_FOR_COMPRESSION)
		return 1;
	else return 0;
}