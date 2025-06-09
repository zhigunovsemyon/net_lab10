#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

/*
 * 7. Определить параметры соединения – номера портов с обеих сторон.
 */

typedef int fd_t;

int main ()
{
	fd_t sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		perror("Failed to open sock");
		return 1;
	}
	printf("Открытый сокет: %d\n", sock);

	close(sock);
	return 0;
}
