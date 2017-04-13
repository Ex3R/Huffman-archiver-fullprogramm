#include "header.h"
void deleteNode(Tree* root) {
	if (root) {
		deleteNode(root->left);
		deleteNode(root->right);
		free(root);
	}
}
/*функция подсчёта частоты встречаемости для каждого символа*/
void createFrequencyArr(FILE *file, UINT64 *arr, UINT64 size)
{
	for (UINT64 i = 0; i < size; i++)
	{
		arr[fgetc(file)]++;
	}
	return;
}
void insert(Tree **head, UINT64 count, int symbol, char mode)
{
	Tree* tmp = (Tree*)malloc(sizeof(Tree));
	Tree* cur = NULL;
	int flag = 0;
	if (mode == 0)//добавление в список
	{
		//если список пуст
		if (*head == NULL)
		{
			tmp->left = tmp->right = NULL;
			tmp->symbol = symbol;
			tmp->count = count;
			tmp->next = *head;
			*head = tmp;
			return;
		}
		if (count <= (*head)->count) /* вставить перед первым */
		{
			tmp->count = count;
			tmp->symbol = symbol;
			tmp->left = tmp->right = NULL;
			tmp->next = (*head);
			*head = tmp;
		}
		else
		{
			cur = (*head);
			/* ищем позицию для вставки */
			while ((flag == 0) && (cur->next != NULL))
			{
				if ((cur->count < count) && (cur->next->count >= count)) flag = 1;
				cur = cur->next;
			}
			if (flag == 0) /* позиция не найдена - вставить в конец */
			{
				tmp->count = count;
				tmp->symbol = symbol;
				tmp->left = tmp->right = NULL;
				tmp->next = NULL;
				cur->next = tmp;
			}
			else /* позиция в середине найдена */
			{
				tmp->count = count;
				tmp->symbol = symbol;
				tmp->left = tmp->right = NULL;
				tmp->next = cur->next;
				cur->next = tmp;
			}
		}
	}
	//добавление в дерево
	else
	{
		if (count <= (*head)->count) /* вставить перед первым */
		{
			tmp->count = (*head)->count + (*head)->next->count;
			tmp->symbol = NODE;
			tmp->left = *head;
			tmp->right = (*head)->next;
			tmp->next = (*head);
			*head = tmp;
		}
		else
		{
			cur = (*head);
			/* ищем позицию для вставки */
			while ((flag == 0) && (cur->next != NULL))
			{
				if ((cur->count < count) && (cur->next->count >= count)) flag = 1;
				cur = cur->next;
			}
			if (flag == 0) /* позиция не найдена - вставить в конец */
			{
				tmp->count = (*head)->count + (*head)->next->count;
				tmp->symbol = NODE;
				tmp->left = *head;
				tmp->right = (*head)->next;
				tmp->next = NULL;
				cur->next = tmp;
				(*head) = (*head)->next->next;
			}
			else /* позиция в середине найдена */
			{
				tmp->count = (*head)->count + (*head)->next->count;
				tmp->symbol = NODE;
				tmp->left = *head;
				tmp->right = (*head)->next;
				tmp->next = cur->next;
				cur->next = tmp;
				(*head) = (*head)->next->next;
			}
		}
	}
}
/*Создание дерева Хаффмана*/
void makeHuffmanTree(Tree **head)
{
	UINT64 count;
	while ((*head)->next)
	{
		count = (*head)->count + (*head)->next->count;
		insert(head, count, NODE, MAKETREE);
	}
}
/*Построение неравномерного кода с помощью дерева Хаффмана*/
void CodeTable(Tree *root, char codes[256][256], char vrm[256])
{
	char tmp[256];
	int i = 0;
	strcpy(tmp, vrm);
	if (root->symbol >= 0)
	{
		strcpy(codes[root->symbol], tmp);
		return;
	}
	if (root->left)
		CodeTable(root->left, codes, strcat(tmp, "0"));

	while (tmp[i])
		i++;
	tmp[--i] = '\0';
	if (root->right)
		CodeTable(root->right, codes, strcat(tmp, "1"));
}
void CharToString(char *SymBuf, char c)
{
	for (int i = 0; i < 8; i++)
	{
		SymBuf[i] += !!(c & (1 << (7 - i)));
	}
}
/*
Функция записи бит
pos - позиция в буфере(0 до 7)
buffer - накапливаемый до байта набор битов
value - массив символов, который мы добавляем
*/
UINT64 writeBits(FILE *file, int *position, unsigned char *buffer, char *value,unsigned short *crc)
{
	int written = 0;//для подсчёта размера (в байтах)
	int bit;
	while (*value)
	{
		bit = *value - '0';
		if (bit)
		{
			*buffer = (*buffer | 1 << (7 - (*position)));
		}
		(*position)++;
		if ((*position) == 8)
		{
			fwrite(buffer, sizeof(char), 1, file);
			if (crc)
			{
				crc16(buffer, 1 , crc);
			}
			written++;
			(*position) = 0;
			*buffer = 0;
		}
		value++;
	}
	return written;
}
/*Функция записи дерева в файл
0-если узел, 1 - если лист, за 1 всегда следует код символа
*/
void WriteTree(Tree* root, unsigned char *buffer, int *position, FILE *outputFile)
{
	char SymBuf[] = { "00000000" };

	if (root->symbol == -1)
	{
		writeBits(outputFile, position, buffer, "0",NULL);
		WriteTree(root->left, buffer, position, outputFile);
		WriteTree(root->right, buffer, position, outputFile);
		return;
	}
	writeBits(outputFile, position, buffer, "1",NULL);
	CharToString(SymBuf, (char)(root->symbol));
	writeBits(outputFile, position, buffer, SymBuf,NULL);
}
/*Кодирование содержимого файла*/
UINT64 writeData(char codes[256][256], int *position, unsigned char *buffer, FILE *inputFile, FILE *outputFile, UINT64 size, unsigned short *crc)
{
	int c;
	UINT64 writtenData = 0;
	UINT64 count = 0;
	while (count != size) {
		c = fgetc(inputFile);
		writtenData += writeBits(outputFile, position, buffer, codes[c],crc);
		count++;
	}
	return writtenData;
}

