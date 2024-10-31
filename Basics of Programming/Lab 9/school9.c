#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// Task 1
char *copystring(const char *str) {
    printf("Copying string: %s\n", str);

    char *copy = (char*)malloc(strlen(str) + 1);

    for (int i = 0; str[i] != '\0'; i++) {
        copy[i] = str[i];
    }
    copy[strlen(str)] = '\0';

    return copy;
}

void task1() {
    char *str = copystring("apple");
    printf("Copied string: %s\n", str);
    free(str);
}



// Task 2
char *stringcat(char *str1, const char *str2) {
    printf("Concatenating strings: %s and %s\n", str1, str2);

    int len1 = strlen(str1);
    int len2 = strlen(str2);
    
    char *result = (char*)malloc(len1 + len2 + 1);

    for (int i = 0; i < len1; i++) {
        result[i] = str1[i];
    }

    for (int i = 0; i < len2; i++) {
        result[len1 + i] = str2[i];
    }

    result[len1 + len2] = '\0';

    return result;
}

void task2() {
    char *x = copystring("apple");

    x = stringcat(x, "tree");
    printf("%s\n", x);

    free(x);
}

// Task 3
char *substring(const char *str, int start, int end) {
    int len = strlen(str);
    
    if (start < 0 || end > len || start > end) {
        printf("Error: Invalid start or end index.\n");
        return NULL;
    }
    
    int sub_len = end - start;
    char *result = (char*)malloc(sub_len + 1);

    memcpy(result, &str[start], sub_len);
    result[sub_len] = '\0';

    return result;
}


void task3() {
    char str[100];
    printf("Enter a string: ");
    scanf("%99s", str);

    int start, end;
    printf("Enter start and end index: ");
    scanf("%d %d", &start, &end);

    char *test = substring(str, start, end);
    printf("substring(%d, %d): \"%s\"\n", start, end, test);
    free(test);
}

// Task 4
char *remove_substring(const char *str, int start, int end) {
    int len = strlen(str);
    
    if (start < 0 || end > len || start > end) {
        printf("Error: Invalid start or end index.\n");
        return NULL;
    }

    int new_len = len - (end - start);
    char *result = (char*)malloc(new_len + 1);

    memcpy(result, str, start);
    memcpy(result + start, str + end, len - end);

    result[new_len] = '\0';

    return result;
}

void task4() {
    char str[100];
    printf("Enter a string: ");
    scanf("%99s", str);

    int start, end;
    printf("Enter start and end index: ");
    scanf("%d %d", &start, &end);

    char *test = remove_substring(str, start, end);
    printf("remove_substring(%d, %d): \"%s\"\n", start, end, test);

    free(test);
}

// Task 5
char *insert_string(const char *str, int pos, const char *insert) {
    int len_str = strlen(str);
    int len_insert = strlen(insert);

    if (pos < 0 || pos > len_str) {
        printf("Error: Invalid position for insertion.\n");
        return NULL;
    }

    char *result = (char*)malloc(len_str + len_insert + 1);
    if (result == NULL) {
        printf("Error: Memory allocation failed.\n");
        return NULL;
    }

    memcpy(result, str, pos);
    memcpy(result + pos, insert, len_insert);
    memcpy(result + pos + len_insert, str + pos, len_str - pos);

    result[len_str + len_insert] = '\0';

    return result;
}

void task5() {
    char str[100];
    printf("Enter a string: ");
    scanf("%99s", str);

    int pos;
    printf("Enter position: ");
    scanf("%d", &pos);

    char insert_str[100];
    printf("Enter string to insert: ");
    scanf("%99s", insert_str);

    char *test = insert_string(str, pos, insert_str);
    printf("insert_string(%d, \"%s\"): \"%s\"\n", pos, insert_str, test);

    free(test);
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
    else if (task_number == 4) {
        task4();
    }
    else if (task_number == 5) {
        task5();
    }
    else {
        printf("Invalid task number.\n");
    }
    return 0;
}