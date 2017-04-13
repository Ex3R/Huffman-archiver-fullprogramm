#include "header.h";
void showInfo(char* archiveName, Info **ptrOnStruct)
{
	FILE* archive = NULL;
	FILE* infoAboutFiles = NULL;
	if ((archive = fopen(archiveName, "rb")) == NULL)
		OPEN_ERR
	if ((infoAboutFiles = fopen("output/infoAboutFiles.txt", "wb")) == NULL)
		CREATE_FILE_ERR;
	UINT64 endOFFile = getSize(archive);
	if (endOFFile == 0)
	{
		printf("[WARNING:]Архив пуст:(\n");
		return;
	}
	//однократное чтение сигнатуры
	if (_fseeki64_nolock(archive, SIZE_SIGNATURE, SEEK_SET) != 0)
		FSEEK_ERR
	while ((ftell(archive))!= endOFFile)
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
			if (_fseeki64_nolock(archive, (*ptrOnStruct)->size, SEEK_CUR)!=0)
			FSEEK_ERR
	//запись информации
	for(int i=0; (i<(*ptrOnStruct)->lengthName);i++)
		fprintf(infoAboutFiles, "%c", (*ptrOnStruct)->name[i]);
	fprintf(infoAboutFiles, "  %   llu    ", (*ptrOnStruct)->size);
	if ((*ptrOnStruct)->flags == ZERO) fprintf(infoAboutFiles, "%s", "Не сжатый   ");
		else fprintf(infoAboutFiles, "%s", "Cжатый   ");
	fprintf(infoAboutFiles, "%d%s", (*ptrOnStruct)->compression, "%\n");
	fflush(archive);
	}
	if (fclose(archive) == -1)
		CLOSING_FILE_ERR
	if (fclose(infoAboutFiles) == -1)
		CLOSING_FILE_ERR
}