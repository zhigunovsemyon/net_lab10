#ifndef IO_H_
#define IO_H_

/*Чтение базы данных из текстового файла в ОЗУ.
 *!ПАМЯТЬ ДОЛЖНА БЫТЬ ОСВОБОЖДЕНА ВЫЗОВОМ free()!*/
char * read_db_from_file(char const * fname);

#endif // !IO_H_
