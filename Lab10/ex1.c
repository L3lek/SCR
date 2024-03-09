#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define AMMOUT 10
#define MAXROBOTS 10

typedef struct{
    int id, weight, ammout;
    char rockType[100];
}taczka;

void printfTaczka( taczka pomocnicza){
        printf("Taczka nr %d:\n", pomocnicza.id);
        printf("  Kamień: %s, waga: %d, ilość: %d\n",
               pomocnicza.rockType, pomocnicza.weight,
               pomocnicza.ammout);
    printf("\n");
}

taczka tablicaTaczek[1000];
int pozycjaNaTablicy = 0;

void processLine(char buffer[], taczka *tablicaTaczek, int *pozycjaNaTablicy) {
    int minute, type;
    taczka pomocnicza;

    //minute = buffer[0] - '0';
    //printf("Moment %d\n", minute);

    type = 0;
    for (int i = 2; ; i++) {
        if ((buffer[i] == '\0') || (buffer[i] == '\n')) {
            break;
        }

        if (buffer[i] != ' ') {
            if (type == 0) {
                sscanf(&buffer[i], "%d", &pomocnicza.id);
                while (buffer[i] != ' ' && buffer[i] != '\n' && buffer[i] != '\0') {
                    i++;
                }
                i--;
            } else if (type == 1) {
                sscanf(&buffer[i], "%s", pomocnicza.rockType);
                while (buffer[i] != ' ' && buffer[i] != '\n' && buffer[i] != '\0') {
                    i++;
                }
                i--;
            } else if (type == 2) {
                sscanf(&buffer[i], "%d", &pomocnicza.weight);
                while (buffer[i] != ' ' && buffer[i] != '\n' && buffer[i] != '\0') {
                    i++;
                }
                i--;
            } else if (type == 3) {
                sscanf(&buffer[i], "%d", &pomocnicza.ammout);
                while (buffer[i] != ' ' && buffer[i] != '\n' && buffer[i] != '\0') {
                    i++;
                }
                i--;
                tablicaTaczek[(*pozycjaNaTablicy)++] = pomocnicza;
                printfTaczka(pomocnicza);
            }
        } else {
            type = (type + 1) % 4;
        }
    }
}

int main(int argc, char *argv[]){

    for (int i = 0; i < 1000; i++){
        tablicaTaczek[i].id = -1;
    }

    FILE *fptr;
    fptr = fopen("Dane1.txt", "r");
    if (fptr == NULL) {
        printf("Nie udało się otworzyć domyślnego pliku: Dane1.txt\n");
        return -1;
    }
        
    char buffer[1000];

    while (fgets(buffer, sizeof(buffer), fptr) != NULL) {
        processLine(buffer, tablicaTaczek, &pozycjaNaTablicy);
    }
	
	return 0;
}