#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <openssl/evp.h>
#include <ctype.h>

#define MAX_LEN 256
#define MAX_USERS 1000
#define MAX_THREADS 16 // Ustaw maksymalną liczbę wątków

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

void read_dictionary(const char *filename, Dictionary *dictionary, int *num_passwords) {
	int i=0;
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

				char temp[MAX_LEN];
				char temp_hashed[33];
				char list[3][MAX_LEN];


				snprintf(temp, MAX_LEN, "%s", data->dictionary[i].password);
				strcpy(list[0], temp);
				bytes2md5(temp, strlen(temp), temp_hashed);
				if (strcmp(temp_hashed, data->hashed_passwords[j].hashed_password) == 0) {
					printf("Password for %s is %s\n", data->hashed_passwords[j].email, temp);
				}

				snprintf(temp, MAX_LEN, "%s", data->dictionary[i].password);
				for (int k = 0; temp[k] != '\0'; k++) {
					temp[k] = toupper(temp[k]);
				}
				strcpy(list[1], temp);
				bytes2md5(temp, strlen(temp), temp_hashed);
				if (strcmp(temp_hashed, data->hashed_passwords[j].hashed_password) == 0) {
					printf("Password for %s is %s\n", data->hashed_passwords[j].email, temp);
				}

				snprintf(temp, MAX_LEN, "%s", data->dictionary[i].password);
				temp[0] = toupper(temp[0]);
				strcpy(list[2], temp);
				bytes2md5(temp, strlen(temp), temp_hashed);
				if (strcmp(temp_hashed, data->hashed_passwords[j].hashed_password) == 0) {
					printf("Password for %s is %s\n", data->hashed_passwords[j].email, temp);
				}

				for(int a=0;a<3;a++){
					for(int k=0;k<100;k++){
					snprintf(temp, MAX_LEN, "%d%s", k, list[a]);
					bytes2md5(temp, strlen(temp), temp_hashed);
					if (strcmp(temp_hashed, data->hashed_passwords[j].hashed_password) == 0) {
						printf("Password for %s is %s\n", data->hashed_passwords[j].email, temp);
						break;
					}
					
					snprintf(temp, MAX_LEN, "%s%d", list[a], k);
					bytes2md5(temp, strlen(temp), temp_hashed);
					if (strcmp(temp_hashed, data->hashed_passwords[j].hashed_password) == 0) {
						printf("Password for %s is %s\n", data->hashed_passwords[j].email, temp);
						break;
					}

					for (int l = 0; l < 100; l++) {
							snprintf(temp, MAX_LEN, "%d%s%d", l, list[a], k);
							char temp_hashed[MAX_LEN];
							bytes2md5(temp, strlen(temp), temp_hashed);
							if (strcmp(temp_hashed, data->hashed_passwords[j].hashed_password) == 0) {
							printf("Password for %s is %s\n", data->hashed_passwords[j].email, temp);
							break;
							}
						}
					}
				}
			}
		}

    pthread_exit(NULL);
}


int main() {

    Dictionary *dictionary = malloc(count_lines_in_file("xd.txt") * sizeof(Dictionary));
    UserInfo hashed_passwords[MAX_USERS];
    int num_dictionary, num_passwords;

    read_dictionary("xd.txt", dictionary, &num_dictionary);

    read_password_file("4.txt", hashed_passwords, &num_passwords);

    pthread_t threads[MAX_THREADS];
    ThreadData thread_data[MAX_THREADS];

    int chunk_size = num_dictionary / MAX_THREADS;
    int remainder = num_dictionary % MAX_THREADS;

    for (int i = 0; i < MAX_THREADS; i++) {
        thread_data[i].dictionary = dictionary;
        thread_data[i].hashed_passwords = hashed_passwords;
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i == MAX_THREADS - 1) ? (i + 1) * chunk_size + remainder : (i + 1) * chunk_size;
        thread_data[i].num_passwords = num_passwords; 

        pthread_create(&threads[i], NULL, search_passwords, (void *)&thread_data[i]);
    }

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    free(dictionary);

    return 0;
}