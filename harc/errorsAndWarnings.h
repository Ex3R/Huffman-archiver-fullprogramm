//1)ОШИБКА ПРИ ОТКРЫТИИ ФАЙЛА
#define OPEN_ERR									\
{													\
	printf("[ERROR]Ошибка при открытии файла\n");   \
	return 1;										\
}				
//2)ОШИБКА ПРИ ВЫДЕЛЕНИИ ПАМЯТИ
#define ALLOC_MEMORY_ERR								\
{														\
	printf("[ERROR]Ошибка при выделении памяти\n");     \
	return 1;										    \
}				
//3)ОШИБКА ПРИ ЗАПИСИ ДАННЫХ
#define WRITING_DATA_ERR								  \
{														  \
	printf("[ERROR]Ошибка при записи данных\n");		  \
	return 1;											  \
}		
//4)ОШИБКА ПРИ ЧТЕНИИ ДАННЫХ
#define READING_DATA_ERR								\
{													    \
	printf("[ERROR]Ошибка при чтении данных\n");        \
	return 1;								            \
}	
//5)ОШИБКА ПРИ ЗАКРЫТИИ ФАЙЛА
#define CLOSING_FILE_ERR					            \
{											            \
	printf("[ERROR]Ошибка при закрытии файла\n");       \
	return 1;								            \
}
//6)ОШИБКА ПРИ РАБОТЕ ФУНКЦИИ fseek
#define FSEEK_ERR										   \
{														    \
	printf("[ERROR]ОШИБКА ПРИ РАБОТЕ ФУНКЦИИ fseek\n");      \
	return 1;												  \
}
//7)ОШИБКА ПРИ СОЗДАНИИ ФАЙЛА
#define CREATE_FILE_ERR									   \
{														    \
	printf("[ERROR]ОШИБКА ПРИ СОЗДАНИИ ФАЙЛА\n");			 \
	return 1;												  \
}
