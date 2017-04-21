#include "header.h"
char read_bit(FILE* in)
{
	static unsigned char buf = 0, counter = 0;
	if (!counter)
	{
		fread(&buf, sizeof(char), 1, in);
	}
	counter++;
	int bit = buf >> 7;
	buf <<= 1;
	if (counter == 8)
		counter = 0;
	return bit;
}
unsigned char read_char(FILE* in)
{
	unsigned char c = 0;
	for (int i = 0; i < sizeof(char) * 8; i++)
	{
		c <<= 1;
		c |= read_bit(in);
	}
	return c;
}
/*Восстановление дерева из файла*/
Tree *createNode(FILE *inputFile)
{
	unsigned char c;
	Tree *tmp = (Tree*)malloc(sizeof(Tree));
	char bit = read_bit(inputFile);
	if (bit == 0)
	{
		tmp->symbol = -1;
		tmp->left = createNode(inputFile);
		tmp->right = createNode(inputFile);
		return tmp;
	}
	if (bit == 1)
	{
		c = read_char(inputFile);
		tmp->symbol = c;
		tmp->left = tmp->right = NULL;
		tmp->next = NULL;
		return tmp;
	}
}
void decode(FILE *inputFile, FILE *outputFile, unsigned short *crc, UINT64 size)
{
	char *data = NULL;
	UINT64 dataSize = 0;
	fread(&dataSize, sizeof(UINT64), 1, inputFile);
	if ((data = (char*)malloc(SizeOfBuf)) == NULL)
		ALLOC_MEMORY_ERR
	size-=sizeof(UINT64);//потому что в размере содержится размер закодированной части
	computeCRC(data,inputFile,crc,size,WITHSHIFT);
	Tree *root = createNode(inputFile);
	Tree *tmp = root;
	unsigned char c;
	/*Декодирование с помощью прохода по дереву*/
	for (UINT64 i = 0; i < dataSize; i++)
	{
		c = read_bit(inputFile);
		if (!tmp->next)
		{
			if (fwrite(&(tmp->symbol), sizeof(char), 1, outputFile) != 1)
				WRITING_DATA_ERR
				tmp = root;
			continue;
		}
		if (c == 0)
		{
			
			tmp = tmp->left;
		}
		else
		{
			tmp = tmp->right;
		}
		if (tmp->symbol != -1)
		{
			if (fwrite(&(tmp->symbol), sizeof(char), 1, outputFile) != 1)
				WRITING_DATA_ERR
			tmp = root;
		}
	}
	free(data);
	deleteNode(root); root = NULL;
	fcloseall;
}