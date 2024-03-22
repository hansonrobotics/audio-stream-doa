#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

typedef struct {
    int sock;
    char* host_ip;
    int port;
    struct sockaddr_in dest_addr;
} tcp_client_t;

tcp_client_t* tcp_client_init(char* ip, int port);
bool tcp_client_restart(tcp_client_t* tcp_client);
bool tcp_client_send(tcp_client_t* client, uint8_t* payload, size_t payload_len);
bool tcp_client_recv(tcp_client_t* tcp_client, uint8_t* rx_buffer, size_t rx_buffer_len);