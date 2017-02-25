#include "header.h";
#include "errorsAndWarnings.h";
char* makeNameShorter(char* name)
{
	//char String[] = { "name" };
	//char *newShortName = NULL;
	int i = strlen(name);
	for (i; ((name[i] != '/') && (i + 1)); i--);
	return &name[++i];
}
int addFiles(char* archiveName, char* fileName, Info **ptrOnStruct, FILE *fin, FILE *fout, int firstly)
{
	const unsigned int ussd = 111;
	struct stat info;
	stat(fileName, &info);
	if ((fin = fopen(fileName, "rb")) == NULL)
		OPEN_ERR
	if ((fout = fopen(archiveName, "a+b")) ==NULL)
		OPEN_ERR
	char *data = NULL;
	if ((data = (char*)malloc(info.st_size)) == NULL)
		ALLOC_MEMORY_ERR
	
	(*ptrOnStruct)->checkSum = crc16(data, info.st_size);//контрольная сумма
	(*ptrOnStruct)->lengthName = strlen(fileName); //длина имени файла
	strcpy((*ptrOnStruct)->name, fileName);
	(*ptrOnStruct)->flags = 0;
	(*ptrOnStruct)->size = info.st_size;

		//запись сигнатуры
		if (firstly==1) fwrite(&(ussd), 1, sizeof(const unsigned int), fout);
		if ((fwrite(&((*ptrOnStruct)->checkSum),sizeof(unsigned short), 1, fout)) != 1)
			WRITING_DATA_ERR
		if ((fwrite(&((*ptrOnStruct)->lengthName), sizeof(char), 1, fout)) != 1)
			WRITING_DATA_ERR
		if ((fwrite(&((*ptrOnStruct)->name), strlen((*ptrOnStruct)->name), 1, fout)) != 1)
			WRITING_DATA_ERR
		if (fwrite(&((*ptrOnStruct)->flags), sizeof(char), 1, fout) != 1)
			WRITING_DATA_ERR
		if (fwrite(&((*ptrOnStruct)->size), sizeof((*ptrOnStruct)->size), 1, fout) != 1)
			WRITING_DATA_ERR
		//чтение самого файла
	if ((fread(data, (*ptrOnStruct)->size, 1, fin)) != 1)
		READING_DATA_ERR
	if ((fwrite(data, (*ptrOnStruct)->size, 1, fout)) != 1)
		WRITING_DATA_ERR
	//очищение буфера, в котором хранится файл
	free(data);

	if (!fin) CLOSING_FILE_ERR
		else fclose(fin);
	if (!fout) CLOSING_FILE_ERR 
		else fclose(fout);
	return 0;
}