#include "io.h"
#include "socks.h"
#include <malloc.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * Добавить в сервис поддержку дополнительной команды, реализующей телефонный
 * справочник. Клиент отправляет на сервер некоторое символьное имя. Сервер
 * ищет в файле телефон, связанный с этим именем. Если в файле информация не
 * найдена, клиенту возвращается соответствующее сообщение.
	- Входной параметр: ФИО.
	- Ответ сервера: телефон, связанный с введённым ФИО.
 */

char const * DB_FILENAME = "list.txt";
constexpr in_port_t PORT = 8789;
[[maybe_unused]] constexpr uint32_t LOCALHOST = (127 << 24) + 1;

int communication_cycle(fd_t fd, char const * db_mem);

int main()
{
	// Структура с адресом и портом клиента
	struct sockaddr_in client_addr = {};
	socklen_t client_addr_len = sizeof(client_addr);

	// Структура с адресом и портом сервера
	struct sockaddr_in server_addr = {};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	// server_addr.sin_addr = (struct in_addr){htonl(LOCALHOST)};

	// Открытие и чтение базы данных
	char * db_copy = read_db_from_file(DB_FILENAME);
	if (!db_copy) {
		perror("Failed to open db!");
		return -1;
	}

	// Входящий сокет
	fd_t serv_sock = create_bind_server_socket(&server_addr);
	if (-1 == serv_sock) {
		perror("Failed to create binded socket");
		return -1;
	}

	printf("Ожидание соединения на порт %hu\n", PORT);
	fd_t client_sock = accept(serv_sock, (struct sockaddr *)&client_addr,
				  &client_addr_len);
	if (client_sock < 0) {
		perror("Accept failed");
		close(serv_sock);
		return 1;
	}
	print_sockaddr_in_info(&client_addr);

	int communication_cycle_bad = communication_cycle(client_sock, db_copy);
	if (communication_cycle_bad < 0) {
		perror("Recv failed");
		close(serv_sock);
		close(client_sock);
		return 1;
	}

	// Штатное завершение работы
	puts("Клиент прервал соединение");
	close(serv_sock);
	close(client_sock);
	free(db_copy);
	return 0;
}

int handle_request(fd_t fd, char const * db_mem)
{
	return (int)(send(fd, db_mem, strlen(db_mem), 0));
}

int communication_cycle(fd_t fd, char const * db_mem)
{
	constexpr size_t buflen = 64;
	char buf[buflen + 1];
	buf[buflen] = '\0';

	do {
		ssize_t recv_ret = recv(fd, buf, buflen, 0);
		if (recv_ret > 0) {
			buf[recv_ret] = '\0';
			if (handle_request(fd, db_mem) < 0)
				return -1;
			continue;
		} else if (recv_ret == 0) {
			return 0;
		} else // if (recv_ret < 0)
			return -1;
	} while (true);
}
