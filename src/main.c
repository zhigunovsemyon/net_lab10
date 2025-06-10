#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * Добавить в сервис поддержку дополнительной команды, реализующей телефонный
 * справочник. Клиент отправляет на сервер некоторое символьное имя. Сервер
 * ищет в файле телефон, связанный с этим именем. Если в файле информация не
 * найдена, клиенту возвращается соответствующее сообщение.
	- Входной параметр: ФИО.
	- Ответ сервера: телефон, связанный с введе6нным ФИО.
 */

// Файловый дескриптор
typedef int fd_t;

constexpr in_port_t PORT = 8789;
[[maybe_unused]] constexpr uint32_t LOCALHOST = (127 << 24) + 1;

int communication_cycle(fd_t fd);

fd_t create_bind_server_socket(struct sockaddr_in const * ip_info);

void print_sockaddr_in_info(struct sockaddr_in const * addr);

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

	// Входящий сокет
	fd_t serv_sock = create_bind_server_socket(&server_addr);
	if (-1 == serv_sock) {
		perror("Failed to create binded socket");
		return -1;
	}

	fd_t client_sock = accept(serv_sock, (struct sockaddr *)&client_addr,
				  &client_addr_len);
	if (client_sock < 0) {
		perror("Accept failed");
		close(serv_sock);
		return 1;
	}
	print_sockaddr_in_info(&client_addr);

	int communication_cycle_bad = communication_cycle(client_sock);
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
	return 0;
}

void print_sockaddr_in_info(struct sockaddr_in const * addr)
{
	// 1-4 октеты
	uint8_t const fsto = ntohl(addr->sin_addr.s_addr) >> 24;
	uint8_t const sndo = 0xFF & (ntohl(addr->sin_addr.s_addr) >> 16);
	uint8_t const trdo = 0xFF & (ntohl(addr->sin_addr.s_addr) >> 8);
	uint8_t const ftho = 0xFF & (ntohl(addr->sin_addr.s_addr));

	printf("Подключение от:%u.%u.%u.%u", fsto, sndo, trdo, ftho);
	printf(":%u\n", ntohs(addr->sin_port));
}

fd_t create_bind_server_socket(struct sockaddr_in const * ip_info)
{
	// Входящий сокет
	fd_t serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (serv_sock == -1)
		return -1;

	// Соединение сокета с портом и адресом
	if (bind(serv_sock, (struct sockaddr const *)ip_info,
		 sizeof(*ip_info))) {
		close(serv_sock);
		return -1;
	}

	// Создание очереди запросов на соединение
	if (listen(serv_sock, 100)) {
		close(serv_sock);
		return -1;
	}

	return serv_sock;
}

int communication_cycle(fd_t fd)
{
	constexpr size_t buflen = 64;
	char buf[buflen + 1];
	buf[buflen] = '\0';

	do {
		ssize_t recv_ret;

		recv_ret = recv(fd, buf, buflen, 0);
		if (recv_ret < 0) {
			return 1;
		}
		if (recv_ret == 0) {
			return 0;
		}
		buf[recv_ret] = '\0';

		// Вывод буфера
		printf("%s", buf);
	} while (true);
}
