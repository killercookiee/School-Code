#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    int id;
    char name[101];
    char descr[201];
    int status;
} job;

void task1() {
    FILE *input = fopen("lorem.txt", "r");
    FILE *output = fopen("output.txt", "w");

    char ch;
    while ((ch = fgetc(input)) != EOF) {
        fputc(toupper(ch), output);
    }

    fclose(input);
    fclose(output);
    printf("Output is uppercase\n");
}

void task2() {
    FILE *input = fopen("lorem.txt", "r");

    int line_count = 0;
    char buffer[1024];

    while (fgets(buffer, sizeof(buffer), input) != NULL) {
        line_count++;
    }

    fclose(input);
    printf("Total number of lines: %d\n", line_count);
}

void task3() {
    FILE *input = fopen("lorem.txt", "r");
    FILE *output = fopen("output.txt", "w");

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), input) != NULL) {
        int length = strlen(buffer);
        int pos = 0;

        while (pos < length) {
            int chars_to_write = (length - pos > 20) ? 20 : length - pos;
            fwrite(buffer + pos, sizeof(char), chars_to_write, output);
            fputc('\n', output);
            pos += chars_to_write;
        }
    }

    fclose(input);
    fclose(output);
    printf("Line wrapping completed.\n");
}

void task4() {
    FILE *infile = fopen("jobs.txt", "r");
    if (infile == NULL) {
        printf("Error: Unable to open jobs.txt.\n");
        return;
    }

    int N;
    fscanf(infile, "%d", &N);
    job *a = (job *)malloc(sizeof(job) * N);

    for (int i = 0; i < N; i++) {
        fscanf(infile, "%d\n", &a[i].id);
        fgets(a[i].name, 101, infile);
        a[i].name[strlen(a[i].name) - 1] = '\0';
        fgets(a[i].descr, 201, infile);
        a[i].descr[strlen(a[i].descr) - 1] = '\0';
        fscanf(infile, "%d\n", &a[i].status);
    }

    fclose(infile);

    FILE *outfile = fopen("jobs2.txt", "w");
    for (int i = 0; i < N; i++) {
        if (strcmp(a[i].name, "Joe") == 0) {
            printf("Job ID: %d\nDescription: %s\nStatus: %d\n\n", a[i].id, a[i].descr, a[i].status);
            a[i].status = 1;
        }
        fprintf(outfile, "%d\n%s\n%s\n%d\n", a[i].id, a[i].name, a[i].descr, a[i].status);
    }

    fclose(outfile);
    free(a);
    printf("Task 4 completed. Data saved to jobs2.txt.\n");
}

void task5() {
    // Program 1
    FILE *infile = fopen("jobs.txt", "r");
    if (infile == NULL) {
        printf("Error: Unable to open jobs.txt.\n");
        return;
    }

    int N;
    fscanf(infile, "%d", &N);
    job *a = (job *)malloc(sizeof(job) * N);

    for (int i = 0; i < N; i++) {
        fscanf(infile, "%d\n", &a[i].id);
        fgets(a[i].name, 101, infile);
        a[i].name[strlen(a[i].name) - 1] = '\0';
        fgets(a[i].descr, 201, infile);
        a[i].descr[strlen(a[i].descr) - 1] = '\0';
        fscanf(infile, "%d\n", &a[i].status);
    }
    fclose(infile);

    FILE *binfile = fopen("jobs.bin", "wb");
    if (binfile == NULL) {
        printf("Error: Unable to create jobs.bin.\n");
        free(a);
        return;
    }

    fwrite(&N, sizeof(int), 1, binfile);
    fwrite(a, sizeof(job), N, binfile);
    fclose(binfile);
    free(a);
    printf("Data saved to jobs.bin in binary format.\n\n");


    // Program 2
    FILE *bininput = fopen("jobs.bin", "rb");
    if (bininput == NULL) {
        printf("Error: Unable to open jobs.bin.\n");
        return;
    }

    fread(&N, sizeof(int), 1, bininput);
    a = (job *)malloc(sizeof(job) * N);

    fread(a, sizeof(job), N, bininput);
    fclose(bininput);

    for (int i = 0; i < N; i++) {
        printf("ID: %d\nName: %s\nDescription: %s\nStatus: %d\n\n", 
                a[i].id, a[i].name, a[i].descr, a[i].status);
    }

    free(a);
    printf("Binary data read and displayed.\n");
}


int main() {
    int task_number;

    printf("Task number: ");
    scanf("%d", &task_number);

    if (task_number == 1) {
        task1();
    } else if (task_number == 2) {
        task2();
    } else if (task_number == 3) {
        task3();
    } else if (task_number == 4) {
        task4();
    } else if (task_number == 5) {
        task5();
    } else {
        printf("Invalid task number.\n");
    }
    return 0;
}
