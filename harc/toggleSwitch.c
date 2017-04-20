#include "header.h"
void printHelp()
{
	printf("Программа архиватор имеет следующий формат вызова:\n"
		"harc.exe <операция> <имя архива> <имя файла> ....\n"
		"Операции:\n"
		"-a - поместить файл(ы) в архив\n"
		"-x - извлечь файл(ы) из архивa\n"
		"-d - удалить файл из архива\n"
		"-t - проверить целостность архива\n"
		"P.S не поддерживается архивация файлов из разных дирректорий\n"
	);
}
/*
Parsing parameters:
1 - error
0 - without mistakes
*/
int toggleSwitch(char* operation, int amount, char *param[])
{
	if ((amount<3) & (strcmp(param[1], "-help")))
	{
		printf("[ERROR:] Количество параметром не может быть меньше 2\n");
		printHelp();
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
						printf("[WARNING:]Несколько раз встретился файл %s\n", param[i]);
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
		if (strcount > 0) addFiles(param[2], strarray, strcount, &ptrOnStruct);
		else printf("[ERROR:] No files for adding\n");
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
		unsigned short crc = CRC;
		if ((checkUssd(param[2], SIGNATURE)) != 0)
			return 0;
		FILE *archive = NULL;
		if (accessRights(param[2],READING) != 1) {
			printf("[WARNING:]Архив %s не имеет прав на чтение\n", param[2]);
			return 1;
		}
		if ((archive = fopen(param[2], "rb")) == NULL)
			OPEN_ERR
		Info *ptrOnStruct = NULL;
		if ((ptrOnStruct = (Info*)malloc(sizeof(Info))) == NULL)
			ALLOC_MEMORY_ERR
		List *head = NULL;
		List *tmp = NULL;
		tmp = (List*)malloc(sizeof(List));
		makeListOfFiles(amount, param, &head);
		if (!head)
		{
			printf("[ERROR:] Список извлекаемых пуст\n");
			return 0;
		}
		UINT64 size = getSize(archive);
		if (_fseeki64_nolock(archive, SIZE_SIGNATURE, SEEK_SET) != 0)
			FSEEK_ERR
		int count = 0;
		while ((_ftelli64_nolock(archive)) != size)
		{
			if ((fread(&((ptrOnStruct)->checkSum), SIZE_CHECKSUM, 1 , archive)) != 1)
				READING_DATA_ERR
			if ((fread(&((ptrOnStruct)->lengthName), SIZE_LENGTHNAME , 1 , archive))!= 1)
				READING_DATA_ERR
			if ((fread(&((ptrOnStruct)->name), ((ptrOnStruct)->lengthName), 1, archive)) != 1)
				READING_DATA_ERR
			if ((fread(&((ptrOnStruct)->flags), SIZE_FLAGS, 1, archive)) != 1)
				READING_DATA_ERR
			if ((fread(&((ptrOnStruct)->compression), SIZE_FLAGS, 1, archive)) != 1)
				READING_DATA_ERR
			if ((fread(&((ptrOnStruct)->size), SIZE_SIZE, 1, archive)) != 1)
				READING_DATA_ERR
			char * tmpFileName = (char*)malloc(((ptrOnStruct)->lengthName) + 1);
			strncpy((tmpFileName), (ptrOnStruct)->name, (ptrOnStruct)->lengthName);
			tmpFileName[(ptrOnStruct)->lengthName] = '\0';
			char flagForDeleting = 0;
			//сравнение со списком файлов
			tmp = head;
			while (tmp)
			{
				//если они совпали
				if (!strcmp(tmp->file, tmpFileName))
				{
					flagForDeleting = 1;
					FILE *newFile = NULL;
					char *data = NULL;
					if (fileExists(tmpFileName))
					{
						if (!accessRights(tmpFileName, WRITING))
						{
								printf("[WARNING:]Архив %s не имеет прав на чтение и запись\n", tmpFileName);
								return 1;
						}
						
					}
					if ((newFile = fopen(tmpFileName, "wb")) == NULL)
						CREATE_FILE_ERR
					if ((ptrOnStruct)->flags == 0) {
						if ((data = (char*)malloc(SizeOfBuf)) == NULL)
							ALLOC_MEMORY_ERR
						writeDataToFile(data, archive, newFile, &crc, ptrOnStruct->size);
						free(data);
					}
					else
					{
						decode(archive, newFile,&crc, (ptrOnStruct)->size);
					}
					//проверка crc
					if (crc != (ptrOnStruct)->checkSum)
						printf("[WARNING:] В процессе извлечения файл был повреждён:(\n");
					count =deleteByValue(&head, tmpFileName);
					fflush(newFile);
					if (fclose(newFile) == -1)
						CLOSING_FILE_ERR
					break;
				}
				tmp = tmp->next;
			}
			if (flagForDeleting == 0)
			{
				_fseeki64_nolock(archive, ptrOnStruct->size, SEEK_CUR);
			}
			else
			{
				flagForDeleting = 0;
			}
			//если не было совпадений

			free(tmpFileName);
		}
		if (count == 1)  head = NULL;
		if (head)
		{
			printf("[WARNING:] Следующие файлы отсутствуют в архиве\n");
			printLinkedList(head);
		}
		if (fclose(archive) == -1)
			CLOSING_FILE_ERR
		free(ptrOnStruct);
		return 0;
	}
	//вывести информацию о файлах
	if (!strcmp(param[1], "-l"))
	{
		if (amount != 3)
		{
			printf("[WARNING:]Неверное количество параметров для опции %s\n", "-l");
			printHelp();
			return 0;
		}
		Info *ptrOnStruct = NULL;
		if ((ptrOnStruct = (Info*)malloc(sizeof(Info))) == NULL)
			ALLOC_MEMORY_ERR
		if (!checkUssd(param[2], SIGNATURE))
		{
			showInfo(param[2],&ptrOnStruct);
		}
		free(ptrOnStruct);
		return 0;
	}
	//удалить файл из архива
	if (!strcmp(param[1], "-d"))
	{
		if (amount != 4)
		{
			printf("[WARNING:]Неверное количество параметров для опции %s\n", DELETE);
			printHelp();
			return 0;
		}
		if ((checkUssd(param[2],SIGNATURE))!= 0)
			return 0;
		Info *ptrOnStruct = NULL; 
		if ((ptrOnStruct = (Info*)malloc(sizeof(Info))) == NULL)
			ALLOC_MEMORY_ERR
		delete(param[2],param[3],&ptrOnStruct);
		free(ptrOnStruct);
		return 0;
	}

	//проверить целостность архива
	if (!strcmp(param[1], "-t"))
	{
		if (amount != 3)
		{
			printf("[WARNING:]Неверное количество параметров для опции %s\n", INTEGRITYCHECK);
			printHelp();
			return 0;
		}
		if ((checkUssd(param[2], SIGNATURE)) != 0)
			return 0;
		Info *ptrOnStruct = NULL;
		if ((ptrOnStruct = (Info*)malloc(sizeof(Info))) == NULL)
			ALLOC_MEMORY_ERR
		char **file =NULL;
		if (integrityСheck(param[2], &ptrOnStruct, &file) == 1)
			printf("Архив %s повреждён, а именно на файле %s\n", param[2], file);
		else printf("Архив %s цел\n", param[2]);
		//TODO free???
		free(file);
		file = NULL;
		free(ptrOnStruct);
		return 0;
	}
	if (!strcmp(param[1], "-help"))
	{
		printHelp();
		return 0;
	}
	return 1;
}