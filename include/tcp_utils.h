#ifndef TCP_UTILS_H
#define TCP_UTILS_H
#include "common.h"
#define TCP_MAX_MSG_LEN 4096

Error tcp_utils_server_init();
Error tcp_utils_accept();
int tcp_utils_get_client_socket();
void tcp_utils_close_server_socket();
void tcp_utils_close_client_socket();
Error tcp_utils_read(char*);
Error tcp_utils_write(char*);
Error tcp_utils_send_file(char*, long);

#endif /* TCP_UTILS_H */
