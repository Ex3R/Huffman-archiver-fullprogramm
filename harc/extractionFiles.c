#include "header.h"
/*
0- без ошибок
1 - Данный файл имеет отличное от архивного расширения
2- Данный файл имеет отличную от архивной сигнатуру
*/
char checkUssd(char* archiveName, unsigned int ussd)
{
	//проверка расширения
	int i = strlen(archiveName);
	for (i; ((archiveName[i] != '.') && (i + 1)); i--);
	char* tmp = NULL;
	tmp = &archiveName[i++];
	if (strcmp(tmp, EXTENTION))
	{
		printf("Данный файл имеет отличное от архивного расширения %s\n",EXTENTION);
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
	if (fclose(archive) == -1)
		CLOSING_FILE_ERR
	if (curUssd != ussd)
	{
		printf("Данный файл имеет отличную от архивной сигнатуру\n");
		return 2;
	}

	return 0;
}
char delete(char *archiveName, char *fileName, Info **ptrOnStruct)
{
	FILE *archive = NULL;
	FILE *tmp = NULL;
	char *data = NULL;
	unsigned int ussd = SIGNATURE;
	if ((archive = fopen(archiveName,"rb")) == NULL)
		CREATE_FILE_ERR
	char path ="output/tmp.txt";
	if ((tmp = fopen("output/tmp.txt", "wb")) == NULL)
		CREATE_FILE_ERR
	if ((fwrite(&ussd , SIZE_SIGNATURE , 1 ,tmp))!=1)
				WRITING_DATA_ERR
	_fflush_nolock(tmp);
	unsigned __int64 size =getSize(archive);
	char flagFounded = 0;
	if (_fseeki64_nolock(archive, SIZE_SIGNATURE, SEEK_SET) != 0)
		FSEEK_ERR
		while ((_ftelli64_nolock(archive)) != size)
		{
			if ((fread(&((*ptrOnStruct)->checkSum), SIZE_CHECKSUM, 1 , archive)) != 1)
				READING_DATA_ERR
			if ((fread(&((*ptrOnStruct)->lengthName), SIZE_LENGTHNAME , 1 , archive))!= 1)
				READING_DATA_ERR
			if ((fread(&((*ptrOnStruct)->name), ((*ptrOnStruct)->lengthName), 1, archive)) != 1)
				READING_DATA_ERR
			if ((fread(&((*ptrOnStruct)->flags), SIZE_FLAGS, 1, archive)) != 1)
				READING_DATA_ERR
			if ((fread(&((*ptrOnStruct)->size), SIZE_SIZE, 1, archive)) != 1)
				READING_DATA_ERR
			char *TMP2name = malloc( ((*ptrOnStruct)->lengthName) + 1);
			int b = 0;
			for (b; b < ((*ptrOnStruct)->lengthName); b++)
			{
				TMP2name[b] = ((*ptrOnStruct)->name)[b];
			}
			TMP2name[b] = '\0';
			//если совпали
				if (!strcmp(TMP2name, fileName))
				{	
					flagFounded = 1;
					free(TMP2name);
					if (_fseeki64_nolock(archive, ((*ptrOnStruct)->size), SEEK_CUR) != 0)
						FSEEK_ERR
					continue;
				}
			//иначе не совпали - то есть нужно переписать в темп
			if ((data = (char*)malloc((*ptrOnStruct)->size)) == NULL)
				ALLOC_MEMORY_ERR
			if ((fwrite(&((*ptrOnStruct)->checkSum), SIZE_CHECKSUM, 1, tmp)) != 1)
				WRITING_DATA_ERR
			if ((fwrite(&((*ptrOnStruct)->lengthName), SIZE_LENGTHNAME, 1, tmp)) != 1)
				WRITING_DATA_ERR
			if ((fwrite(&((*ptrOnStruct)->name), (*ptrOnStruct)->lengthName, 1, tmp)) != 1)
				WRITING_DATA_ERR
			if (fwrite(&((*ptrOnStruct)->flags), SIZE_FLAGS, 1, tmp) != 1)
				WRITING_DATA_ERR
			if (fwrite(&((*ptrOnStruct)->size), SIZE_SIZE, 1, tmp) != 1)
				WRITING_DATA_ERR
			writeDataToFile(data,archive,tmp,NULL, (*ptrOnStruct)->size);
			free(data);
		}
	if (fclose(archive) == -1)
		CLOSING_FILE_ERR
	if (fclose(tmp) == -1)
		CLOSING_FILE_ERR
	if (remove(archiveName) == -1)
		perror("[ERROR]Could not delete %s\n", archiveName);
	if (rename("output/tmp.txt", archiveName) == -1)
		printf("[ERROR]Не удалось переименовать временный архив\n");
	//если файл не нашли
	if (!flagFounded)
	{
		printf("[WARNING]Файл %s отсутствует в архиве %s \n", fileName, archiveName);
		return 1;
	}
	else printf("Файл %s был успешно удалён из архива %s \n", fileName, archiveName);
	
	return 0;
}