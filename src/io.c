#include "io.h"
#include <assert.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

// Файловый дескриптор
typedef int fd_t;

char * read_db_from_file(char const * fname)
{
	struct stat fstats = {};
	fd_t db_fd = open(fname, O_RDONLY);
	if (db_fd == -1)
		return nullptr;

	if(fstat(db_fd, &fstats)){
		close(db_fd);
		return nullptr;
	}
	off_t const filesize = fstats.st_size;
	assert(filesize >= 0);
		
	char * db_mem = (char *)malloc(1 + (size_t)filesize);
	if(!db_mem){
		close(db_fd);
		return nullptr;
	}
	db_mem[filesize] = '\0';

	ssize_t const read_ret = read(db_fd, db_mem, (size_t)filesize);
	if (read_ret < 0) {
		close(db_fd);
		return nullptr;
	}
	db_mem[read_ret] = '\0';

	close(db_fd);
	return db_mem;
}
