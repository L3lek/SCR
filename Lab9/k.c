#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <openssl/evp.h>
#include <ctype.h>

#define MAX_WORD_LENGTH 100
#define MAX_HASH_LENGTH 33

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

void read_dictionary(const char *filename, char dictionary[][MAX_WORD_LENGTH], int *wordCount) {
    FILE *file = fopen(filename, "r");

    *wordCount = 0;
    while (fscanf(file, "%s", dictionary[*wordCount]) == 1) {
        (*wordCount)++;
    }

    fclose(file);
}

void read_hashes(const char *filename, char hashes[][MAX_HASH_LENGTH], int *hashCount) {
    FILE *file = fopen(filename, "r");

    *hashCount = 0;
    while (fscanf(file, "%s", hashes[*hashCount]) == 1) {
        (*hashCount)++;
    }

    fclose(file);
}

int main() {
    char dictionary[10000][MAX_WORD_LENGTH];
    char hashes[100][MAX_HASH_LENGTH];
    int dictionarySize, hashCount;

    read_dictionary("slownik.txt", dictionary, &dictionarySize);
    read_hashes("1.txt", hashes, &hashCount);

    for (int i = 0; i < dictionarySize; i++) {
        for (int j = 0; j < hashCount; j++) {
            char calculatedHash[MAX_HASH_LENGTH];
            bytes2md5(dictionary[i], strlen(dictionary[i]), calculatedHash);

            if (strcmp(calculatedHash, hashes[j]) == 0) {
                printf("Password for %s is %s\n", dictionary[i], calculatedHash);
            }
        }
    }

    return 0;
}