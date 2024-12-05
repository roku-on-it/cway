#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../include/http_utils.h"

char *cw_get_req_str(int client_fd) {
  ssize_t total_read = 0;

  char *req_str = calloc(MAX_REQ_SIZE, sizeof(char));
  if (req_str == NULL) {
    perror("Could not reallocate");
    cw_send_str(client_fd, 500, "Internal Server Error");
  }

  while (1) {
    char temp_buf[CHUNK_SIZE] = {0};
    ssize_t read_bytes = recv(client_fd, temp_buf, CHUNK_SIZE, 0);
    if (read_bytes < 0) {
      perror("Recv failed");
      cw_send_str(client_fd, 500, "Internal Server Error");
    }

    if (read_bytes == 0) {
      break;
    }

    if (read_bytes == -1) {
      perror("Recv failed");
      cw_send_str(client_fd, 500, "Internal Server Error");
    }

    total_read += read_bytes;
    req_str = realloc(req_str, total_read);
    if (req_str == NULL) {
      perror("Could not reallocate");
      cw_send_str(client_fd, 500, "Internal Server Error");
    }

    strncat(req_str, temp_buf, read_bytes);

    if (read_bytes < CHUNK_SIZE) {
      break;
    }

    if (total_read > MAX_REQ_SIZE) {
      cw_send_str(client_fd, 413, "Content Too Large");
      cw_close_client(client_fd, req_str);
    }
  }

  if (total_read < 8) {
    cw_send_str(client_fd, 405, "Method Not Allowed");
    cw_close_client(client_fd, req_str);
  }

  return req_str;
}

void cw_close_client(int client_fd, char *req_str) {
  close(client_fd);
  free(req_str);
  pthread_exit(NULL);
}

void cw_send_str(int client_fd, int status_code, char *body) {
  char response[MAX_REQ_SIZE] = {0};
  sprintf(response,
          "HTTP/1.1 %d %s\nContent-Type: text/plain\nContent-Length: %ld\n\n%s",
          status_code, cw_get_status_text(status_code), strlen(body), body);
  send(client_fd, response, strlen(response), 0);
}

char *cw_get_status_text(int status_code) {
  switch (status_code) {
  case 100:
    return "Continue";
  case 101:
    return "Switching Protocols";
  case 102:
    return "Processing";
  case 103:
    return "Early Hints";
  // 200 - 299
  case 200:
    return "OK";
  case 201:
    return "Created";
  case 202:
    return "Accepted";
  case 203:
    return "Non-Authoritative Information";
  case 204:
    return "No Content";
  case 205:
    return "Reset Content";
  case 206:
    return "Partial Content";
  case 207:
    return "Multi-Status";
  case 208:
    return "Already Reported";
  case 226:
    return "IM Used";
  //- 300 - 399
  case 300:
    return "Multiple Choices";
  case 301:
    return "Moved Permanently";
  case 302:
    return "Found";
  case 303:
    return "See Other";
  case 304:
    return "Not Modified";
  case 305:
    return "Use Proxy";
  case 306:
    return "unused";
  case 307:
    return "Temporary Redirect";
  case 308:
    return "Permanent Redirect";
  // 400 - 499
  case 400:
    return "Bad Request";
  case 401:
    return "Unauthorized";
  case 402:
    return "Payment Required";
  case 403:
    return "Forbidden";
  case 404:
    return "Not Found";
  case 405:
    return "Method Not Allowed";
  case 406:
    return "Not Acceptable";
  case 407:
    return "Proxy Authentication Required";
  case 408:
    return "Request Timeout";
  case 409:
    return "Conflict";
  case 410:
    return "Gone";
  case 411:
    return "Length Required";
  case 412:
    return "Precondition Failed";
  case 413:
    return "Payload Too Large";
  case 414:
    return "URI Too Long";
  case 415:
    return "Unsupported Media Type";
  case 416:
    return "Range Not Satisfiable";
  case 417:
    return "Expectation Failed";
  case 418:
    return "I'm a teapot";
  case 421:
    return "Misdirected Request";
  case 422:
    return "Unprocessable Content";
  case 423:
    return "Locked";
  case 424:
    return "Failed Dependency";
  case 425:
    return "Too Early";
  case 426:
    return "Upgrade Required";
  case 428:
    return "Precondition Required";
  case 429:
    return "Too Many Requests";
  case 431:
    return "Request Header Fields Too Large";
  case 451:
    return "Unavailable For Legal Reasons";
    // 400 - 499
  case 500:
    return "Internal Server Error";
  case 501:
    return "Not Implemented";
  case 502:
    return "Bad Gateway";
  case 503:
    return "Service Unavailable";
  case 504:
    return "Gateway Timeout";
  case 505:
    return "HTTP Version Not Supported";
  case 506:
    return "Variant Also Negotiates";
  case 507:
    return "Insufficient Storage";
  case 508:
    return "Loop Detected";
  case 510:
    return "Not Extended";
  case 511:
    return "Network Authentication Required";
  default:
    return "UNKNOWN";
  }
}