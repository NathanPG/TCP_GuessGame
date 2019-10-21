# include "utilities.h"

/* remove all \n from the string */
void remove_white_space(char** str) {
    char* modified = (char*)malloc(sizeof(char) * MAX_WORD_LEN);
    int i = 0, j = 0;
    while((*str)[i] != '\0') {
        if ((*str)[i] != '\n') {
            modified[j] = (*str)[i];
            j ++;
        }
        i ++;
    }
    *str = modified;
}

/* convert the string to all lower case */
void lower(char** str) {
    char* modified = (char*)malloc(sizeof(char) * MAX_WORD_LEN);
    int i = 0;
    while((*str)[i] != '\0') {
        modified[i] = tolower((*str)[i]);
        i ++;
    }
    *str = modified;
}

/* case insensitive compare of 2 strings */
int str_compare(char* str1, char* str2) {
    char* lstr1 = (char*)malloc(sizeof(char) * (strlen(str1) + 1));
    char* lstr2 = (char*)malloc(sizeof(char) * (strlen(str2) + 1));
    strcpy(lstr1, str1);
    strcpy(lstr2, str2);
    lower(&lstr1);
    lower(&lstr2);
    int rc = strcmp(lstr1, lstr2);
    free(lstr2);
    free(lstr1);
    return rc;
}

/* str1 is the guessed word; str2 is the secret word */
/* count the number of letters that are in both str1 and str2 */ 
int str_num_correct(char* str1, char* str2) {
    int count = 0;
    char table1[256];
    char char_count[256];
    // char table2[256];
    memset(table1, '\0', 256);
    memset(char_count, 0, 256);
    // memset(table2, '\0', 256);
    for (int i = 0;i < strlen(str2);i ++) {
        char_count[hash_char(str2[i])] ++;
    }
    for (int i = 0;i < strlen(str1);i ++) {
        if (char_count[hash_char(str1[i])] != 0) {
            count ++;
            char_count[hash_char(str1[i])] --;
        }
    }
    // for (int i = 0;i < strlen(str2);i ++) {
    //     table2[hash_char(str2[i])] = str2[i];
    // }
    // for (int i = 0;i < 256;i ++) {
    //     if (table1[i] != '\0' && table2[i] != '\0') {
    //         count ++;
    //     } 
    // }
    // for (int i = 0;i < strlen(str2);i ++) {
    //     if (table1[hash_char(str2[i])] != '\0') {
    //         count ++;
    //     }
    // }
    // for (int i = 0;i < strlen(str1);i ++) {
    //     for (int j = 0;j < strlen(str2);j ++) {
    //         if (tolower(str1[i]) == tolower(str2[j])) {
    //             count ++;
    //             break;
    //         }
    //     }
    // }
    return count;
}

/* str1 is the guessed word; str2 is the secret word */
/* count the number of letters that are correctly placed in str1. */
int str_num_correct_placed(char* str1, char* str2) {
    int count = 0;
    for (int i = 0;i < strlen(str1) && i < strlen(str2);i ++) {
        if (tolower(str1[i]) == tolower(str2[i])) count ++;
    }
    return count;
}

/* load the dictionary from the file */
int load_dict(char* filename, char*** dictionary) {
    FILE* dict_file = fopen(filename, "r");
    char** tmp_dict = (char**)malloc(sizeof(char*) * MAX_DICT_LEN);
    for (int i = 0;i < MAX_DICT_LEN;i ++) {
        tmp_dict[i] = (char*)malloc(sizeof(char) * MAX_WORD_LEN);
    }
    int count = 0;
    while(fgets(tmp_dict[count], MAX_WORD_LEN, dict_file)) {
        remove_white_space(&tmp_dict[count]);
        count ++;
    }
    *dictionary = tmp_dict;
    return count;
}

int hash_char(char c) {
    return (int)c;
}

// int main() {
//     char* test = "asfasd\n";
//     printf("%s\n", test);
//     remove_white_space(&test);
//     printf("%s\n", test);
//     char* upper = "BAD";
//     char* l = "bad";
//     // printf("%d\n", str_compare(upper, l));
//     printf("num correct: %d\n", str_num_correct("aaaaa", "untap"));
//     printf("num correcy place: %d\n", str_num_correct_placed("aaaaa", "untap"));
//     // char* file = "linux.words";
//     // char** dict;
//     // int count = load_dict(file, &dict);
//     // for(int i = 0;i < count;i ++) {
//     //     printf("%s\n", dict[i]);
//     // }
//     // printf("%s\n", upper);
//     // lower(&upper);
//     // printf("%s\n", upper);
// }