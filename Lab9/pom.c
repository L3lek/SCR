#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <openssl/evp.h>
#include <ctype.h>

#define MAX_LEN 256
#define MAX_USERS 1000
#define MAX_THREADS 4 // Ustaw maksymalną liczbę wątków

pthread_barrier_t barrier; 

char* checked_users[MAX_USERS];  // Lista globalna sprawdzonych użytkowników
int num_matched_users = 0;
int race=0;
int isUserMatched = 0; 
char c;

typedef struct {
    char password[33];
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
    int num_dictionary;
    int num_passwords;
    int pom;
    pthread_mutex_t mutex;
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

void read_dictionary(const char *filename, Dictionary *dictionary, int *num_passwords) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    *num_passwords = 0;
    while (fscanf(file, "%s", dictionary[*num_passwords].password) == 1) {
        (*num_passwords)++;
    }

    fclose(file);
}

void read_password_file(const char *filename, UserInfo *users, int *num_users) {
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
            fprintf(stderr, "Błąd, złe dane użytkownika o id: %s\n", users[*num_users].id);
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

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

void toUpperCase(char *word) {
    for (int i = 0; word[i] != '\0'; i++) {
        word[i] = toupper(word[i]);
    }
}

void toUpperFirst(char *word) {
        word[0] = toupper(word[0]);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

void checkPassword(int num, char *hashedPassword, UserInfo *user, char *originalPassword) {
    if (strcmp(hashedPassword, user->hashed_password) == 0) {
        printf("%d: Password for %s is %s\n", num, user->email, originalPassword);
        if (num_matched_users < MAX_USERS) {
            checked_users[num_matched_users] = strdup(user->email);
            num_matched_users++;
        }
    }
}

//Bez liczb
void search_original(ThreadData *data, int j, int i) {
    char temp[MAX_LEN];
    char temp_hashed[33];

                for (int k = 0; k < num_matched_users; k++) {
                    if (strcmp(data->hashed_passwords[j].id, checked_users[k]) == 0) {
                        isUserMatched = 1;
                        return;
                    }
                }

            if (!isUserMatched) {
                snprintf(temp, MAX_LEN, "%s", data->dictionary[i].password);
                bytes2md5(temp, strlen(temp), temp_hashed);
                checkPassword(data->pom, temp_hashed, &data->hashed_passwords[j], temp);
            }
        }

//////Postfiks

void search_original_postfix(ThreadData *data, int j, int i) {
    char temp[MAX_LEN];
    char temp_with_postfix[MAX_LEN];
    char temp_hashed[33];
            
                for (int k = 0; k < num_matched_users; k++) {
                    if (strcmp(data->hashed_passwords[j].id, checked_users[k]) == 0) {
                        isUserMatched = 1;
                        return;
                    }
                }

            if (!isUserMatched) {
                snprintf(temp, MAX_LEN, "%s", data->dictionary[i].password);
                for (int k = 0; k < 100; k++) {
                    if (!isUserMatched) {
                    snprintf(temp_with_postfix, MAX_LEN, "%s%d", temp, k);
                    bytes2md5(temp_with_postfix, strlen(temp_with_postfix), temp_hashed);
                    checkPassword(4,temp_hashed, &data->hashed_passwords[j], temp_with_postfix);
                    }else{
                        return;
                    }
                }
            }
        }

//////Prefix

void search_original_prefix(ThreadData *data, int j, int i) {
    char temp[MAX_LEN];
    char temp_with_postfix[MAX_LEN];
    char temp_hashed[33];
            for (int k = 0; k < num_matched_users; k++) {
                    if (strcmp(data->hashed_passwords[j].id, checked_users[k]) == 0) {
                        isUserMatched = 1;
                        return;
                    }
                }

            if (!isUserMatched) {
                snprintf(temp, MAX_LEN, "%s", data->dictionary[i].password);
                for (int k = 0; k < 100; k++) {
                    if (!isUserMatched) {
                    snprintf(temp_with_postfix, MAX_LEN, "%d%s",k, temp);
                    bytes2md5(temp_with_postfix, strlen(temp_with_postfix), temp_hashed);
                    checkPassword(7,temp_hashed, &data->hashed_passwords[j], temp_with_postfix);
                    }else{
                        return;
                    }
                }
            }
        }

//////Full

void search_original_full(ThreadData *data, int j, int i) {
    char temp[MAX_LEN];
    char temp_with_postfix[MAX_LEN];
    char temp_hashed[33];
            for (int k = 0; k < num_matched_users; k++) {
                    if (strcmp(data->hashed_passwords[j].id, checked_users[k]) == 0) {
                        isUserMatched = 1;
                        return;
                    }
                }

            if (!isUserMatched) {
                snprintf(temp, MAX_LEN, "%s", data->dictionary[i].password);
                for (int k = 0; k < 100; k++) {
                    for (int l = 0; l < 100; l++) {
                        if (!isUserMatched) {
                        snprintf(temp_with_postfix, MAX_LEN, "%d%s%d",k, temp, l);
                        bytes2md5(temp_with_postfix, strlen(temp_with_postfix), temp_hashed);
                        checkPassword(10,temp_hashed, &data->hashed_passwords[j], temp_with_postfix);
                        }else{
                            return;
                        }
                    }
                }
            }
        }

void *search_passwords(void *thread_data) {
    ThreadData *data = (ThreadData *)thread_data;
    int local_race;

        pthread_mutex_lock(&data->mutex);
        ++race;
        local_race=race;
        pthread_mutex_unlock(&data->mutex);

		for(int i=0; i<data->num_passwords;i++){
            while (local_race<=data->num_dictionary){
                char* word = data->dictionary[race].password;
                if(!isUserMatched){
                    search_original(data,i,local_race);
                }else{
                    break;
                }
                if(!isUserMatched){
                    search_original_prefix(data,i,local_race);
                }else{
                    break;
                }
                if(!isUserMatched){
                    search_original_postfix(data,i,local_race);
                }else{
                    break;
                }
                if(!isUserMatched){
                    search_original_full(data,i,local_race);
                }else{
                    break;
                }
                if(!isUserMatched){
                pthread_mutex_lock(&data->mutex);
                race++;
                local_race=race;
                pthread_mutex_unlock(&data->mutex);
                }
            }
            
            pthread_barrier_wait(&barrier);
            
            pthread_mutex_lock(&data->mutex);
            isUserMatched = 0;
            race=0;
            local_race=race;
            ++race;
            pthread_mutex_unlock(&data->mutex);
    }
    pthread_exit(NULL);
}


int main() {

    Dictionary *dictionary = malloc(count_lines_in_file("slownik.txt") * sizeof(Dictionary));
    UserInfo hashed_passwords[MAX_USERS];
    int num_dictionary, num_passwords;

    read_dictionary("slownik.txt", dictionary, &num_dictionary);

    read_password_file("2.txt", hashed_passwords, &num_passwords);

    pthread_t threads[MAX_THREADS];
    ThreadData thread_data[MAX_THREADS];

    for (int i = 0; i < MAX_THREADS; i++) {
        thread_data[i].dictionary = dictionary;
        thread_data[i].hashed_passwords = hashed_passwords;
        thread_data[i].num_dictionary = num_dictionary;
        thread_data[i].num_passwords = num_passwords;
        thread_data[i].pom=i;
        pthread_barrier_init(&barrier, NULL, MAX_THREADS);
        pthread_mutex_init(&thread_data->mutex, NULL);

		pthread_create(&threads[i], NULL, search_passwords, (void *)&thread_data[i]);
    }

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    free(dictionary);

    return 0;
}