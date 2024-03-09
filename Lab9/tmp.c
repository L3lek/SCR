#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <openssl/evp.h>
#include <ctype.h>
#include <signal.h>

#define MAX_LEN 256
#define MAX_USERS 1000
#define MAX_THREADS 10 // Ustaw maksymalną liczbę wątków

pthread_barrier_t barrier; 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t conditional = PTHREAD_COND_INITIALIZER;

char* checked_users[MAX_USERS]; 
char* found_passwords[MAX_USERS];
int num_matched_users = 0;
int shown_passwords=0;
int race=0;
int finish=0;
int finish_sig=0;

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
    int is_matched;
    int searched_password;
} ThreadData;

void handleSIGHUP(int signum) {
    printf("\nPasswords found:\n");
    for (int i = 0; i < num_matched_users; i++) {
        printf("%s: %s\n", checked_users[i], found_passwords[i]);
    }

    finish_sig = 1;

    exit(0);
}

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

void checkPassword(char *hashedPassword, UserInfo *user, char *originalPassword) {
    if (strcmp(hashedPassword, user->hashed_password) == 0) {
        pthread_mutex_lock(&mutex);
        checked_users[num_matched_users] = strdup(user->email);
        found_passwords[num_matched_users]=strdup(originalPassword);
        num_matched_users++;
        pthread_cond_signal(&conditional);
        pthread_mutex_unlock(&mutex);
    }
}

//Bez liczb
void search_original(ThreadData *data, int j, int i) {
    char temp[MAX_LEN];
    char temp_hashed[33];

                for (int k = 0; k < num_matched_users; k++) {
                    if (strcmp(data->hashed_passwords[j].email, checked_users[k]) == 0) {
                        data->is_matched = 1;
                        return;
                    }
                }

            if ((data->is_matched || finish_sig)==0) {
                snprintf(temp, MAX_LEN, "%s", data->dictionary[i].password);
                bytes2md5(temp, strlen(temp), temp_hashed);
                checkPassword(temp_hashed, &data->hashed_passwords[j], temp);
            }
        }

void search_upper(ThreadData *data, int j, int i){
	char temp[MAX_LEN];
	char temp_hashed[33];

            for (int k = 0; k < num_matched_users; k++) {
                if (strcmp(data->hashed_passwords[j].email, checked_users[k]) == 0) {
                    data->is_matched = 1;
                    return;
                }
            }
            if ((data->is_matched || finish_sig)==0) {
                snprintf(temp, MAX_LEN, "%s", data->dictionary[i].password);
                toUpperCase(temp);
                bytes2md5(temp, strlen(temp), temp_hashed);
                checkPassword(temp_hashed, &data->hashed_passwords[j], temp);
           }
		}

void search_first(ThreadData *data, int j, int i){
	char temp[MAX_LEN];
	char temp_hashed[33];
            for (int k = 0; k < num_matched_users; k++) {
                if (strcmp(data->hashed_passwords[j].email, checked_users[k]) == 0) {
                    data->is_matched = 1;
                    return;
                }
            }

            if ((data->is_matched || finish_sig)==0) {
                snprintf(temp, MAX_LEN, "%s", data->dictionary[i].password);
                toUpperFirst(temp);
                bytes2md5(temp, strlen(temp), temp_hashed);
                checkPassword(temp_hashed, &data->hashed_passwords[j], temp);
            }
		}

//////Postfiks

void search_original_postfix(ThreadData *data, int j, int i) {
    char temp[MAX_LEN];
    char temp_with_postfix[MAX_LEN];
    char temp_hashed[33];
            
                for (int k = 0; k < num_matched_users; k++) {
                    if (strcmp(data->hashed_passwords[j].email, checked_users[k]) == 0) {
                        data->is_matched = 1;
                        return;
                    }
                }

            if ((data->is_matched || finish_sig)==0) {
                snprintf(temp, MAX_LEN, "%s", data->dictionary[i].password);
                for (int k = 0; k < 100; k++) {
                    if ((data->is_matched || finish_sig)==0) {
                    snprintf(temp_with_postfix, MAX_LEN, "%s%d", temp, k);
                    bytes2md5(temp_with_postfix, strlen(temp_with_postfix), temp_hashed);
                    checkPassword(temp_hashed, &data->hashed_passwords[j], temp_with_postfix);
                    }else{
                        return;
                    }
                }
            }
        }

