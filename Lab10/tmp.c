#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define AMMOUT 10
#define MAXROBOTS 10

int iloscRobotow = 0;
int kwantCzasu = 0;
int typAlgorytmu = 2; //FCFS

// FCFS Var
int lastTaczka = 0;
// RR Var
int kwantCzasuCounters[MAXROBOTS];

struct taczka
{
    int id, weight, ammout, timeToUnload;
    char rockType[100];
};

void printfTaczka(struct taczka pomocnicza){
    printf("    wheelbarrow arrived <%d %s %d %d [%d]>\n", pomocnicza.id, pomocnicza.rockType, pomocnicza.weight, pomocnicza.ammout, pomocnicza.weight * pomocnicza.ammout);
}

struct taczka tablicaTaczek[1000];
int pozycjaNaTablicy = 0;

void pop(int number){
    pozycjaNaTablicy = pozycjaNaTablicy -1;
    for (int i = number; i < pozycjaNaTablicy; i++)
    {
        tablicaTaczek[i] = tablicaTaczek[i+1];
    }
    
}


void RR(){
    struct taczka pomocnicza;
    printf("        ");
    for (int i = 0; i < iloscRobotow; i++){
        if ((i < pozycjaNaTablicy) && (tablicaTaczek[i].timeToUnload != 0)){
            printf("[%s        %d]",tablicaTaczek[i].rockType, tablicaTaczek[i].timeToUnload);
            tablicaTaczek[i].timeToUnload = tablicaTaczek[i].timeToUnload -1;

            if (tablicaTaczek[i].timeToUnload == 0){
                kwantCzasuCounters[i] = (kwantCzasu -1);                   
            }
        }        
        else{
            printf("[        ]");
        }
        
    }
    for (int j =0; j< iloscRobotow; j++){
        if ((kwantCzasu-1) == kwantCzasuCounters[j]){
            for (int i = 0; i < iloscRobotow; i++)
            {
                if (i < pozycjaNaTablicy){
                    if (tablicaTaczek[0].timeToUnload != 0){
                        pomocnicza = tablicaTaczek[0];
                        pop(0);
                        tablicaTaczek[pozycjaNaTablicy] = pomocnicza;
                        pozycjaNaTablicy++; 
                    }
                    else{
                        if (tablicaTaczek[iloscRobotow+1].id > 0){ // Jeśli nie moglismy ostatnio przerzucić to przerzucmy teraz
                            printf("AA");
                            tablicaTaczek[0] = tablicaTaczek[iloscRobotow+1];
                            pop(iloscRobotow);
                        }
                        else{
                            // printf("%s died, %d",tablicaTaczek[0].rockType,tablicaTaczek[pozycjaNaTablicy].id);
                            pop(0); 
                        }
                    }
                }
            }

        }
    }

    for (int k = 0; k < iloscRobotow; k++)
    {
        kwantCzasuCounters[k] = (kwantCzasuCounters[k] +1)%kwantCzasu;
    }
       
    printf("\n\n");
}

void FCFS(){
    printf("        ");
    for (int i = 0; i < iloscRobotow; i++){
        if ((lastTaczka + i) < pozycjaNaTablicy){
            while ((tablicaTaczek[lastTaczka + i].timeToUnload == 0) && ((lastTaczka + i) < pozycjaNaTablicy)) {
                lastTaczka++;
            }


            printf("[%s        %d]",tablicaTaczek[lastTaczka + i].rockType, tablicaTaczek[lastTaczka + i].timeToUnload);
            tablicaTaczek[lastTaczka + i].timeToUnload = tablicaTaczek[lastTaczka + i].timeToUnload -1;

        }
        else{
            printf("[        ]");
        }
        
    }
    printf("\n\n");
}

