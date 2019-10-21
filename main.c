/* main.c */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/select.h>  
#include <netdb.h> 

#include "map.h"
#include "utilities.h"
#include "network.h"

/* Predefined constants for array sizes */
#define BUFFERSIZE 1024
#define MAX_CLIENTS 32

/* Predefined messages to be sent.*/
#define GUESS_CORRECT "%s has correctly guessed the word %s\n"
#define INVALID_GUESS "Invalid guess length. The secret word is %lu letter(s).\n"
#define WRONG_GUESS "%s guessed %s: %d letter(s) were correct and %d letter(s) were correctly placed.\n"
#define CONFIRM_LOGIN "Let's start playing, %s\n"
#define GAME_INFO "There are %d player(s) playing. The secret word is %lu letter(s).\n"
#define USERNAME_IN_USE "Username %s is already taken, please enter a different username\n"
#define WELCOME "Welcome to Guess the Word, please enter your username.\n"


#define USAGE_ERROR "usage: ./word_guess.out [seed] [port] [dictionary_file] [longest_word_length]\n"

unsigned short port;

int main(int argc, char *argv[])
{
	if (argc != 5) {
		fprintf(stderr, USAGE_ERROR);
		return EXIT_FAILURE;
	}

	/* load dictionary */
	char* dict_file = argv[3];
	char** dict;
	int num_words = load_dict(dict_file, &dict);
	int seed = atoi(argv[1]);
	srand(seed);
	port = (unsigned short)atoi(argv[2]);
	int rand_index = rand() % num_words;
	char* secret_word = dict[rand_index];

	/* setup the connection */
	fd_set readfds;
	int sd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( sd == -1 ){
		printf( "socket() failed" );
		return EXIT_FAILURE;
	}
	struct sockaddr_in server;
	struct sockaddr_in client;
  	int fromlen = sizeof( client );
  	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl( INADDR_ANY );
	server.sin_port = htons( port );
	int len = sizeof( server );
	struct Connection* conn = (struct Connection*)malloc(sizeof(struct Connection));
	if ( bind( sd, (struct sockaddr *)&server, len ) == -1 ){
		perror( "bind() failed\n" );
		return EXIT_FAILURE;
	}
	if ( listen( sd, 5 ) == -1 ){
		perror( "listen() failed\n" );
		return EXIT_FAILURE;
	}

	initialize(conn);

	while(1){
		/* reset select */
		struct timeval timeout;
		timeout.tv_sec = 0;
    	timeout.tv_usec = 1;
    	FD_ZERO( &readfds );
    	FD_SET( sd, &readfds );
		int* connected_clients = keys(conn);
		for (int i = 0;i < conn->num_connected;i ++) {
			FD_SET(connected_clients[i], &readfds);
		}
		int ready = select( FD_SETSIZE, &readfds, NULL, NULL, &timeout );
		/* if no socket has any activities, skip this iteration */
		if (ready == 0) {
			continue;
		}
		/* if the listen() socket has received new connection */
		if (FD_ISSET(sd, &readfds)) {
			int newsd = accept( sd, (struct sockaddr *)&client, (socklen_t *)&fromlen );
			if (newsd == -1) {
				perror("accept() failed\n");
			}
			int ret = send(newsd, WELCOME, strlen(WELCOME), 0);
			if (ret == -1) {
				perror("send() failed\n");
			}
			add_to_entries(conn, "", newsd);
		}

		/* loop through the connected clients to see if there is any activity. */
		for (int i = 0;i < conn->num_connected;i ++) {
			int activity = connected_clients[i];
			if (FD_ISSET(activity, &readfds)) {
				char* buffer = (char*)malloc(sizeof(char) * BUFFERSIZE);
				int n = recv(activity, buffer, BUFFERSIZE, 0);
				if (n == -1){
					perror("recv() failed\n");
				} 
				
				/* client closed the connection */
				else if (n == 0) {
					remove_from_entries(conn, activity);
					close(activity);
				} 
				
				/* receive data from client */
				else {
					buffer[n] = '\0';
					char* username = lookup(conn, activity);
					if (username != NULL) {
						remove_white_space(&buffer);
						int dup = 0;

						/* if this client is not logged in */
						if (str_compare(username, "") == 0) {
							/* check if the username has been taken before */
							for (int j = 0;j < conn->num_connected;j ++) {
								if (str_compare(lookup(conn, connected_clients[j]), buffer) == 0) {
									dup = 1;
								}
							}

							/* if the username is in user, make it pick another one */
							if (dup) {
								char repick[BUFFERSIZE];
								sprintf(repick, USERNAME_IN_USE, buffer);
								send(activity, repick, strlen(repick), 0);
							} 
							
							/* otherwise, register it in the socket-username map */
							else {
								change_value(conn, buffer, activity);
								char confirm_msg[BUFFERSIZE];
								char game_info[BUFFERSIZE];
								int* logged_in;
								int num_logged_in = get_logged_in(conn, &logged_in);
								sprintf(confirm_msg, CONFIRM_LOGIN, buffer);
								sprintf(game_info, GAME_INFO, num_logged_in, strlen(secret_word));
								send(activity, confirm_msg, strlen(confirm_msg), 0);
								send(activity, game_info, strlen(game_info), 0);
								free(logged_in);
							}
						} 
						/* if the client is logged in, guess word starts */
						else {
							int* logged_in;
							int num_logged_in = get_logged_in(conn, &logged_in);
							/* if the guess matches the secret word */
							if (str_compare(buffer, secret_word) == 0) {
								char msg[BUFFERSIZE];
								sprintf(msg, GUESS_CORRECT, lookup(conn, activity), secret_word);
								broadcast(logged_in, msg, num_logged_in);
								int n_conn = conn->num_connected;
								for (int j = 0;j < n_conn;j ++) {
									if (close(connected_clients[j]) == -1) {
										perror("close() failed\n");
									}
									remove_from_entries(conn, connected_clients[j]);
								}
								free(secret_word);
								rand_index = rand() % num_words;
								secret_word = dict[rand_index];
								break;
							} 
							/* if the guess doesn't match the secret word*/
							else {
								if(strlen(buffer) != strlen(secret_word)) {
									char msg[BUFFERSIZE];
									sprintf(msg, INVALID_GUESS, strlen(secret_word));
									send(activity, msg, strlen(msg), 0);
								} else {
									int num_correct_letters = str_num_correct(buffer, secret_word);
									int num_correct_placed = str_num_correct_placed(buffer, secret_word);
									char msg[BUFFERSIZE];
									sprintf(msg, WRONG_GUESS, lookup(conn, activity), buffer, num_correct_letters, num_correct_placed);
									broadcast(logged_in, msg, num_logged_in);
								}
							}

						}

					}
				}
			}
		}
	}
	return 0;
}