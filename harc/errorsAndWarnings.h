//1)ОШИБКА ПРИ ОТКРЫТИИ ФАЙЛА
#define OPEN_ERR							\
{											\
	printf("Ошибка при открытии файла\n");  \
	return 1;								\
}				

//2)ОШИБКА ПРИ ВЫДЕЛЕНИИ ПАМЯТИ
#define ALLOC_MEMORY_ERR						\
{											    \
	printf("Ошибка при выделении памяти\n");    \
	return 1;								    \
}				

//3)ОШИБКА ПРИ ЗАПИСИ ДАННЫХ
#define WRITING_DATA_ERR					    \
{											    \
	printf("Ошибка при записи данных\n");       \
	return 1;								    \
}		

//4)ОШИБКА ПРИ ЧТЕНИИ ДАННЫХ
#define READING_DATA_ERR					    \
{											    \
	printf("Ошибка при чтении данных\n");       \
	return 1;								    \
}	

//5)ОШИБКА ПРИ ЗАКРЫТИИ
#define CLOSING_FILE_ERR					    \
{											    \
	printf("Ошибка при закрытии файла\n");      \
	return 1;								    \
}	