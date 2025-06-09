#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * 7. Определить параметры соединения – номера портов с обеих сторон.
 */

typedef int fd_t;

constexpr in_port_t PORT = 8789;
constexpr uint32_t LOCALHOST = (127 << 24) + 1;

int main()
{
	// Структура с адресом и портом сервера
	struct sockaddr_in server_addr = {};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr = (struct in_addr){htonl(LOCALHOST)};

	// Входящий сокет
	fd_t serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (serv_sock == -1) {
		perror("Failed to open sock");
		return 1;
	}
	printf("Серверный сокет: %d\n", serv_sock);

	// Соединение сокета с портом и адресом
	if (bind(serv_sock, (struct sockaddr const *)&server_addr,
		 sizeof(server_addr))) {
		perror("Failed to bind socket");
		close(serv_sock);
		return 1;
	}

	// Создание очереди запросов на соединение
	if (listen(serv_sock, 100)) {
		perror("Listen failed");
		close(serv_sock);
		return 1;
	}

	// printf("Клиентский порт: %d\n", )

	// Структура с адресом и портом клиента
	struct sockaddr_in client_addr;
	socklen_t client_addr_len;

	fd_t client_sock = accept(serv_sock, (struct sockaddr *)&client_addr,
				  &client_addr_len);
	if (client_sock < 0) {
		perror("Accept failed");
		close(serv_sock);
		return 1;
	}
	printf("Клиентский сокет: %d\n", client_sock);


	ssize_t recv_ret;
	do {
		char buf[17];
		buf[16] = '\0';

		recv_ret = recv(client_sock, buf, 16, 0);
		if (recv_ret < 0) {
			perror("Recv failed");
			close(serv_sock);
			close(client_sock);
			return 1;
		}
		if(recv_ret == 0)
			break;

		printf("%s|\n", buf);


	} while (true);

	close(serv_sock);
	close(client_sock);
	return 0;
}
