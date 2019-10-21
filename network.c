#include "network.h"

/* broadcast the message to all clients that are logged in */
void broadcast(int* clients, char* msg, int num_clients) {
    for (int i = 0;i < num_clients;i ++) {
        int ret = send(clients[i], msg, strlen(msg), 0);
        if (ret == -1) {
            printf("send() failed");
        }
    }
}