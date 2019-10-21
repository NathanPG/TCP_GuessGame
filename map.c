#include "map.h"

int hash(int key) {
    return key;
}

/* add a socket-username pair to the map */
int add_to_entries(struct Connection* conn, char* username, int sd) {
    int index = hash(sd) % conn->heap_size;
    struct Entry* entry = (struct Entry*)malloc(sizeof(struct Entry));
    entry->client_sd = sd;
    entry->username = username;
    while (conn->entries[index] != NULL) {
        if (conn->entries[index]->client_sd == -1) {
            free(conn->entries[index]);
            break;
        }
        index ++;
        index = index % conn->heap_size;
    }
    conn->entries[index] = entry;
    conn->num_connected ++;
    return 0;
}

/* change the value of an existing entry in the map*/
int change_value(struct Connection* conn, char* new_val, int sd) {
    int index = find(conn, sd);
    if (index == -1) return -1;
    conn->entries[index]->username = new_val;
    return 0;
}

/* remove an entry from the map */
int remove_from_entries(struct Connection* conn, int sd) {
    int index = find(conn, sd);
    if (index == -1) {
        return -1;
    }
    struct Entry* entry = (struct Entry*)malloc(sizeof(struct Entry));
    entry->client_sd = -1;
    entry->username = "";
    free(conn->entries[index]);
    conn->entries[index] = entry;
    conn->num_connected --;
    return 0;
}

/* find the index of the username associated with the socket descriptor */
int find(struct Connection* conn, int sd) {
    int index = hash(sd) % conn->heap_size;
    if (conn->entries[index] == NULL) {
        return -1;
    }
    int i = 0;
    while(conn->entries[index]->client_sd != sd) {
        if (i == conn->heap_size) {
            return -1;
        }
        index ++;
        index = index % conn->heap_size;
        i ++;
        if (conn->entries[index] == NULL) return -1;
    }
    return index;
}

/* find the username associated with the socket descriptor */
char* lookup(struct Connection* conn, int sd) {
    int index = find(conn, sd);
    if (index == -1) return NULL;
    else return conn->entries[index]->username;
}

void print_table(struct Connection* conn) {
    for(int i = 0;i < conn->heap_size;i ++) {
        if (conn->entries[i] == NULL) {
            // printf("Index %d: NULL\n", i);
            continue;
        } else {
            if (conn->entries[i]->client_sd == -1) continue;
            printf("Index %d: (%s, %d)\n", i, conn->entries[i]->username, conn->entries[i]->client_sd);
        }
    }
}

/* return all keys(socket descriptor) in the map as an integer array */
int* keys(struct Connection* conn) {
    int* sds = (int*)malloc(sizeof(int) * conn->heap_size);
    int n = 0;
    for (int i = 0;i < conn->heap_size;i ++) {
        if (conn->entries[i] != NULL && conn->entries[i]->client_sd != -1) {
            sds[n] = conn->entries[i]->client_sd;
            n ++;
        }
    }
    return sds;
}

/* initialize the map */
void initialize(struct Connection* conn) {
    conn->entries = (struct Entry**)malloc(sizeof(struct Entry*) * 64);
    for (int i = 0;i < INITIAL_SIZE;i ++) {
        conn->entries[i] = NULL;
    }
    conn->num_connected = 0;
    conn->heap_size = INITIAL_SIZE;
}

/* free up all dynamically allocated memories in the map */
void cleanup(struct Connection* conn) {
    for (int i = 0;i < conn->heap_size;i ++) {
        free(conn->entries[i]);
    }
    free(conn->entries);
}

/* get all socket descriptors in the map that is logged in with a valid username */
int get_logged_in(struct Connection* conn, int** clients) {
    int* connected = (int*)malloc(sizeof(int) * INITIAL_SIZE);
    int count = 0;
    for (int i = 0;i < conn->heap_size;i ++) {
        if (conn->entries[i] != NULL && strcmp(conn->entries[i]->username, "") != 0) {
            connected[count] = conn->entries[i]->client_sd;
            count ++;
        }
    }
    *clients = connected;
    return count;
}

// int main() {
//     struct Connection* conn = (struct Connection*)malloc(sizeof(struct Connection));
//     initialize(conn);
//     add_to_entries(conn, "xur2", 4);
//     add_to_entries(conn, "frankx", 5);
//     add_to_entries(conn, "bob", 6);
//     remove_from_entries(conn, 6);
//     add_to_entries(conn, "bob", 11);
//     change_value(conn, "frankxur2", 5);
//     print_table(conn);
//     printf("hash test: %s\n", lookup(conn, 11));
//     cleanup(conn);
//     return 0;
// }