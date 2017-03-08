#include "header.h";
//не рабочя((((((
char* checkValidFiles(int amount, char * param[], int* strcount)
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

	/* print the array of strings*/ 
	for (int i = 0; i < strcount; i++)
		printf("strarray[%d] == %s\n", i, strarray[i]);
	
	return strarray;
}
//для обрезки длинного имени
char* shortNameOnly(char* name)
{
	int i = strlen(name);
	for (i; ((name[i] != '/') && (i + 1)); i--);
	return &name[++i];
}
/*Функция будет определять размер файла
0 - если не удалось открыть
1- если удалось открыть, но он пуст
2- если не пуст

*/
int isEmptyFile(char* fileName)
{
	FILE* file = NULL;
	if ((file = fopen(fileName, "rb")) == NULL)
	{
		return 0;
	}
	//конец файла
	long endOFFile = 0;
	fseek(file, 0, SEEK_END);
	endOFFile = ftell(file);
	if (fclose(file) == -1)
		CLOSING_FILE_ERR
	if (endOFFile == 0)
	{
		//значит файл есть, но он пуст
		return 1;
	}
	else return 2;
}
int addFiles(char *archiveName, char **fileNames,int *amountOfFiles, Info **ptrOnStruct)
{
	const unsigned int ussd = 111;
	struct stat info;
	char *data = NULL;
	FILE *fin = NULL, *tmp=NULL, *fout = NULL;
	int makeTmpArchieve;//создавать временный архив или нет
	int u = 0; 
	switch (makeTmpArchieve = isEmptyFile(archiveName))
	{
		case 0:
		case 1:
			if ((fout = fopen(archiveName, "a+b")) == NULL)
				OPEN_ERR
			if ((fwrite(&(ussd),sizeof(unsigned int), 1 ,fout))!=1)
				WRITING_DATA_ERR
			fflush(fout);
			if (fclose(fout) == -1)
				CLOSING_FILE_ERR
			makeTmpArchieve = 0;
			break;
		case 2:
			checkUssd(archiveName, 111);
			break;
	}
	//в цикле просто по порядку добавляем в пустой архив
	if ((makeTmpArchieve == 0) || (makeTmpArchieve == 1))
	{
		if ((fout = fopen(archiveName, "a+b")) == NULL)
			OPEN_ERR
		for (u; u < amountOfFiles; u++)
		{
			stat(fileNames[u], &info);
			if ((fin = fopen(fileNames[u], "rb")) == NULL)
				OPEN_ERR
			if ((data = (char*)malloc(info.st_size)) == NULL)
				ALLOC_MEMORY_ERR
			//заполнение полей структуры
			(*ptrOnStruct)->checkSum = crc16(data, info.st_size);//контрольная сумма
			(*ptrOnStruct)->lengthName = strlen(shortNameOnly(fileNames[u])); //длина имени файла
			strcpy((*ptrOnStruct)->name, shortNameOnly(fileNames[u]));
			(*ptrOnStruct)->flags = 0;
			(*ptrOnStruct)->size = info.st_size;
			//запись данных в архив
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
			//чтение и запись самого файла
			if ((fread(data, (*ptrOnStruct)->size, 1, fin)) != 1)
				READING_DATA_ERR
			if ((fwrite(data, (*ptrOnStruct)->size, 1, fout)) != 1)
				WRITING_DATA_ERR
			//очищение буфера, в котором хранится файл
			free(data);
			//закрытие очередного файла, который записали
			if (fclose(fin) == -1)
				CLOSING_FILE_ERR
		}
	//закрытие архива
	if (fclose(fout) == -1)
		CLOSING_FILE_ERR
	}
	//если нужно создавать временный архив
	else
	{
		if ((fin = fopen(archiveName, "rb")) == NULL)
			OPEN_ERR
		//создание временного файла
		if ((tmp = fopen("output/tmp.txt", "wb")) == NULL)
			OPEN_ERR
		//запись сигнатуры во временный архив
		if ((fwrite(&(ussd), sizeof(unsigned int), 1, tmp))!= 1)
			WRITING_DATA_ERR
		fflush(tmp);
		//определение размера архива
		unsigned int endOFFile = 0;
		fseek(fin, 0, SEEK_END);
		endOFFile = ftell(fin);
		fseek(fin, 0, SEEK_SET);
		//временные переменные
		unsigned int TMPussd = 0;
		unsigned short TMPcheckSum = 0;
		char TMPlengthName = "";
		char TMPname[256];
		char TMPflags = 0;
		unsigned int TMPsize = 0;
		//чтение сигнатуры
		if (fread(&TMPussd, sizeof(unsigned int), 1, fin) != 1)
			READING_DATA_ERR
		while ((ftell(fin)) != endOFFile)
		{
			//флаг = 1 , если совпадёт хоть один файл
			int flag = 0;
			int y;//переменная для цикла for(поиск повторов)
				if ((fread(&TMPcheckSum, sizeof(unsigned short), 1, fin))!=1)
					READING_DATA_ERR
				if ((fread(&TMPlengthName, sizeof(char), 1, fin))!=1)
					READING_DATA_ERR
				if ((fread(&TMPname, TMPlengthName, 1, fin))!=1)
					READING_DATA_ERR
				if ((fread(&TMPflags, sizeof(char), 1, fin))!=1)
					READING_DATA_ERR
				if ((fread(&TMPsize, sizeof(unsigned int), 1, fin))!=1)
					READING_DATA_ERR
				//TODO не обязательно читать файл, если в цикле совпадение будет
				if ((data = (char*)malloc(TMPsize)) == NULL)
					ALLOC_MEMORY_ERR
				if ((fread(data, TMPsize, 1, fin)) != 1)
					READING_DATA_ERR
				//поиск повторов
				y = 0;
				char *TMP2name = malloc(TMPlengthName+1);
				int b = 0;
				for (b; b < TMPlengthName; b++)
				{
					TMP2name[b] = TMPname[b];
				}
				TMP2name[b] = '\0';
				for (y; y < amountOfFiles; y++)
				{
					if (!strcmp(TMP2name, shortNameOnly(fileNames[y])))
					{
						flag = 1;
						break;
					}
				}
				free(TMP2name);
				//если есть совпадение, то мы просто пропускаем данный файл и смотрим следующий
				if (flag)
				{
					flag = 0;
					free(data);
					continue;
				}
				else
				{
				//записываем во временный архив
				if ((fwrite(&TMPcheckSum,sizeof(unsigned short), 1, tmp)) != 1)
					WRITING_DATA_ERR
				if ((fwrite(&TMPlengthName, sizeof(char), 1, tmp)) != 1)
					WRITING_DATA_ERR
				if ((fwrite(&TMPname, TMPlengthName, 1, tmp)) != 1)
					WRITING_DATA_ERR
				if (fwrite(&TMPflags, sizeof(char), 1, tmp) != 1)
					WRITING_DATA_ERR
				if (fwrite(&TMPsize, sizeof(unsigned int), 1, tmp) != 1)
					WRITING_DATA_ERR
				//запись самого файла
				if ((fwrite(data, TMPsize, 1, tmp)) != 1)
					WRITING_DATA_ERR
				free(data);
				}
			}
		//закрываем наш архив
			if (fclose(fin) == -1)
				CLOSING_FILE_ERR
		//теперь во временный нужно переписать то, что было в основном
			for (int t = 0; t < amountOfFiles; t++)
			{
				stat(fileNames[t], &info);
				if ((data = (char*)malloc(info.st_size)) == NULL)
				ALLOC_MEMORY_ERR
				if ((fin = fopen(fileNames[t], "rb")) == NULL)
					OPEN_ERR
				//заполнение данных
				(*ptrOnStruct)->checkSum = crc16(data, info.st_size);//контрольная сумма
				(*ptrOnStruct)->lengthName = strlen(shortNameOnly(fileNames[t])); //длина имени файла
				strcpy((*ptrOnStruct)->name, shortNameOnly(fileNames[t]));
				(*ptrOnStruct)->flags = 0;
				(*ptrOnStruct)->size = info.st_size;
				//запись данных
				if ((fwrite(&((*ptrOnStruct)->checkSum),sizeof(unsigned short), 1, tmp)) != 1)
					WRITING_DATA_ERR
				if ((fwrite(&((*ptrOnStruct)->lengthName), sizeof(char), 1, tmp)) != 1)
					WRITING_DATA_ERR
				if ((fwrite(&((*ptrOnStruct)->name), strlen((*ptrOnStruct)->name), 1, tmp)) != 1)
					WRITING_DATA_ERR
				if (fwrite(&((*ptrOnStruct)->flags), sizeof(char), 1, tmp) != 1)
					WRITING_DATA_ERR
				if (fwrite(&((*ptrOnStruct)->size), sizeof((*ptrOnStruct)->size), 1, tmp) != 1)
					WRITING_DATA_ERR
				//чтение и запись самого файла
				if ((fread(data, (*ptrOnStruct)->size, 1, fin)) != 1)
					READING_DATA_ERR
				if ((fwrite(data, (*ptrOnStruct)->size, 1, tmp)) != 1)
					WRITING_DATA_ERR
				//очищение буфера, в котором хранится файл
				free(data);
				if (fclose(fin) == -1)
					CLOSING_FILE_ERR
			}
	
			//удалить архив переименовать temp
			if (fclose(tmp) == -1)
				CLOSING_FILE_ERR
			if (remove(archiveName) == -1)
				perror("Could not delete '123'");
			if (rename("output/tmp.txt", archiveName) == -1)
				printf("[ERROR]Не удалось переименовать временный архив");
	}
return 0;
}

	

	
	/*
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
		fwrite(&(ussd), 1, sizeof(const unsigned int), fout);

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
		else fclose(fout);*/
