#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>

char base_directory[1024] = {0};

bool parse_path_req(const char *request, char *path, size_t path_size)
{
	const char *start = strstr(request, " ");
	if (!start)
		return false;

	start++;
	const char *end = strstr(start, " ");
	if (!end)
		end = request + strlen(request);

	size_t len = end - start;
	if (len > path_size - 1)
		len = path_size - 1;

	strncpy(path, start, len);
	path[len] = '\0';
	return true;
}

bool starts_with(const char *pre, const char *str)
{
	size_t lenpre = strlen(pre), lenstr = strlen(str);
	return lenstr >= lenpre && strncmp(pre, str, lenpre) == 0;
}

bool check_path(const char *request, const char *custom_path)
{
	char path[1024];
	if (!parse_path_req(request, path, sizeof(path)))
		return false;

	size_t custom_path_len = strlen(custom_path);
	if (custom_path[custom_path_len - 1] == '*')
	{
		char prefix[1024];
		strncpy(prefix, custom_path, custom_path_len - 1);
		prefix[custom_path_len - 1] = '\0';
		return starts_with(prefix, path);
	}
	return strcmp(path, custom_path) == 0;
}

void get_last_path_part(const char *path, char *segment, size_t segment_size)
{
	const char *last_slash = strrchr(path, '/');
	if (!last_slash)
	{
		strncpy(segment, path, segment_size);
	}
	else
	{
		last_slash++;
		size_t len = strlen(last_slash);
		if (len > segment_size - 1)
			len = segment_size - 1;
		strncpy(segment, last_slash, len);
		segment[len] = '\0';
	}
}

void handle_file_request(int client_fd, const char *path)
{
	char full_path[2048];
	snprintf(full_path, sizeof(full_path), "%s%s", base_directory, path);

	FILE *file = fopen(full_path, "rb");
	if (file == NULL)
	{
		char response404[] = "HTTP/1.1 404 Not Found\r\n\r\n";
		send(client_fd, response404, strlen(response404), 0);
		return;
	}

	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *buffer = malloc(file_size);
	fread(buffer, 1, file_size, file);

	char header[1024];
	snprintf(header, sizeof(header), "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: %ld\r\n\r\n", file_size);
	send(client_fd, header, strlen(header), 0);
	send(client_fd, buffer, file_size, 0);

	fclose(file);
	free(buffer);
}

void save_file_from_post(int client_fd, const char *path, const char *data)
{
	char full_path[2048];
	snprintf(full_path, sizeof(full_path), "%s/%s", base_directory, path);

	FILE *file = fopen(full_path, "wb");
	if (file == NULL)
	{
		char response500[] = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
		send(client_fd, response500, strlen(response500), 0);
		return;
	}

	fwrite(data, 1, strlen(data), file);
	fclose(file);

	char response201[] = "HTTP/1.1 201 Created\r\n\r\nFile saved successfully.";
	send(client_fd, response201, strlen(response201), 0);
}

char *get_post_body(const char *request)
{
	const char *body_start = strstr(request, "\r\n\r\n");
	if (!body_start)
	{
		return NULL;
	}
	return strdup(body_start + 4);
}

void *handle_connection(void *client_socket)
{
	int client_fd = *((int *)client_socket);
	free(client_socket);

	char client_request[4096] = {0};
	int request_size = recv(client_fd, client_request, sizeof(client_request), 0);
	if (request_size < 0)
	{
		printf("Error reading from socket: %s\n", strerror(errno));
		close(client_fd);
		return NULL;
	}

	char path[1024];
	parse_path_req(client_request, path, sizeof(path));
	printf("request: %s\n", client_request);

	if (check_path(client_request, "/"))
	{
		char response200[] = "HTTP/1.1 200 OK\r\n\r\ni want to end myself :)";
		send(client_fd, response200, strlen(response200), 0);
	}
	else if (check_path(client_request, "/echo/*"))
	{
		char last_segment[1024];
		get_last_path_part(path, last_segment, sizeof(last_segment));
		char response[2048];
		snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %zu\r\n\r\n%s", strlen(last_segment), last_segment);
		printf("/echo: %s", response);
		send(client_fd, response, strlen(response), 0);
	}
	else if (check_path(client_request, "/user-agent"))
	{
		char userAgent[1024] = {0};
		const char *userAgentPrefix = "User-Agent: ";
		char *start = strstr(client_request, userAgentPrefix);
		if (start)
		{
			start += strlen(userAgentPrefix);
			const char *end = strstr(start, "\r\n");
			if (end)
			{
				size_t userAgentLen = end - start;
				if (userAgentLen > sizeof(userAgent) - 1)
					userAgentLen = sizeof(userAgent) - 1;
				strncpy(userAgent, start, userAgentLen);
				userAgent[userAgentLen] = '\0';
			}
		}

		char response[2048];
		snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %zu\r\n\r\n%s", strlen(userAgent), userAgent);
		printf("/user-agent: %s\n", response);
		send(client_fd, response, strlen(response), 0);
	}
	else if (strncmp(client_request, "POST ", 5) == 0 && starts_with("/files/", path))
	{
		char *file_name = path + 7;
		char *post_data = get_post_body(client_request);
		if (post_data)
		{
			save_file_from_post(client_fd, file_name, post_data);
			free(post_data);
		}
		else
		{
			char response404[] = "HTTP/1.1 404 Not Found\r\n\r\n404 - Not Found";
			send(client_fd, response404, strlen(response404), 0);
		}
	}

	else if (check_path(client_request, "/files/*"))
	{
		char last_segment[1024];
		get_last_path_part(path, last_segment, sizeof(last_segment));
		handle_file_request(client_fd, last_segment);
	}
	else
	{
		char response404[] = "HTTP/1.1 404 Not Found\r\n\r\n404 - Not Found";
		printf("404: %s\n", response404);
		send(client_fd, response404, strlen(response404), 0);
	}

	close(client_fd);
	return NULL;
}

int main(int argc, char **argv)
{

	strncpy(base_directory, argv[2], sizeof(base_directory) - 1);

	int server_fd;
	struct sockaddr_in serv_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(4221),
		.sin_addr = {htonl(INADDR_ANY)}};

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		perror("socket creation failed");
		return 1;
	}

	int reuse = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
	{
		perror("setsockopt SO_REUSEADDR failed");
		return 1;
	}

	if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0)
	{
		perror("binding failed");
		return 1;
	}

	if (listen(server_fd, 10) != 0)
	{
		perror("listen failed");
		return 1;
	}

	printf("server running...\n");

	while (true)
	{
		struct sockaddr_in client_addr;
		socklen_t client_addr_len = sizeof(client_addr);
		int *client_fd = malloc(sizeof(int));
		*client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
		if (*client_fd < 0)
		{
			perror("error in accepting connection");
			free(client_fd);
			continue;
		}

		pthread_t thread_id;
		if (pthread_create(&thread_id, NULL, handle_connection, client_fd) != 0)
		{
			perror("Error creating thread");
			close(*client_fd);
			free(client_fd);
			continue;
		}

		pthread_detach(thread_id);
	}

	close(server_fd);
	return 0;
}
