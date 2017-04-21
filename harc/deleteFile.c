#include "header.h"
char delete(char *archiveName, char *fileName, Info **ptrOnStruct)
{
	FILE *archive = NULL;
	FILE *tmp = NULL;
	UINT64 size;
	char *data = NULL;
	char flagFounded = 0;
	char *TMP2name = NULL; 	//короткая строка для правильного сравнения
	unsigned int ussd = SIGNATURE;
	if (accessRights(archiveName,READING) != 1) {
		printf("[WARNING:]Архив %s не имеет прав на чтение\n", archiveName);
		return 0;
	}
	if ((archive = fopen(archiveName,"rb")) == NULL)
		OPEN_ERR
	char *tmpArchiveName =NULL; tmpArchiveName = uniqName();
	if ((tmp = fopen(tmpArchiveName, "wb")) == NULL)
		CREATE_FILE_ERR
	if ((fwrite(&ussd , SIZE_SIGNATURE , 1 ,tmp))!=1)
				WRITING_DATA_ERR _fflush_nolock(tmp);
	size = getSize(archive);
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
			if ((fread(&((*ptrOnStruct)->compression), SIZE_FLAGS, 1, archive)) != 1)
				READING_DATA_ERR
			if ((fread(&((*ptrOnStruct)->size), SIZE_SIZE, 1, archive)) != 1)
				READING_DATA_ERR
			if ((TMP2name = (char*)malloc( ((*ptrOnStruct)->lengthName) + 1)) ==NULL)
				ALLOC_MEMORY_ERR
			strncpy((TMP2name), (*ptrOnStruct)->name, (*ptrOnStruct)->lengthName);
			TMP2name[(*ptrOnStruct)->lengthName] = '\0';
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
			if ((fwrite(&((*ptrOnStruct)->checkSum), SIZE_CHECKSUM, 1, tmp)) != 1)
				WRITING_DATA_ERR
			if ((fwrite(&((*ptrOnStruct)->lengthName), SIZE_LENGTHNAME, 1, tmp)) != 1)
				WRITING_DATA_ERR
			if ((fwrite(&((*ptrOnStruct)->name), (*ptrOnStruct)->lengthName, 1, tmp)) != 1)
				WRITING_DATA_ERR
			if (fwrite(&((*ptrOnStruct)->flags), SIZE_FLAGS, 1, tmp) != 1)
				WRITING_DATA_ERR
			if (fwrite(&((*ptrOnStruct)->compression), SIZE_FLAGS, 1, tmp) != 1)
				WRITING_DATA_ERR
			if (fwrite(&((*ptrOnStruct)->size), SIZE_SIZE, 1, tmp) != 1)
				WRITING_DATA_ERR
			if ((data = (char*)malloc(SizeOfBuf)) == NULL)
				ALLOC_MEMORY_ERR
			writeDataToFile(data, archive, tmp, NULL, (*ptrOnStruct)->size);
			free(data);
		}
	if (fclose(archive) == -1)
		CLOSING_FILE_ERR
	if (fclose(tmp) == -1)
		CLOSING_FILE_ERR
	if (remove(archiveName) == -1)
		perror("[ERROR:] Could not delete %s\n", archiveName);
	if (rename(tmpArchiveName, archiveName) == -1)
		printf("[ERROR:] Не удалось переименовать временный архив\n");
	//если файл не нашли
	if (!flagFounded)
	{
		printf("[WARNING:] Файл %s отсутствует в архиве %s \n", fileName, archiveName);
		return 1;
	}
	else printf("[SUCCSESS:] Файл %s был успешно удалён из архива %s \n", fileName, archiveName);
	//после удаления может остаться лишь одна сигнатура
	if ((archive = fopen(archiveName, "rb")) == NULL)
		OPEN_ERR
	size = getSize(archive);
	if (fclose(archive) == -1)
		CLOSING_FILE_ERR
	if (size == SIZE_SIGNATURE)
	{
		printf("[WARNING:] В архиве %s сожержится лишь сигнатура, хотите удалить архив? Y/N\n",archiveName);
		char answer;
		answer = getchar();
		if ((answer == 'Y') || (answer == 'y') || (answer == 'н'))
		{
			if (remove(archiveName) == -1)
				perror("[ERROR]Could not delete %s\n", archiveName);
			printf("Успешно удалено!\n");
		}
		else
		{
			printf("Удаление отменено\n");
			return 0;
		}
	}
	return 0;
}