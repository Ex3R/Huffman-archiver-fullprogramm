#include "header.h";
/*Определение размера файла*/
UINT64 getSize(FILE* file)
{
	UINT64 endOFFile = 0;
	_fseeki64_nolock(file, 0, SEEK_END);
	endOFFile = _ftelli64_nolock(file);
	_fseeki64_nolock(file, 0, SEEK_SET);
	return endOFFile;
}
/*
 Функция копирования файла 
0 - если ошибок при записи не возникло
1- если ошибки
*/
char writeDataToFile(char *buf, FILE *fin, FILE *fout, unsigned short* crc, UINT64 amount)
{
	UINT64 temp = 0;
	UINT64 temp2 = amount;
	int bufferSize = SizeOfBuf;
	while (temp2!=0)
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

		if (fwrite(buf, 1, temp, fout) != temp)
			WRITING_DATA_ERR
	}
	return 0;
}
/*
Функция оставляет непосредственно имя файла
возвращает указатель на нужный символ
*/
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
char isEmptyFile(char* fileName)
{
	FILE* file = NULL;
	if ((file = fopen(fileName, "rb")) == NULL)
	{
		return 0;
	}
	//конец файла
	UINT64 endOFFile = getSize(file);
	if (fclose(file) == -1)
		CLOSING_FILE_ERR
	if (endOFFile == 0)
	{
		return 1;
	}
	else return 2;
}
/*
0 - not compress
1 - compress
*/
char compressOrNot(UINT64 size)
{
	if (size > LIMIT_FOR_COMPRESSION)
		return 1;
	else return 0;
}
double compressionRatio(double firstSize, double lastSize)
{
	double percent;
	percent = (firstSize - lastSize) / firstSize * 100;
	return percent;
}
int addFiles(char *archiveName, char **fileNames,int *amountOfFiles, Info **ptrOnStruct)
{
	unsigned short crc = CRC;
	const unsigned int ussd = SIGNATURE;
	struct _stat64 info;
	UINT64 posForWRCRC;//позиция для записи crc
	UINT64 posForWRSize;
	UINT64 realSize;
	char percentCompression;
	char *data = NULL;
	FILE *fin = NULL, *tmp=NULL, *fout = NULL;
	int makeTmpArchieve;//создавать временный архив или нет(флаг)
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
			if (checkUssd(archiveName, SIGNATURE) != 0) return 1;
			break;
	}
	//в цикле просто по порядку добавляем в пустой архив
	if ((makeTmpArchieve == 0) || (makeTmpArchieve == 1))
	{
		if ((fout = fopen(archiveName, "rb+")) == NULL)
			CREATE_FILE_ERR
		for (u; u < amountOfFiles; u++)
		{
				_stat64(fileNames[u], &info);
				if ((fin = fopen(fileNames[u], "rb")) == NULL)
					OPEN_ERR
				//заполнение полей структуры
				(*ptrOnStruct)->lengthName = strlen(shortNameOnly(fileNames[u])); //длина имени файла
				strcpy((*ptrOnStruct)->name, shortNameOnly(fileNames[u]));
				(*ptrOnStruct)->size = info.st_size;
				if (compressOrNot((*ptrOnStruct)->size))
					(*ptrOnStruct)->flags = COMPRESSED;
				else (*ptrOnStruct)->flags = NOTCOMPRESSED;
				(*ptrOnStruct)->compression = 0;
			//запись данных в архив
					_fseeki64_nolock(fout, 0, SEEK_END);
					 posForWRCRC = _ftelli64_nolock(fout);
					(*ptrOnStruct)->checkSum = crc;//контрольная сумма
					if ((fwrite(&((*ptrOnStruct)->checkSum), SIZE_CHECKSUM, 1, fout)) != 1)
						WRITING_DATA_ERR
					if ((fwrite(&((*ptrOnStruct)->lengthName), SIZE_LENGTHNAME, 1, fout)) != 1)
						WRITING_DATA_ERR
					if ((fwrite(&((*ptrOnStruct)->name), (*ptrOnStruct)->lengthName, 1, fout)) != 1)
						WRITING_DATA_ERR
					if (fwrite(&((*ptrOnStruct)->flags), SIZE_FLAGS, 1, fout) != 1)
						WRITING_DATA_ERR
					if (fwrite(&((*ptrOnStruct)->compression), SIZE_FLAGS, 1, fout) != 1)
						WRITING_DATA_ERR
					posForWRSize = _ftelli64_nolock(fout);
					if (fwrite(&((*ptrOnStruct)->size), SIZE_SIZE, 1, fout) != 1)
						WRITING_DATA_ERR
					//запись файла
					crc = CRC;
					if (compressOrNot((*ptrOnStruct)->size))
						encode(fin, fout, getSize(fin),&crc);
					else {
						if ((data = (char*)malloc(info.st_size)) == NULL)
							ALLOC_MEMORY_ERR
						writeDataToFile(data, fin, fout, &crc, (*ptrOnStruct)->size);
						free(data);
					}
					fflush(fout);
					realSize = _ftelli64_nolock(fout) - posForWRSize - SIZE_SIZE;
					percentCompression = (char)(compressionRatio((double)(*ptrOnStruct)->size, (double)realSize));
					//сдвиг для записи размера
					// -1 для записи процента сжатия
					_fseeki64_nolock(fout,posForWRSize-1, SEEK_SET);
					if ((fwrite(&percentCompression, SIZE_FLAGS, 1, fout)) != 1)
						WRITING_DATA_ERR
					if ((fwrite(&realSize, SIZE_SIZE, 1, fout)) != 1)
						WRITING_DATA_ERR
					//сдвиг обратно для записи контрольной суммы
					_fseeki64_nolock(fout, posForWRCRC, SEEK_SET);
					(*ptrOnStruct)->checkSum = crc;//контрольная сумма
					if ((fwrite(&((*ptrOnStruct)->checkSum), SIZE_CHECKSUM, 1, fout)) != 1)
						WRITING_DATA_ERR
					_fseeki64_nolock(fout,0,SEEK_END);
		}
		fcloseall;
	}
	//если нужно создавать временный архив
	else
	{
		if ((fin = fopen(archiveName, "rb+")) == NULL)
			OPEN_ERR
		//создание временного файла
		if ((tmp = fopen("output/tmp.txt", "wb+")) == NULL)
			OPEN_ERR
		//запись сигнатуры во временный архив
		if ((fwrite(&(ussd), sizeof(unsigned int), 1, tmp))!= 1)
			WRITING_DATA_ERR fflush(tmp);
		//определение размера архива
		UINT64 endOFFile = getSize(fin);
		//временные переменные
		unsigned int TMPussd = 0;
		//чтение сигнатуры
		if (fread(&TMPussd, SIZE_SIGNATURE, 1 , fin) != 1)
			READING_DATA_ERR
		while ((_ftelli64_nolock(fin)) != endOFFile)
		{
			//флаг = 1 , если совпадёт хоть один файл
			int flag = 0;
			int y;//переменная для цикла for(поиск повторов)
			if ((fread(&((*ptrOnStruct)->checkSum), SIZE_CHECKSUM, 1 , fin)) != 1)
				READING_DATA_ERR
			if ((fread(&((*ptrOnStruct)->lengthName), SIZE_LENGTHNAME , 1 ,fin))!= 1)
				READING_DATA_ERR
			if ((fread(&((*ptrOnStruct)->name), ((*ptrOnStruct)->lengthName), 1, fin)) != 1)
				READING_DATA_ERR
			if ((fread(&((*ptrOnStruct)->flags), SIZE_FLAGS, 1, fin)) != 1)
				READING_DATA_ERR
			if ((fread(&((*ptrOnStruct)->compression), SIZE_FLAGS, 1, fin)) != 1)
				READING_DATA_ERR
			if ((fread(&((*ptrOnStruct)->size), SIZE_SIZE, 1, fin)) != 1)
				READING_DATA_ERR
				y = 0;
				char *TMP2name = NULL;
				if ((TMP2name = (char*)malloc(((*ptrOnStruct)->lengthName) + 1)) == NULL)
					ALLOC_MEMORY_ERR
				strncpy((TMP2name), (*ptrOnStruct)->name, (*ptrOnStruct)->lengthName);
				TMP2name[(*ptrOnStruct)->lengthName] = '\0';
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
					//нужно сдвинуться на размер файла
					_fseeki64_nolock(fin, (*ptrOnStruct)->size, SEEK_CUR);
					flag = 0;
					free(data);
					continue;
				}
				else
				{
				//записываем во временный архив
				_fseeki64_nolock(tmp, 0, SEEK_END);
				posForWRCRC = _ftelli64_nolock(tmp);
				(*ptrOnStruct)->checkSum = crc;
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
				posForWRSize = _ftelli64_nolock(tmp);
				if (fwrite(&((*ptrOnStruct)->size), SIZE_SIZE, 1, tmp) != 1)
				WRITING_DATA_ERR
						//запись самого файла
						crc = CRC;
						if (compressOrNot((*ptrOnStruct)->size))
							encode(fin, tmp, getSize(fin),&crc);
						else {
							if ((data = (char*)malloc((*ptrOnStruct)->size)) == NULL)
								ALLOC_MEMORY_ERR
							writeDataToFile(data, fin, tmp, &crc, (*ptrOnStruct)->size);
							free(data);
						}
						fflush(tmp);
						realSize = _ftelli64_nolock(tmp) - posForWRSize - SIZE_SIZE;
						percentCompression = (char)(compressionRatio((double)(*ptrOnStruct)->size, (double)realSize));
						//сдвиг для записи размера
						_fseeki64_nolock(tmp, posForWRSize - 1, SEEK_SET);
						if ((fwrite(&percentCompression, SIZE_FLAGS, 1, tmp)) != 1)
							WRITING_DATA_ERR
						if ((fwrite(&realSize, SIZE_SIZE, 1, tmp)) != 1)
							WRITING_DATA_ERR
						//сдвиг обратно для записи контрольной суммы
						_fseeki64_nolock(tmp, posForWRCRC, SEEK_SET);
						(*ptrOnStruct)->checkSum = crc;//контрольная сумма
						if ((fwrite(&((*ptrOnStruct)->checkSum), SIZE_CHECKSUM, 1, tmp)) != 1)
							WRITING_DATA_ERR
						_fseeki64_nolock(tmp,0,SEEK_END);
				}
			}
		//закрываем наш архив
		if (fclose(fin) == -1)
		CLOSING_FILE_ERR
		//теперь во временный нужно переписать то, что было в основном
			for (int t = 0; t < amountOfFiles; t++)
			{
				_stat64(fileNames[t], &info);
				if ((fin = fopen(fileNames[t], "rb")) == NULL)
					OPEN_ERR
				//заполнение данных
				(*ptrOnStruct)->checkSum = crc;
				(*ptrOnStruct)->lengthName = strlen(shortNameOnly(fileNames[t]));
				strcpy((*ptrOnStruct)->name, shortNameOnly(fileNames[t]));
				(*ptrOnStruct)->size = info.st_size;
				if (compressOrNot((*ptrOnStruct)->size))
					(*ptrOnStruct)->flags = COMPRESSED;
				else (*ptrOnStruct)->flags = NOTCOMPRESSED;
				(*ptrOnStruct)->compression = 0;
				//запись данных
				_fseeki64_nolock(tmp, 0, SEEK_END);
				posForWRCRC = _ftelli64_nolock(tmp);
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
				posForWRSize = _ftelli64_nolock(tmp);
				if (fwrite(&((*ptrOnStruct)->size), SIZE_SIZE, 1, tmp) != 1)
				WRITING_DATA_ERR
				//чтение и запись самого файла
				crc = CRC;
				if (compressOrNot((*ptrOnStruct)->size))
					encode(fin, tmp, getSize(fin), &crc);
				else {
					if ((data = (char*)malloc((*ptrOnStruct)->size)) == NULL)
						ALLOC_MEMORY_ERR
						writeDataToFile(data, fin, tmp, &crc, (*ptrOnStruct)->size);
					free(data);
				}
				fflush(tmp);
				realSize = _ftelli64_nolock(tmp) - posForWRSize - SIZE_SIZE;
				percentCompression = (char)(compressionRatio((double)(*ptrOnStruct)->size, (double)realSize));
				_fseeki64_nolock(tmp, posForWRSize - 1, SEEK_SET);
				if ((fwrite(&percentCompression, SIZE_FLAGS, 1, tmp)) != 1)
					WRITING_DATA_ERR
				if ((fwrite(&realSize, SIZE_SIZE, 1, tmp)) != 1)
					WRITING_DATA_ERR
				//сдвиг обратно для записи контрольной суммы
				_fseeki64_nolock(tmp, posForWRCRC, SEEK_SET);
				(*ptrOnStruct)->checkSum = crc;//контрольная сумма
				if ((fwrite(&((*ptrOnStruct)->checkSum), sizeof(unsigned short), 1, tmp)) != 1)
					WRITING_DATA_ERR
				_fseeki64_nolock(tmp, 0, SEEK_END);
				if (fclose(fin) == -1)
					CLOSING_FILE_ERR
			}
		if (fclose(tmp) == -1)
			CLOSING_FILE_ERR
			//удалить архив переименовать temp
			if (remove(archiveName) == -1)
				perror("[ERROR]:Could not delete %s\n",archiveName);
			if (rename("output/tmp.txt", archiveName) == -1)
				printf("[ERROR]:Не удалось переименовать временный архив\n");
	}
	return 0;
}

