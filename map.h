#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

# define INITIAL_SIZE 64

struct Entry {
    char* username;
    int client_sd;
};

struct Connection {
    struct Entry** entries;
    int num_connected;
    int heap_size;
};

int hash(int key);
int add_to_entries(struct Connection* conn, char* username, int sd);
int change_value(struct Connection* conn, char* new_val, int sd);
int remove_from_entries(struct Connection* conn, int sd);
int find(struct Connection* conn, int sd);
char* lookup(struct Connection* conn, int sd);
void print_table(struct Connection* conn);
int* keys(struct Connection* conn);
void initialize(struct Connection* conn);
void cleanup(struct Connection* conn);
int get_logged_in(struct Connection* conn, int** clients);