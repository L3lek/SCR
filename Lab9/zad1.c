#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <openssl/evp.h>

#define MAX_LEN 256
#define MAX_USERS 1000
#define MAX_THREADS 4  // Ustaw maksymalną liczbę wątków

typedef struct {
    char password[33];
    char hashed_password[33]; // 32 characters + null terminator
} Dictionary;

typedef struct {
    char id[MAX_LEN];
    char hashed_password[33]; // 32 characters + null terminator
    char email[MAX_LEN];
    char username[MAX_LEN];
} UserInfo;

typedef struct {
    Dictionary *dictionary;
    UserInfo *hashed_passwords;
    int start;
    int end;
    int num_passwords;  
} ThreadData;

void bytes2md5(const char *data, int len, char *md5buf) {
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_md5();
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len, i;

    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, data, len);
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_free(mdctx);

    for (i = 0; i < md_len; i++) {
        snprintf(&(md5buf[i * 2]), 16 * 2, "%02x", md_value[i]);
    }
}

void read_dictionary(const char *filename, Dictionary *dictionary, int *num_passwords, int hash_password) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    *num_passwords = 0;
    while (*num_passwords < MAX_LEN && fscanf(file, "%s", dictionary[*num_passwords].password) == 1) {

        if (hash_password) {
            // If hash_password is true, hash the password
            bytes2md5(dictionary[*num_passwords].password, strlen(dictionary[*num_passwords].password),
                      dictionary[*num_passwords].hashed_password);
        } else {
            // If hash_password is false, use the password directly
            strcpy(dictionary[*num_passwords].hashed_password, dictionary[*num_passwords].password);
        }
        (*num_passwords)++;
    }

    fclose(file);
}

void read_password_file(const char *filename, UserInfo *users, int *num_users, int hash_password) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    *num_users = 0;
    char line[MAX_LEN];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "%s %s %s %[^\n]", users[*num_users].id,
                   users[*num_users].hashed_password,
                   users[*num_users].email, users[*num_users].username) == 4) {
            (*num_users)++;
        } else {
            fprintf(stderr, "Error parsing line in file\n");
        }
    }
    fclose(file);
}

int count_lines_in_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    int num_lines = 0;
    char line[MAX_LEN];

    while (fgets(line, sizeof(line), file) != NULL) {
        num_lines++;
    }

    fclose(file);
    return num_lines;
}

void *search_passwords(void *thread_data) {
    ThreadData *data = (ThreadData *)thread_data;
    
    for (int i = data->start; i < data->end; i++) {
        for (int j = 0; j < data->num_passwords; j++) {
            if (strcmp(data->dictionary[i].hashed_password, data->hashed_passwords[j].hashed_password) == 0) {
                printf("Password for %s is %s\n", data->hashed_passwords[j].email, data->dictionary[i].password);
            }
        }
    }

    pthread_exit(NULL);
}


int main() {

    Dictionary *dictionary = malloc(count_lines_in_file("slownik.txt") * sizeof(Dictionary));
    UserInfo hashed_passwords[MAX_USERS];
    int num_dictionary, num_passwords;

    read_dictionary("slownik.txt", dictionary, &num_dictionary, 1); // Hash the passwords

    read_password_file("1.txt", hashed_passwords, &num_passwords, 1);

    pthread_t threads[MAX_THREADS];
    ThreadData thread_data[MAX_THREADS];

    int chunk_size = num_dictionary / MAX_THREADS;
    int remainder = num_dictionary % MAX_THREADS;

    for (int i = 0; i < MAX_THREADS; i++) {
        thread_data[i].dictionary = dictionary;
        thread_data[i].hashed_passwords = hashed_passwords;
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i == MAX_THREADS - 1) ? (i + 1) * chunk_size + remainder : (i + 1) * chunk_size;
        thread_data[i].num_passwords = num_passwords;  // Przypisanie wartości

        pthread_create(&threads[i], NULL, search_passwords, (void *)&thread_data[i]);
    }

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    free(dictionary);

    return 0;
}