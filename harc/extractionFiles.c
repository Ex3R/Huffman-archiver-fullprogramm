#include "header.h"
char *tmpName(char* NameHHH,int length)
{
	char *TMP2name = malloc(length + 1);
	int b = 0;
	for (b; b < (length); b++)
	{
		TMP2name[b] = NameHHH[b];
	}
	TMP2name[b] = '\0';
	return &TMP2name;
}
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
		printf("[ERROR:]Данный файл имеет отличное от архивного расширения %s\n",EXTENTION);
		return 1;
	}
	FILE* archive = NULL;
	if ((archive = fopen(archiveName, "rb")) == NULL)
		OPEN_ERR
	//проверка сигнатуры
	unsigned int curUssd = 0;
	if (fread(&curUssd, SIZE_SIGNATURE, 1, archive) != 1)
		READING_DATA_ERR
	if (fclose(archive) == -1)
		CLOSING_FILE_ERR
	if (curUssd != SIGNATURE)
	{
		printf("[ERROR:]Данный файл имеет отличную от архивной сигнатуру\n");
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
	//TODO
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
	//после удаления может остаться лишь одна сигнатура
	if ((archive = fopen(archiveName, "rb")) == NULL)
		OPEN_ERR
	size = getSize(archive);
	if (fclose(archive) == -1)
		CLOSING_FILE_ERR
	if (size == SIZE_SIGNATURE)
	{
		printf("[WARNING:]В архиве сожержится лишь сигнатура, хотите удалить архив? Y/N\n");
		char answer;
		answer = getchar();
		if (answer == 'Y')
		{
			if (remove(archiveName) == -1)
				perror("[ERROR]Could not delete %s\n", archiveName);
		}
		else return 0;
	
	}

	
	return 0;
}
//операции со списком
void adding(List **head, char *fileName)
{
		List *cur = NULL;
		cur = *head;
		while (cur)
		{
			if (!strcmp(cur->file, fileName)) return;
			cur = cur->next;
		}
		List *tmp = NULL;
		tmp = (List*)malloc(sizeof(List));
		(tmp->file) = (char*)malloc(strlen(fileName) + 1);
		strncpy(tmp->file, fileName, strlen(fileName));
		tmp->file[strlen(fileName)] = '\0';
		tmp->next = *head;
		*head = tmp;
		return;
}
void printLinkedList(List *head) {
	if (!head) {
		printf("Spisok pust");
	}
	while (head) {
		printf("%s\n", head->file);
		head = head->next;
	}
}
makeListOfFiles(int argc, char* argv[],List **head)
{
	for (int i = 3; i < argc; i++)
	{
		adding(head, argv[i]);
	}
}
int  deleteByValue(List **head, char *fileName)
{
	if (*head == NULL)
		return;
	List *pred = NULL;
	List *tmp = NULL;
	tmp = (List*)malloc(sizeof(List));
	pred = (List*)malloc(sizeof(List));
	tmp = *head;
	int count = 0;
	while (tmp)
	{
		tmp = tmp->next;
		count++;
	}
	tmp = *head;
	//конец подсчёта количества элементов в списке
	while (strcmp(tmp->file, fileName))
	{
		pred = tmp;
		tmp = tmp->next;
	}
	if (!strcmp(tmp->file, fileName))
	{
		if (tmp==(*head))
		{
			*head = tmp->next;
		}
		else pred->next = tmp->next;
		free(tmp);
	}
	return count;
}