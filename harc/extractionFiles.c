#include "header.h"
#include "errorsAndWarnings.h"
/*
0- без ошибок
1 - Данный файл имеет отличное от архивного расширения
2- Данный файл имеет отличную от архивной сигнатуру
*/

int checkUssd(char* archiveName, const unsigned int ussd, char func)
{
	//проверка расширения
	int i = strlen(archiveName);
	for (i; ((archiveName[i] != '.') && (i + 1)); i--);
	char* tmp = NULL;
	tmp = &archiveName[i++];
	//заменить .txt на .mi
	if (strcmp(tmp, ".txt"))
	{
		printf("Данный файл имеет отличное от архивного расширения .txt\n");
		return 1;
	}
	FILE* archive = NULL;
	if ((archive = fopen(archiveName, "rb")) == NULL)
	OPEN_ERR
	//проверка сигнатуры
	unsigned int curUssd = 0;
	if (fread(&curUssd, sizeof(unsigned int), 1, archive) != 1)
		READING_DATA_ERR
	fflush(archive);
	if (!archive) CLOSING_FILE_ERR
	else fclose(archive);
	if (curUssd != ussd)
	{
		printf("Данный файл имеет отличную от архивной сигнатуру\n");
		return 2;
	}

	return 0;
}