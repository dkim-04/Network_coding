#ifndef SNET_H
#define SNET_H

#include <sys/types.h>

int input_hostname(char hostname[], size_t size);
int setup_client(char *hostname, u_short port);
int setup_server(char *hostname, u_short port);
int setup_m_srv(char *hostname, u_short port, int backlog);
int m_accept(int soc, int limit, void (*func)(int, int));
int accept_client(int soc);

#endif