void search_upper_postfix(ThreadData *data, int j, int i){
	char temp[MAX_LEN];
	char temp_with_postfix[MAX_LEN];
	char temp_hashed[33];
            for (int k = 0; k < num_matched_users; k++) {
                if (strcmp(data->hashed_passwords[j].email, checked_users[k]) == 0) {
                    data->is_matched = 1;
                    return;
                }
            }
            if ((data->is_matched || finish_sig)==0) {
                snprintf(temp, MAX_LEN, "%s", data->dictionary[i].password);
                toUpperCase(temp);
                for (int k = 0; k < 100; k++) {
                    if ((data->is_matched || finish_sig)==0) {
                    snprintf(temp_with_postfix, MAX_LEN, "%s%d", temp, k);
                    bytes2md5(temp_with_postfix, strlen(temp_with_postfix), temp_hashed);
                    checkPassword(temp_hashed, &data->hashed_passwords[j], temp_with_postfix);
                    }else{
                        return;
                    }
                }
            }   
		}

void search_first_postfix(ThreadData *data, int j, int i){
	char temp[MAX_LEN];
	char temp_with_postfix[MAX_LEN];
	char temp_hashed[33];
            for (int k = 0; k < num_matched_users; k++) {
                if (strcmp(data->hashed_passwords[j].email, checked_users[k]) == 0) {
                    data->is_matched = 1;
                    return;
                }
            }
            if ((data->is_matched || finish_sig)==0) {
                snprintf(temp, MAX_LEN, "%s", data->dictionary[i].password);
                toUpperFirst(temp);
                for (int k = 0; k < 100; k++) {
                    if ((data->is_matched || finish_sig)==0) {
                    snprintf(temp_with_postfix, MAX_LEN, "%s%d", temp, k);
                    bytes2md5(temp_with_postfix, strlen(temp_with_postfix), temp_hashed);
                    checkPassword(temp_hashed, &data->hashed_passwords[j], temp_with_postfix);
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
                    if (strcmp(data->hashed_passwords[j].email, checked_users[k]) == 0) {
                        data->is_matched = 1;
                        return;
                    }
                }

            if ((data->is_matched || finish_sig)==0) {
                snprintf(temp, MAX_LEN, "%s", data->dictionary[i].password);
                for (int k = 0; k < 100; k++) {
                    if ((data->is_matched || finish_sig)==0) {
                    snprintf(temp_with_postfix, MAX_LEN, "%d%s",k, temp);
                    bytes2md5(temp_with_postfix, strlen(temp_with_postfix), temp_hashed);
                    checkPassword(temp_hashed, &data->hashed_passwords[j], temp_with_postfix);
                    }else{
                        return;
                    }
                }
            }
        }

void search_upper_prefix(ThreadData *data, int j, int i){
	char temp[MAX_LEN];
	char temp_with_postfix[MAX_LEN];
	char temp_hashed[33];
            for (int k = 0; k < num_matched_users; k++) {
                if (strcmp(data->hashed_passwords[j].email, checked_users[k]) == 0) {
                    data->is_matched = 1;
                    return;
                }
            }
            if ((data->is_matched || finish_sig)==0) {
                snprintf(temp, MAX_LEN, "%s", data->dictionary[i].password);
                toUpperCase(temp);
                for (int k = 0; k < 100; k++) {
                    if ((data->is_matched || finish_sig)==0) {
                    snprintf(temp_with_postfix, MAX_LEN, "%d%s",k , temp);
                    bytes2md5(temp_with_postfix, strlen(temp_with_postfix), temp_hashed);
                    checkPassword(temp_hashed, &data->hashed_passwords[j], temp_with_postfix);
                    }else{
                        return;
                    }
                }
            }
		}

void search_first_prefix(ThreadData *data, int j, int i){
	char temp[MAX_LEN];
	char temp_with_postfix[MAX_LEN];
	char temp_hashed[33];
            for (int k = 0; k < num_matched_users; k++) {
                if (strcmp(data->hashed_passwords[j].email, checked_users[k]) == 0) {
                    data->is_matched = 1;
                    return;
                }
            }
            if ((data->is_matched || finish_sig)==0) {
                snprintf(temp, MAX_LEN, "%s", data->dictionary[i].password);
                toUpperFirst(temp);
                for (int k = 0; k < 100; k++) {
                    if ((data->is_matched || finish_sig)==0) {
                    snprintf(temp_with_postfix, MAX_LEN, "%d%s", k, temp);
                    bytes2md5(temp_with_postfix, strlen(temp_with_postfix), temp_hashed);
                    checkPassword(temp_hashed, &data->hashed_passwords[j], temp_with_postfix);
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
                    if (strcmp(data->hashed_passwords[j].email, checked_users[k]) == 0) {
                        data->is_matched = 1;
                        return;
                    }
                }

            if ((data->is_matched || finish_sig)==0) {
                snprintf(temp, MAX_LEN, "%s", data->dictionary[i].password);
                for (int k = 0; k < 100; k++) {
                    for (int l = 0; l < 100; l++) {
                        if ((data->is_matched || finish_sig)==0) {
                        snprintf(temp_with_postfix, MAX_LEN, "%d%s%d",k, temp, l);
                        bytes2md5(temp_with_postfix, strlen(temp_with_postfix), temp_hashed);
                        checkPassword(temp_hashed, &data->hashed_passwords[j], temp_with_postfix);
                        }else{
                            return;
                        }
                    }
                }
            }
        }

void search_upper_full(ThreadData *data, int j, int i){
	char temp[MAX_LEN];
	char temp_with_postfix[MAX_LEN];
	char temp_hashed[33];
            for (int k = 0; k < num_matched_users; k++) {
                if (strcmp(data->hashed_passwords[j].email, checked_users[k]) == 0) {
                    data->is_matched = 1;
                    return;
                }
            }
            if ((data->is_matched || finish_sig)==0) {
                snprintf(temp, MAX_LEN, "%s", data->dictionary[i].password);
                toUpperCase(temp);
                for (int k = 0; k < 100; k++) {
                    for (int l = 0; l < 100; l++) {
                        if ((data->is_matched || finish_sig)==0) {
                        snprintf(temp_with_postfix, MAX_LEN, "%d%s%d",k, temp, l);
                        bytes2md5(temp_with_postfix, strlen(temp_with_postfix), temp_hashed);
                        checkPassword(temp_hashed, &data->hashed_passwords[j], temp_with_postfix);
                        }else{
                            return;
                        }
                    }
                }
            }
		}

void search_first_full(ThreadData *data, int j, int i){
	char temp[MAX_LEN];
	char temp_with_postfix[MAX_LEN];
	char temp_hashed[33];
            for (int k = 0; k < num_matched_users; k++) {
                if (strcmp(data->hashed_passwords[j].email, checked_users[k]) == 0) {
                    data->is_matched = 1;
                    break;
                }
            }
            if ((data->is_matched || finish_sig)==0) {
                snprintf(temp, MAX_LEN, "%s", data->dictionary[i].password);
                toUpperFirst(temp);
                for (int k = 0; k < 100; k++) {
                    for (int l = 0; l < 100; l++) {
                        if ((data->is_matched || finish_sig)==0) {
                        snprintf(temp_with_postfix, MAX_LEN, "%d%s%d",k, temp, l);
                        bytes2md5(temp_with_postfix, strlen(temp_with_postfix), temp_hashed);
                        checkPassword(temp_hashed, &data->hashed_passwords[j], temp_with_postfix);
                        }else{
                            return;
                        }
                    }
                }
            }
		}

//Wątki

void *search_passwords(void *thread_data) {
    ThreadData *data = (ThreadData *)thread_data;
    int local_race;

        pthread_mutex_lock(&mutex);
        ++race;
        local_race=race;
        pthread_mutex_unlock(&mutex);

		while(data->searched_password<data->num_passwords){
            while (local_race<=data->num_dictionary){

                if((data->is_matched || finish_sig)==0){
                    search_original(data,data->searched_password,local_race);
                }else{
                    break;
                }
                if((data->is_matched || finish_sig)==0){
                    search_upper(data,data->searched_password,local_race);
                }else{
                    break;
                }
                if((data->is_matched || finish_sig)==0){
                    search_first(data,data->searched_password,local_race);
                }else{
                    break;
                }
                if((data->is_matched || finish_sig)==0){
                    search_original_prefix(data,data->searched_password,local_race);
                }else{
                    break;
                }
                if((data->is_matched || finish_sig)==0){
                    search_upper_prefix(data,data->searched_password,local_race);
                }else{
                    break;
                }
                if((data->is_matched || finish_sig)==0){
                    search_first_prefix(data,data->searched_password,local_race);
                }else{
                    break;
                }
                if((data->is_matched || finish_sig)==0){
                    search_original_postfix(data,data->searched_password,local_race);
                }else{
                    break;
                }
                if((data->is_matched || finish_sig)==0){
                    search_upper_postfix(data,data->searched_password,local_race);
                }else{
                    break;
                }
                if((data->is_matched || finish_sig)==0){
                    search_first_postfix(data,data->searched_password,local_race);
                }else{
                    break;
                }
                if((data->is_matched || finish_sig)==0){
                    search_original_full(data,data->searched_password,local_race);
                }else{
                    break;
                }
                if((data->is_matched || finish_sig)==0){
                    search_upper_full(data,data->searched_password,local_race);
                }else{
                    break;
                }
                if((data->is_matched || finish_sig)==0){
                    search_first_full(data,data->searched_password,local_race);
                }else{
                    break;
                }

                pthread_mutex_lock(&mutex);
                race++;
                local_race=race;
                pthread_mutex_unlock(&mutex);

            }
            pthread_barrier_wait(&barrier);

            data->searched_password++;

            pthread_mutex_lock(&mutex);
            data->is_matched = 0;
            race=0;
            local_race=race;
            ++race;
            pthread_mutex_unlock(&mutex);
    }
    pthread_barrier_wait(&barrier);
    finish=1;
    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&conditional);
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

void *wait_for_signal(void *thread_data) {
    ThreadData *data = (ThreadData *)thread_data;

    pthread_mutex_lock(&mutex);
    while (!finish) {
        pthread_cond_wait(&conditional, &mutex);
        if(shown_passwords!=num_matched_users){
        printf("Password for %s is %s\n", checked_users[shown_passwords], found_passwords[shown_passwords]);
        shown_passwords++;
        }
    }
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

void *main_thread_function(void *data){
    Dictionary *dictionary = malloc(count_lines_in_file("slownik.txt") * sizeof(Dictionary));
    UserInfo hashed_passwords[MAX_USERS];
    int num_dictionary, num_passwords;

    read_dictionary("slownik.txt", dictionary, &num_dictionary);
    read_password_file("3.txt", hashed_passwords, &num_passwords);

    pthread_t threads[MAX_THREADS];
    ThreadData thread_data[MAX_THREADS];

    pthread_t signal_thread;
    ThreadData signal_thread_data;

    for (int i = 0; i < MAX_THREADS; i++) {
        thread_data[i].dictionary = dictionary;
        thread_data[i].hashed_passwords = hashed_passwords;
        thread_data[i].num_dictionary = num_dictionary;
        thread_data[i].num_passwords = num_passwords;
        thread_data[i].searched_password=0;
        thread_data[i].is_matched=0;

        pthread_barrier_init(&barrier, NULL, MAX_THREADS);
		pthread_create(&threads[i], NULL, search_passwords, (void *)&thread_data[i]);
    }

    pthread_create(&signal_thread, NULL, wait_for_signal, (void *)&signal_thread_data);

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_join(signal_thread, NULL);

    free(dictionary);

    return NULL;

}

int main() {

    pthread_t main_thread;
	pthread_create(&main_thread, NULL, main_thread_function, NULL);
    pthread_join(main_thread, NULL);

    return 0;
}
