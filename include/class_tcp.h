#ifndef CLASS_TCP_H
#define CLASS_TCP_H
#include "common.h"
#define TCP_MAX_MSG_LEN 512

Error Tcp_server_init();
Error Tcp_accept();
int Tcp_get_client_socket();
void Tcp_close_server_socket();
void Tcp_close_client_socket();
Error Tcp_read(char*);

#endif /* CLASS_TCP_H */
