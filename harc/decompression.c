#include "header.h"
char read_bit(FILE* in, int *pos, unsigned char *buffer)
{
	if (*pos==0)
	{
		fread(buffer, sizeof(char), 1, in);
	}
	(*pos)++;
	int bit = (*buffer) >> 7;
	(*buffer) <<= 1;
	if ((*pos) == 8)
		*pos = 0;
	return bit;
}
unsigned char read_char(FILE* in, int *pos, unsigned char *buffer)
{
	unsigned char c = 0;
	for (int i = 0; i < sizeof(char) * 8; i++)
	{
		c <<= 1;
		c |= read_bit(in, pos, buffer);
	}
	return c;
}
/*Восстановление дерева из файла*/
Tree *createNode(FILE *inputFile, int *pos, unsigned char *buffer)
{
	unsigned char c;
	Tree *tmp = (Tree*)malloc(sizeof(Tree));
	char bit = read_bit(inputFile, pos,buffer);
	if (bit == 0)
	{
		tmp->symbol = -1;
		tmp->left = createNode(inputFile, pos, buffer);
		tmp->right = createNode(inputFile, pos, buffer);
		return tmp;
	}
	if (bit == 1)
	{
		c = read_char(inputFile, pos, buffer);
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
		size -= sizeof(UINT64);//потому что в размере содержится размер закодированной части
	computeCRC(data, inputFile, crc, size, WITHSHIFT);
	unsigned char bufferTmp = 0;
	int pos = 0;
	Tree *root = createNode(inputFile,&pos,&bufferTmp);
	Tree *tmp = root;
	unsigned char c;
	/*Декодирование с помощью прохода по дереву*/
	for (UINT64 i = 0; i < dataSize; i++)
	{
		c = read_bit(inputFile, &pos, &bufferTmp);
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