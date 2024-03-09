#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int num_wheelbarrows;

// Structure representing a stone
typedef struct {
    char type[20];
    int weight;
    int quantity;
} Stone;

// Structure representing a wheelbarrow
typedef struct {
    int number;
    Stone stone;
    int arrival_minute;
    int transport_time; // Time needed to transport the wheelbarrow
} Wheelbarrow;

void readData(char* filename, Wheelbarrow wheelbarrows[]) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }

    num_wheelbarrows = 0;
    char line[100];
    int num_fields;
    int minute;
    int number, quantity,weight;
    char type[20];
    while (fgets(line, sizeof(line), file) != NULL) {
        
        num_fields=sscanf(line,"%d %d %s %d %d", &minute,&number, type, &quantity, &weight);     

        if (num_fields == 5) {
            wheelbarrows[num_wheelbarrows].arrival_minute=minute;
            wheelbarrows[num_wheelbarrows].number=number;
            strcpy(wheelbarrows[num_wheelbarrows].stone.type,type);
            wheelbarrows[num_wheelbarrows].stone.quantity=quantity;
            wheelbarrows[num_wheelbarrows].stone.weight=weight;

            num_wheelbarrows++; 
            num_fields=sscanf(line, "%*d %*d %*s %*d %*d %d %s %d %d", &number, type, &quantity, &weight);
            if(num_fields==4){
                wheelbarrows[num_wheelbarrows].arrival_minute=minute;
                wheelbarrows[num_wheelbarrows].number=number;
                strcpy(wheelbarrows[num_wheelbarrows].stone.type,type);
                wheelbarrows[num_wheelbarrows].stone.quantity=quantity;
                wheelbarrows[num_wheelbarrows].stone.weight=weight;                      
                num_wheelbarrows++;      
            }
            
        }
    }
    fclose(file);
}

// Function to display data from an array of wheelbarrows
void displayData(Wheelbarrow wheelbarrows[]) {
    for (int i = 0; i < num_wheelbarrows; i++) {
        printf("Taczka nr %d:\n", wheelbarrows[i].number);
        printf("  Kamień: %s, waga: %d, ilość: %d\n",
               wheelbarrows[i].stone.type, wheelbarrows[i].stone.weight,
               wheelbarrows[i].stone.quantity);
    }
    printf("\n");
}

int main() {
    Wheelbarrow wheelbarrows[100]; // Assuming a maximum of 100 wheelbarrows

    // Read data from the "Dane1.txt" file into the array of wheelbarrows
    readData("Dane1.txt", wheelbarrows);

    // Display the read data
    displayData(wheelbarrows);

    return 0;
}