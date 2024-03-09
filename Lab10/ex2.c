#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define AMMOUT 10
#define MAXROBOTS 10

typedef struct{
    int id, weight, ammout, timeToUnload;
    char rockType[100];
}taczka;

taczka tablicaTaczek[1000];
int pozycjaNaTablicy = 0;

taczka roboty[MAXROBOTS];

int lastTaczka = 0;
int minute;

void printfTaczka( taczka pomocnicza){
        printf("	taczka przyjechała <%d %s %d %d [%d]>\n", pomocnicza.id, pomocnicza.rockType, pomocnicza.weight, pomocnicza.ammout, pomocnicza.weight * pomocnicza.ammout);
}

void FCFS(int numRobots) {
    printf("        ");

    for (int i = 0; i < numRobots; i++) {
        if (lastTaczka < pozycjaNaTablicy || roboty[i].timeToUnload > 0) {
            if (roboty[i].timeToUnload == 0 && lastTaczka < pozycjaNaTablicy) {
                roboty[i] = tablicaTaczek[lastTaczka];
                lastTaczka++;
            }

            printf("[%s        %d]", roboty[i].rockType, roboty[i].timeToUnload);
            roboty[i].timeToUnload--;

        } else {
            printf("[        ]");
        }
    }

    printf("\n\n");
}

void processLine(char buffer[], taczka *tablicaTaczek, int *pozycjaNaTablicy) {
    int type;
    taczka pomocnicza;

    minute = buffer[0] - '0';
    printf("Moment %d\n", minute);

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
				pomocnicza.timeToUnload=pomocnicza.weight*pomocnicza.ammout;
                tablicaTaczek[(*pozycjaNaTablicy)++] = pomocnicza;
                printfTaczka(pomocnicza);
            }
        } else {
            type = (type + 1) % 4;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Użycie: %s <liczba_robotow> <kwant_czasu> <plik_wejsciowy> <algorytm>\n", argv[0]);
        return -1;
    }

    int numRobots = atoi(argv[1]);
    int timeQuantum = atoi(argv[2]);
    char *fileName = argv[3];
    int schedulingAlgorithm = atoi(argv[4]);

    for (int i = 0; i < 1000; i++) {
        tablicaTaczek[i].id = -1;
        tablicaTaczek[i].timeToUnload = 0;
    }

    for (int i = 0; i < numRobots; i++) {
        roboty[i].id = 0;
        roboty[i].timeToUnload = 0;
    }

    FILE *fptr;
    if (strcmp(fileName, "-") == 0) {
        fptr = stdin; // Czytaj dane ze standardowego wejścia
    } else {
        fptr = fopen(fileName, "r");
        if (fptr == NULL) {
            printf("Nie udało się otworzyć pliku: %s\n", fileName);
            return -1;
        }
    }

    char buffer[1000];
    while (fgets(buffer, sizeof(buffer), fptr) != NULL) {
        processLine(buffer, tablicaTaczek, &pozycjaNaTablicy);
        if (schedulingAlgorithm == 2) {
            FCFS(numRobots);
        }
    }

    int minute = 0;
    int isEmpty = 0;

    while (isEmpty == 0) {
        minute++;
        int allRobotsEmpty = 1;

        for (int i = 0; i < numRobots; i++) {
            if (roboty[i].timeToUnload != 0) {
                allRobotsEmpty = 0;
                break;
            }
        }

        isEmpty = allRobotsEmpty;

        if (isEmpty == 0) {
            printf("Moment %d\n", minute);
            if (schedulingAlgorithm == 2) { 
                FCFS(numRobots);
            }

        }
    }

    if (fptr != stdin) {
        fclose(fptr);
    }

    return 0;
}