void encode(FILE *inputFile, FILE *outputFile, UINT64 fileSize, unsigned short *crc)
{
	UINT64 arr[256] = { 0 },//массив для хранения count
		placeBeforeTree = 0,
		writtenData = 0,
		posAtBegin = _ftelli64_nolock(inputFile);
	Tree *head = NULL;
	char vrm[256] = { '\0' };
	char codes[256][256] = { '\0' };
	int pos = 0;
	unsigned char bufferTmp = 0; // Буфер в который будем писать
	char posInWRTree;
	createFrequencyArr(inputFile, arr, fileSize);
	/*Вставка элементов в список*/
	for (int i = 0; i < 256; i++)
	{
		if (arr[i] != 0)
		{
			insert(&head, arr[i], i, MAKELIST);
		}
	}
	makeHuffmanTree(&head);
	CodeTable(head, codes, vrm);// Построение кодов для символов
								//сдвиг для записи размера закодированной части (без дерева и чего-либо)
	placeBeforeTree = _ftelli64_nolock(outputFile);
	_fseeki64_nolock(outputFile, sizeof(UINT64), SEEK_CUR);
	// Записываем дерево
	int m = ftell(outputFile);
	WriteTree(head, &bufferTmp, &pos, outputFile);
	m = ftell(outputFile);
	_fseeki64_nolock(inputFile, posAtBegin, SEEK_SET);//сдвиг в начало инпут файла для кодирования
	posInWRTree = pos;//запоминаем позицию в буфере при записи дерева в файл
	writtenData = writeData(codes, &pos, &bufferTmp, inputFile, outputFile, fileSize,crc);
	m = ftell(outputFile);
	writtenData *= 8;
	if (pos != 0)//дозапись последнего байта
	{
		fwrite(&bufferTmp, sizeof(char), 1, outputFile);
		crc16(bufferTmp, 1, crc);
		bufferTmp = 0;
		writtenData += pos;
		pos = 0;
	}
	writtenData -= posInWRTree;
	fflush(outputFile);
	_fseeki64_nolock(outputFile, placeBeforeTree, SEEK_SET);
	//запись размера закодированной части
	fwrite(&writtenData, sizeof(UINT64), 1, outputFile);
	_fseeki64_nolock(outputFile, 0, SEEK_END);
	deleteNode(head);
}