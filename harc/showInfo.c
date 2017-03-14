#include "header.h";
void showInfo(char* archiveName)
{
	FILE* archive = NULL;
	FILE* infoAboutFiles = NULL;
	if ((archive = fopen(archiveName, "rb")) == NULL)
		OPEN_ERR
	if ((infoAboutFiles = fopen("output/infoAboutFiles.txt", "wb")) == NULL)
		OPEN_ERR
	unsigned __int64 endOFFile = getSize(archive);
	if (endOFFile == 0)
	{
		printf("Архив пуст:(\n");
		return;
	}
	unsigned int ussd = 0;
	unsigned short checkSum = 0;
	char lengthName = 0;
	char name[256];
	char flags = 0;
	unsigned __int64 size = 0;
	//однократное чтение сигнатуры
	if (fread(&ussd, sizeof(unsigned int), 1, archive) != 1)
		READING_DATA_ERR
	while ((ftell(archive))!= endOFFile)
	{
		if ((fread(&checkSum, SIZE_CHECKSUM, 1, archive))!=1)
			READING_DATA_ERR
		if ((fread(&lengthName, SIZE_LENGTHNAME, 1, archive))!=1)
			READING_DATA_ERR
		if ((fread(&name, lengthName, 1, archive))!=1)
			READING_DATA_ERR
		if ((fread(&flags, SIZE_FLAGS, 1, archive))!=1)
			READING_DATA_ERR
		if ((fread(&size, SIZE_SIZE, 1, archive))!=1)
			READING_DATA_ERR
		if (_fseeki64_nolock(archive, size, SEEK_CUR)!=0)
			FSEEK_ERR
	//запись информации
	for(int i=0; i<lengthName;i++)
		fprintf(infoAboutFiles, "%c",name[i]);
	fprintf(infoAboutFiles, "  %llu  ", size);
	if (flags == ZERO) fprintf(infoAboutFiles, "%s", "Не сжатый\n");
		else fprintf(infoAboutFiles, "%s", "Cжатый\n");
	}
	if (fclose(archive) == -1)
		CLOSING_FILE_ERR
	if (fclose(infoAboutFiles) == -1)
		CLOSING_FILE_ERR
}