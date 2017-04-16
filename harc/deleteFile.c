#include "header.h"
char delete(char *archiveName, char *fileName, Info **ptrOnStruct)
{
	FILE *archive = NULL;
	FILE *tmp = NULL;
	char *data = NULL;
	unsigned int ussd = SIGNATURE;
	if (accessRights(archiveName) != 1) {
		printf("[WARNING:]Архив %s не имеет прав на чтение и запись\n", archiveName);
		return 1;
	}
	if ((archive = fopen(archiveName,"rb")) == NULL)
		OPEN_ERR
	//создание временного файла
	char *tmpArchiveName =NULL;
	tmpArchiveName = uniqName();
	if ((tmp = fopen(tmpArchiveName, "wb")) == NULL)
		CREATE_FILE_ERR
	if ((fwrite(&ussd , SIZE_SIGNATURE , 1 ,tmp))!=1)
				WRITING_DATA_ERR
	_fflush_nolock(tmp);
	UINT64 size =getSize(archive);
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
			if ((fread(&((*ptrOnStruct)->compression), SIZE_FLAGS, 1, archive)) != 1)
				READING_DATA_ERR
			if ((fread(&((*ptrOnStruct)->size), SIZE_SIZE, 1, archive)) != 1)
				READING_DATA_ERR
			//короткая строка для правильного сравнения
			char *TMP2name = NULL;
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
			if (fwrite(&((*ptrOnStruct)->compression), SIZE_FLAGS, 1, tmp) != 1)
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
	//TODO уникальное имя для темпа
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
		printf("[WARNING:] В архиве сожержится лишь сигнатура, хотите удалить архив? Y/N\n");
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
	//начинаем с третьего, так как в параметрах файлы с третьего
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