#include "header.h"
int extractFiles(List **listOfFiles,FILE *archive)
{
	unsigned short crc = CRC;
	FILE *newFile;
	int count = 0;
	UINT64 size;
	char *data = NULL;
	Info *ptrOnStruct = NULL;
	List *tmp = NULL;
	char flagForDeleting;
	int flagExtractAll = 0;
	if ((ptrOnStruct = (Info*)malloc(sizeof(Info))) == NULL)
		ALLOC_MEMORY_ERR
	if (!(*listOfFiles)) {
		flagExtractAll = 1;
	}
	tmp = (List*)malloc(sizeof(List));
	size = getSize(archive);
	if (_fseeki64_nolock(archive, SIZE_SIGNATURE, SEEK_SET) != 0)
		FSEEK_ERR
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
			if (!flagExtractAll)
			{
				flagForDeleting = 0;
				tmp = (*listOfFiles);
				while (tmp)
				{
						if (!strcmp(tmp->file, tmpFileName))//если они совпали
						{
							flagForDeleting = 1;
							newFile = NULL;
							if (fileExists(tmpFileName))
							{
								if (!accessRights(tmpFileName, WRITING))
								{
										ACCESSDENIED
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
							if (crc != (ptrOnStruct)->checkSum) //сравнение crc после извлечения
							printf("[WARNING:] В процессе извлечения файл был повреждён:(\n");
							count =deleteByValue(listOfFiles, tmpFileName);
							fflush(newFile);
							if (fclose(newFile) == -1)
								CLOSING_FILE_ERR
							break;
						}
						tmp = tmp->next;
				}
				if (flagForDeleting == 0)
				{
					_fseeki64_nolock(archive, ptrOnStruct->size, SEEK_CUR);//сдвигаемся на размер файла
				}
				else
				{
					flagForDeleting = 0;
				}
				free(tmpFileName);
				if (count == 1)  *listOfFiles = NULL;
				if (*listOfFiles)
				{
					printf("[WARNING:] Следующие файлы отсутствуют в архиве\n");
					printLinkedList(*listOfFiles);
				}
			}
		//извлечь все
			newFile = NULL;
			if (fileExists(tmpFileName))
			{
				if (!accessRights(tmpFileName, WRITING))
				{
					ACCESSDENIED
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
					decode(archive, newFile, &crc, (ptrOnStruct)->size);
				}
			if (crc != (ptrOnStruct)->checkSum) //сравнение crc после извлечения
				printf("[WARNING:] В процессе извлечения файл был повреждён:(\n");
			fflush(newFile);
			if (fclose(newFile) == -1)
				CLOSING_FILE_ERR
		}
	free(ptrOnStruct);
	return 0;
}