#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

/*
 * 7. Определить параметры соединения – номера портов с обеих сторон.
 */

typedef int fd_t;

constexpr in_port_t PORT = 8789;
constexpr uint32_t LOCALHOST = (127 << 24) + 1;

int main()
{
	// Структура с адресом и портом
	struct sockaddr_in sockaddr_ = {};
	sockaddr_.sin_family = AF_INET;
	sockaddr_.sin_port = PORT;
	sockaddr_.sin_addr = (struct in_addr){htonl(LOCALHOST)};

	// Входящий сокет
	fd_t inc_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (inc_sock == -1) {
		perror("Failed to open sock");
		return 1;
	}
	printf("Открытый сокет: %d\n", inc_sock);

	// Соединение сокета с портом и адресом
	if (-1 == bind(inc_sock, (struct sockaddr const *)&sockaddr_,
		       sizeof(sockaddr_))) {
		perror("Failed to bind socket");
		close(inc_sock);
		return 1;
	}

	close(inc_sock);
	return 0;
}
