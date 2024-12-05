#include <arpa/inet.h>
#include <glib-2.0/glib.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "include/http_utils.h"

enum Method { UNKNOWN, GET, HEAD, OPTIONS, TRACE, PUT, DELETE, POST, PATCH };

#define TOU32(c0, c1, c2, c3) (c0 | (c1 << 8) | (c2 << 16) | (c3 << 24))

const uint32_t GET_ = TOU32('G', 'E', 'T', ' ');
const uint32_t PUT_ = TOU32('P', 'U', 'T', ' ');
const uint32_t HEAD_ = TOU32('H', 'E', 'A', 'D');
const uint32_t POST_ = TOU32('P', 'O', 'S', 'T');

const uint32_t OPTIONS_ = TOU32('O', 'P', 'T', 'I');
const uint32_t TRACE_ = TOU32('T', 'R', 'A', 'C');
const uint32_t DELETE_ = TOU32('D', 'E', 'L', 'E');
const uint32_t PATCH_ = TOU32('P', 'A', 'T', 'C');

enum Method get_method(char *req_str) {

  printf("%c%c%c%c\n", req_str[0], req_str[1], req_str[2], req_str[3]);

  switch (TOU32(req_str[0], req_str[1], req_str[2], req_str[3])) {
  case GET:
    return GET;
  case HEAD:
    return HEAD;
  case OPTIONS:
    if (req_str[4] == 'O' && req_str[5] == 'N' && req_str[6] == 'S') {
      return OPTIONS;
    }
    return UNKNOWN;
  case TRACE:
    return TRACE;
  case PUT:
    return PUT;
  case DELETE:
    return DELETE;
  case POST:
    return POST;
  case PATCH:
    return PATCH;

  default:
  }
}

void *handle_client(void *arg) {
  int client_fd = *(int *)arg;

  char *req_str = cw_get_req_str(client_fd);

  enum Method m = get_method(req_str);

  // GHashTable *hash = g_hash_table_new(g_str_hash, g_str_equal);

  for (size_t i = 0; i < strlen(req_str); i++) {
    char c = req_str[i];
  }

  cw_send_str(client_fd, 200, "Barisin amminake");

  cw_close_client(client_fd, req_str);
}

int main(void) {
  // GHashTable *hash = g_hash_table_new(g_str_hash, g_str_equal);
  // g_hash_table_insert(hash, "lmao", "Laughing My Ass Off");
  // g_hash_table_insert(hash, "clang", "i need help. please help");

  // g_hash_table_destroy(hash);

  struct sockaddr_in address;

  int addrlen = sizeof(address);
  int opt = 1;

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == 0) {
    perror("Socket failed");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    perror("setsockopt failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  // Bind to 127.0.0.1:8080
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr("127.0.0.1");
  address.sin_port = htons(8080);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("Bind failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  // Listen for incoming connections
  if (listen(server_fd, 1) < 0) {
    perror("Listen failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  printf("Server is listening on 127.0.0.1:8080\n");

  while (1) {
    // Accept a single connection

    int client_fd =
        accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (client_fd < 0) {
      perror("Accept failed");
      close(server_fd);
      exit(EXIT_FAILURE);
    }

    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, handle_client, &client_fd) != 0) {
      perror("Failed to create thread");
      return 1;
    }

    pthread_detach(thread_id);
  }

  close(server_fd);
  printf("Server closed\n");

  return 0;
}