int main(int argc, char *argv[]){
    if (argc < 3){
        printf("Niewystarczajaca ilosc argumentow wywoalania\n");
        return -1;
    }

    // Przygotowanie tablicy
    for (int i = 0; i < 1000; i++){
        tablicaTaczek[i].timeToUnload = 0;
        tablicaTaczek[i].id = -1; // Ustawienie ID specjalnego
    }

    iloscRobotow = atoi(argv[1]);
    printf("%d robots in the mine\n", iloscRobotow);
    kwantCzasu = atoi(argv[2]);
    FILE *fptr;
    fptr = fopen("Dane1.txt", "r");
    if (fptr == NULL) {
        printf("Nie udało się otworzyć domyślnego pliku: Dane1.txt\n");
        return -1;
    }

    typAlgorytmu = atoi(argv[3]);
    if (typAlgorytmu == 1){
        printf("RR with quantum %d\n", kwantCzasu);        
        for (int i = 0; i < MAXROBOTS; i++)
        {
            if (i >iloscRobotow){
                kwantCzasuCounters[i] = -1;
            }
            else{
                kwantCzasuCounters[i] = 0;
            }
        }
        

    }

    char buffer[1000];
    struct taczka pomocnicza;

    int minute = 0;
    int type = 0;


    while (fgets(buffer,sizeof(buffer), fptr) != NULL)
    {
        minute = buffer[0] - '0';
        printf("Moment %d\n", minute);
        type = 0;
        for (int i = 2; ; i++){
            if ((buffer[i] == '\0') || (buffer[i] == '\n')){
                break;
            }


            if (buffer[i] != ' '){
                switch (type)
                {
                case 0: // Szukamy ID
                    pomocnicza.id = buffer[i] - '0';
                    if ((((buffer[i + 1]) != ' ') && (buffer[i+1] != '\n')) && (buffer[i+1] != '\0')){
                        pomocnicza.id = (pomocnicza.id*10) + (buffer[i+1] - '0');
                        i++;
                    }

                    break;

                case 1: // Szukamy slowa
                    int j = 0;
                    while (buffer[i] != ' ')
                    {
                        pomocnicza.rockType[j] = buffer[i];
                        i++;
                        j++;
                    }
                    pomocnicza.rockType[j] = '\0';
                    i--;
                    break;

                case 2: // Szukamy wagi
                    pomocnicza.weight = buffer[i] - '0';
                    if ((((buffer[i + 1]) != ' ') && (buffer[i+1] != '\n')) && (buffer[i+1] != '\0')){
                        pomocnicza.weight = (pomocnicza.weight*10) + (buffer[i+1] - '0');
                        i++;
                    }


                    break;
                
                case 3: // Szukamy ilości i KOŃCZYMY PRZESZUKIWANIE TEGO KAMULCA
                    pomocnicza.ammout = buffer[i] - '0';
                    if ((((buffer[i + 1]) != ' ') && (buffer[i+1] != '\n')) && (buffer[i+1] != '\0')){
                        pomocnicza.ammout = (pomocnicza.ammout*10) + (buffer[i+1] - '0');
                        i++;
                    }
                    /// WYWAL TĄ TACZKEEEEEEEEEEE

                    pomocnicza.timeToUnload = pomocnicza.ammout * pomocnicza.weight;
                    tablicaTaczek[pozycjaNaTablicy] = pomocnicza;
                    pozycjaNaTablicy++;
                    printfTaczka(pomocnicza);
                    break;
                default:
                    break;
                }
            }
            else{
                type = (type +1)%4;
            }
        }
        switch (typAlgorytmu)
        {
        case 2:
            FCFS();
            break;
        case 1:
            RR();
            break;
        default:
            break;
        }
    }

    int isEmpty = 0;
    if (typAlgorytmu < 1 || typAlgorytmu > 2){
        return -1;
    }
    while ((isEmpty == 0) && (minute < 100))
    {
        minute++;
        isEmpty = 1;

        if (typAlgorytmu == 1){
            for (int i = 0; i < pozycjaNaTablicy; i++)
            {
                if (tablicaTaczek[i].timeToUnload != 0){
                    isEmpty = 0;
                }
            }
        }
        else{
            if (tablicaTaczek[pozycjaNaTablicy-1].timeToUnload != 0){
                isEmpty = 0;
            }
        }

        if (isEmpty == 0){
            printf("Moment %d\n", minute);
            switch (typAlgorytmu)
            {
            case 2:
                FCFS();
                break;
            case 1:
                RR();
                break;
            default:
                break;
            }
        }
        
    }
    
    return 0;
}