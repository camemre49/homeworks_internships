#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Global random number for phase 1
int random_number;

// Function to initialize the bomb
void initialize() {
     // Set the random number for phase 1
    srand(time(NULL));
    random_number = rand() % 1000;  // Let's limit the random number to 3 digits for simplicity
}

// Function to "explode" the bomb
void explode_bomb() {
    printf("BOOM!\n");
    exit(1);
}

// Phase 1: Check for the correct random integer
void phase_1(char *input) {
    int user_input;
    if (sscanf(input, "%d", &user_input) != 1 || user_input != random_number) {
        explode_bomb();
    }
}

// Phase 2: Check the conditions for two characters
void phase_2(char *input) {
    if (strlen(input) != 2 || input[0] < 'A' || input[0] > 'z' || input[1] < 'A' || input[1] > 'z' 
        || (abs(input[0] - input[1]) % 13) != 5) {
        explode_bomb();
    }
}

int main() {
    char input[256];

    initialize();
    printf("Welcome to the simplified bomb!\n");


    // Phase 1
    printf("Enter the secret input for phase 1: ");
    fgets(input, sizeof(input), stdin);
    // Remove trailing newline
    input[strcspn(input, "\n")] = 0;
    phase_1(input);

    printf("Phase 1 defused!\n");

    // Phase 2
    printf("Enter the next input for phase 2: ");
    fgets(input, sizeof(input), stdin);
    // Remove trailing newline
    input[strcspn(input, "\n")] = 0;
    phase_2(input);

    printf("Phase 2 defused! Congratulations!\n");

    return 0;
}

