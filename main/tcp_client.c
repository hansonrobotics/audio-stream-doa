#include "sdkconfig.h"
#include <string.h>
#include <unistd.h>
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include <errno.h>
#include <netdb.h>            // struct addrinfo
#include <arpa/inet.h>
#include "esp_netif.h"

#include "tcp_client.h"

static const char* TAG = "tcp_client";

tcp_client_t* tcp_client_init(char* ip, int port)
{
    tcp_client_t* tcp_client = malloc(sizeof(tcp_client_t));
    if(tcp_client == NULL){
        return NULL;
    }

    tcp_client->host_ip = ip;
    tcp_client->port = port;

    int addr_family = AF_INET;
    int ip_protocol = IPPROTO_IP;

    // Configure tcp client settings and create respective socket
    inet_pton(addr_family, tcp_client->host_ip, &(tcp_client->dest_addr.sin_addr)); // Set the destination binary IP address
    tcp_client->dest_addr.sin_family = addr_family; // Set the destination address family to IPv4
    tcp_client->dest_addr.sin_port = htons(tcp_client->port); // Set the binary destination port number

    tcp_client->sock =  socket(addr_family, SOCK_STREAM, ip_protocol);
    if (tcp_client->sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        free(tcp_client);
        tcp_client = NULL;
        return NULL;
    }
    ESP_LOGI(TAG, "Socket created");

    // Connect socket created earlier to server ip address + port
    int err = connect(tcp_client->sock, &(tcp_client->dest_addr), sizeof(tcp_client->dest_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Unable to connect socket: errno %d", errno);
        free(tcp_client);
        tcp_client = NULL;
        return NULL;
    }
    ESP_LOGI(TAG, "Socket connected");

    return tcp_client;
}

bool tcp_client_restart(tcp_client_t* tcp_client){
    return false;
    /*
    TODO: Implement a function to restart the tcp client
    */
}

bool tcp_client_send(tcp_client_t* tcp_client, uint8_t* payload, size_t payload_len){
    int err = send(tcp_client->sock, payload, payload_len, 0);
    if (err < 0) {
        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
        return false;
    }
    return true;
}

bool tcp_client_recv(tcp_client_t* tcp_client, uint8_t* rx_buffer, size_t rx_buffer_len){
    int len = recv(tcp_client->sock, rx_buffer, rx_buffer_len - 1, 0);
    if (len < 0) {
        ESP_LOGE(TAG, "recv failed: errno %d", errno);
        return false;
    }
    else {
        rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
        ESP_LOGI(TAG, "Received %d bytes from %s:", len, tcp_client->host_ip);
    }
    return true;
}