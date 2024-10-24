#include <stdio.h>
#include <string.h>
#include <stdlib.h>



// Task 1
void task1() {
    char word[100];
    printf("Enter word: ");
    scanf("%s", word);
    
    for (int i = 0; word[i] != '\0'; i++) {
        printf("%c\n", word[i]);
    }    
}



// Task 2
void trim(char *source, char *destination) {
    int start = 0, end = strlen(source) - 1;
    
    while (source[start] == ' ') {
        start++;
    }
    
    while (source[end] == ' ') {
        end--;
    }
    
    int j = 0;
    for (int i = start; i <= end; i++) {
        destination[j++] = source[i];
    }
    destination[j] = '\0';
}

void task2() {
    char source[] = "   Hi, what's up?   ";
    char destination[100];
    
    trim(source, destination);
    printf("Trimmed string: '%s'\n", destination);
    
}



// Task 3
void task3() {
    int n;
    printf("Enter the number of numbers: ");
    scanf("%d", &n);

    double *values = (double*)malloc(n * sizeof(double));
    
    printf("Enter %d numbers:\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%lf", &values[i]);
    }
    
    printf("Numbers in reverse order:\n");
    for (int i = n - 1; i >= 0; i--) {
        printf("%lf\n", values[i]);
    }
    
    free(values);
    
}



int main() {
    int task_number;
    
    printf("Task number: ");
    scanf("%d", &task_number);

    if (task_number == 1) {
        task1();
    }
    else if (task_number == 2) {
        task2();
    }
    else if (task_number == 3) {
        task3();
    }
    else {
        printf("Invalid task number.\n");
    }
    return 0;
}