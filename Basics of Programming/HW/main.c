#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For usleep function

// Function to clear the screen using ANSI escape codes
void clear_screen() {
    printf("\033[H\033[J");
}

// Function to print a simple animation frame
void print_frame(int position) {
    clear_screen(); // Clear the screen before drawing the new frame
    
    // Print spaces to move the object to the current position
    for (int i = 0; i < position; i++) {
        printf(" ");
    }
    printf("*\n"); // Print the animated object (e.g., a star)

    fflush(stdout); // Ensure immediate output
}

int main() {
    int max_position = 40; // Define how far the animation should go
    
    // Simple loop to move the object across the terminal
    for (int i = 0; i < max_position; i++) {
        print_frame(i); // Print the current frame
        usleep(100000); // Pause for 100 milliseconds for animation speed
    }

    return 0;
}
