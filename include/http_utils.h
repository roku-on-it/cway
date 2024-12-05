#pragma once

#define MAX_REQ_SIZE 1024 * 1024 * 1
#define CHUNK_SIZE 512

char *cw_get_req_str(int client_fd);

void cw_close_client(int client_fd, char *req_str);

void cw_send_str(int client_fd, int status_code, char *body);

char *cw_get_status_text(int status_code);