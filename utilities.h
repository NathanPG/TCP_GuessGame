/* functions that handles strings */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_WORD_LEN 1024
#define MAX_DICT_LEN 499330

void remove_white_space(char** str);
void lower(char** str);
int str_compare(char* str1, char* str2);
int str_num_correct(char* str1, char* str2);
int str_num_correct_placed(char* str1, char* str2);
int load_dict(char* filename, char*** dictionary);
int hash_char(char c);