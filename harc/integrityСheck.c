#include "header.h";
char readDataFromFile(char *buf, FILE *fin, unsigned short* crc, unsigned __int64 amount)
{
	unsigned __int64 temp = 0;
	unsigned __int64 temp2 = amount;
	int bufferSize = SizeOfBuf;
	while (temp2 != 0)
	{
		if (temp2 <= bufferSize)
		{
			temp = fread(buf, 1, temp2, fin);
			temp2 -= temp;
		}
		else
		{
			temp = fread(buf, 1, bufferSize, fin);
			temp2 -= temp;
		}

		if (crc)
		{
			crc16(buf, temp, crc);
		}
	}
	return 0;
}
char integrityÑheck(char *archiveName, Info **ptrOnStruct,char **file)
{
	FILE *archive = NULL;
	char *data = NULL;
	unsigned short currentCheckSum;
	if ((archive = fopen(archiveName, "rb")) == NULL)
		OPEN_ERR;
	unsigned __int64 size =getSize(archive);
	if (_fseeki64_nolock(archive, SIZE_SIGNATURE, SEEK_SET) != 0)
		FSEEK_ERR
	char flagErorr = 0;
		while (((_ftelli64_nolock(archive)) != size) && !flagErorr)
		{
			if ((fread(&((*ptrOnStruct)->checkSum), SIZE_CHECKSUM, 1, archive)) != 1)
			{
				flagErorr =1;
				READING_DATA_ERR
			}
			if ((fread(&((*ptrOnStruct)->lengthName), SIZE_LENGTHNAME , 1 , archive))!= 1)
			{
				flagErorr = 1;
				READING_DATA_ERR
			}
			if ((fread(&((*ptrOnStruct)->name), ((*ptrOnStruct)->lengthName), 1, archive)) != 1)
			{
				flagErorr = 1;
				READING_DATA_ERR
			}
			(*file) = (char*)malloc(((*ptrOnStruct)->lengthName) + 1);
			strncpy((*file), (*ptrOnStruct)->name, (*ptrOnStruct)->lengthName);
			(*file)[(*ptrOnStruct)->lengthName] = '\0';

			if ((fread(&((*ptrOnStruct)->flags), SIZE_FLAGS, 1, archive)) != 1)
			{
				flagErorr = 1;
				READING_DATA_ERR
			}
			if ((fread(&((*ptrOnStruct)->size), SIZE_SIZE, 1, archive)) != 1)
			{
				flagErorr = 1;
				READING_DATA_ERR
			}
			if ((data = (char*)malloc((*ptrOnStruct)->size)) == NULL)
				ALLOC_MEMORY_ERR
			//ñðàâíåíèå êîíòðîëüíûõ ñóìì
			currentCheckSum = CRC;
			readDataFromFile(data,archive,&currentCheckSum, (*ptrOnStruct)->size);
			free(data);
			if (currentCheckSum != (*ptrOnStruct)->checkSum)
				flagErorr = 1;

			
	}
		if (fclose(archive) == -1)
			CLOSING_FILE_ERR
	return flagErorr;
}