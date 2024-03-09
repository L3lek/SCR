#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ILOSC_TACZEK 10
#define MAX_ROBOTOW 10

typedef struct {
    int id, waga, ilosc, czasRozladunku;
    char typKamienia[100];
} Tacza;

Tacza tablicaTaczek[1000];
int pozycjaNaTablicy = 0;
int minuta;

Tacza roboty[MAX_ROBOTOW];
int ostatniaTacza = 0;

int kwantCzasu = 0;
int licznikiKwantuCzasu[MAX_ROBOTOW];

void wyswietlTaczke(Tacza pomocnicza) {
    printf("    tacza przyjechała <%d %s %d %d [%d]>\n", pomocnicza.id, pomocnicza.typKamienia, pomocnicza.waga, pomocnicza.ilosc, pomocnicza.waga * pomocnicza.ilosc);
}

void pop(int numer) {
    pozycjaNaTablicy = pozycjaNaTablicy - 1;
    for (int i = numer; i < pozycjaNaTablicy; i++) {
        tablicaTaczek[i] = tablicaTaczek[i + 1];
    }
}

void RR(int iloscRobotow) {
    Tacza pomocnicza;

    for (int i = 0; i < iloscRobotow; i++) {
        roboty[i] = tablicaTaczek[i];
        if (i < pozycjaNaTablicy && roboty[i].czasRozladunku != 0) {
            printf("[%-12s %2d]", roboty[i].typKamienia, roboty[i].czasRozladunku);
            roboty[i].czasRozladunku--;
        } else {
            printf("[%-14s ]", " ");
        }
        licznikiKwantuCzasu[i]++;
        licznikiKwantuCzasu[i] = licznikiKwantuCzasu[i] % kwantCzasu;
        tablicaTaczek[i] = roboty[i];
    }

    for(int i=0;i<iloscRobotow;i++){
    if ((licznikiKwantuCzasu[0] + 1) == kwantCzasu) {
        pomocnicza = tablicaTaczek[0];
        pop(0);
        tablicaTaczek[pozycjaNaTablicy] = pomocnicza;
        pozycjaNaTablicy++;
    }
    }

    for (int i = 0; i < pozycjaNaTablicy; i++) {
        if (tablicaTaczek[i].czasRozladunku == 0) {
            pop(i);
        }
    }
    printf("\n\n");
}

void FCFS(int numRobotow) {
    printf("                 ");

    for (int i = 0; i < numRobotow; i++) {
        if (ostatniaTacza < pozycjaNaTablicy || roboty[i].czasRozladunku > 0) {
            if (roboty[i].czasRozladunku == 0 && ostatniaTacza < pozycjaNaTablicy) {
                roboty[i] = tablicaTaczek[ostatniaTacza];
                ostatniaTacza++;
            }

            printf("[%-12s %2d]", roboty[i].typKamienia, roboty[i].czasRozladunku);
            roboty[i].czasRozladunku--;

        } else {
            printf("[%-14s ]", " ");
        }
    }

    printf("\n\n");
}

void czytajLinie(char bufor[], Tacza *tablicaTaczek, int *pozycjaNaTablicy) {
    int typ;
    Tacza pomocnicza;

    minuta = bufor[0] - '0';
    printf("Moment %d\n", minuta);

    typ = 0;
    for (int i = 2;; i++) {
        if ((bufor[i] == '\0') || (bufor[i] == '\n')) {
            break;
        }

        if (bufor[i] != ' ') {
            if (typ == 0) {
                sscanf(&bufor[i], "%d", &pomocnicza.id);
                while (bufor[i] != ' ' && bufor[i] != '\n' && bufor[i] != '\0') {
                    i++;
                }
                i--;
            } else if (typ == 1) {
                sscanf(&bufor[i], "%s", pomocnicza.typKamienia);
                while (bufor[i] != ' ' && bufor[i] != '\n' && bufor[i] != '\0') {
                    i++;
                }
                i--;
            } else if (typ == 2) {
                sscanf(&bufor[i], "%d", &pomocnicza.waga);
                while (bufor[i] != ' ' && bufor[i] != '\n' && bufor[i] != '\0') {
                    i++;
                }
                i--;
            } else if (typ == 3) {
                sscanf(&bufor[i], "%d", &pomocnicza.ilosc);
                while (bufor[i] != ' ' && bufor[i] != '\n' && bufor[i] != '\0') {
                    i++;
                }
                i--;
                pomocnicza.czasRozladunku = pomocnicza.waga * pomocnicza.ilosc;
                tablicaTaczek[(*pozycjaNaTablicy)++] = pomocnicza;
                wyswietlTaczke(pomocnicza);
            }
        } else {
            typ = (typ + 1) % 4;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Użycie: %s <liczba_robotow> <kwant_czasu> <plik_wejsciowy> <algorytm>\n", argv[0]);
        return -1;
    }

    int numRobotow = atoi(argv[1]);
    kwantCzasu = atoi(argv[2]);
    char *nazwaPliku = argv[3];
    int algorytm = atoi(argv[4]);

    for (int i = 0; i < 1000; i++) {
        tablicaTaczek[i].id = -1;
        tablicaTaczek[i].czasRozladunku = 0;
    }

    for (int i = 0; i < numRobotow; i++) {
        roboty[i].id = 0;
        roboty[i].czasRozladunku = 0;
    }

    if (algorytm == 1) {
        for (int i = 0; i < numRobotow; i++) {
            licznikiKwantuCzasu[i] = 0;
        }
    }
    
    FILE *plik;
    if (strcmp(nazwaPliku, "-") == 0) {
        plik = stdin;
    } else {
        plik = fopen(nazwaPliku, "r");
        if (plik == NULL) {
            printf("Nie udało się otworzyć pliku: %s\n", nazwaPliku);
            return -1;
        }
    }

    char bufor[1000];

    while (fgets(bufor, sizeof(bufor), plik) != NULL) {
        czytajLinie(bufor, tablicaTaczek, &pozycjaNaTablicy);
        if (algorytm == 2) {
            FCFS(numRobotow);
        } else if (algorytm == 1) {
            RR(numRobotow);
        }
    }

    int czyPusta = 0;

    while (czyPusta == 0) {
        minuta++;
        int wszystkieRobotyPuste = 1;
        if (algorytm == 1) {
            for (int i = 0; i < pozycjaNaTablicy; i++) {
                if (tablicaTaczek[i].czasRozladunku != 0) {
                    wszystkieRobotyPuste = 0;
                    break;
                }
            }
        } else {
            for (int i = 0; i < numRobotow; i++) {
                if (roboty[i].czasRozladunku != 0) {
                    wszystkieRobotyPuste = 0;
                    break;
                }
            }
        }

        czyPusta = wszystkieRobotyPuste;

        if ((czyPusta == 0)) {
            printf("Moment %d\n", minuta);
            if (algorytm == 2) {
                FCFS(numRobotow);
            } else if (algorytm == 1) {
                RR(numRobotow);
            }
        }
    }

    if (plik != stdin) {
        fclose(plik);
    }

    return 0;
}
