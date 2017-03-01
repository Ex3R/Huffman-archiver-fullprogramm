#include "header.h";
#include "errorsAndWarnings.h";

char** checkValidFiles(int amount, char * param[], int* strcount)
{
	int noFile = 0;
	int dubl = 0;
	FILE* tryOpen = NULL;
	char **strarray = NULL;
	(*strcount) = 0;
	for (int i = 3; i != amount; i++)
	{
		if ((tryOpen = fopen(param[i], "rb")) == NULL)
			//переходим на следующий
			continue;
		else
		{
			//сравнение на повтор
			for (int j = 3; (j<i); j++)
			{	
				//если совпал, то выводим сообщение и игнорим его
				if (!strcmp(param[i], param[j]))
				{
					printf("[WARNING]Несколько раз встретился файл %s\n", param[i]);
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
		strarray = (char **)realloc(strarray, ((*strcount) + 1) * sizeof(char *));
		strarray[(*strcount)++] = strdup(param[i]);
		}
	}
	//!дописать free для всех элементов
	/* print the array of strings 
	for (int i = 0; i < strcount; i++)
		printf("strarray[%d] == %s\n", i, strarray[i]);
	*/
	return strarray;
}
//для обрезки длинного имени
char* makeNameShorter(char* name)
{
	int i = strlen(name);
	for (i; ((name[i] != '/') && (i + 1)); i--);
	return &name[++i];
}
int addFiles(char* archiveName, char* fileName, Info **ptrOnStruct, int firstly)
{
	FILE *fin = NULL, *fout = NULL;
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