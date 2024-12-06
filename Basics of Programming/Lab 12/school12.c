#include <stdio.h>

double arithmetic(int n, double start, double diff) {
    if (n == 0)
        return start; /*base case*/
    else
        return diff + arithmetic(n - 1, start, diff);
}

double geometric(int n, double start, double quot) {
    if (n == 0)
        return start; /*base case*/
    else
        return quot * geometric(n - 1, start, quot);
}
 
void task1(void) {
    for (int n = 0; n < 10; ++n)
        printf("%g  ", arithmetic(n, 1, 5));
    printf("\n");
    
    for (int n = 0; n < 10; ++n)
        printf("%g  ", geometric(n, 2, 2));
    printf("\n");
}


/* Fibonacci series */
int fib(int n) {
    /* base cases (terminating terms) */
    if (n == 0) return 0; /* element 0. is 0 */
    if (n == 1) return 1; /* element 1. is 1 */

    /* general term */
    return fib(n - 1) + fib(n - 2); /* the nth element is the sum of the previous two */
}

void task2(void) {
    for (int i = 0; i < 40; i++)
        printf("%d\n", fib(i));
}


void print_string_1(char *text) {
    if (text[0] == '\0')
        return;
    putchar(text[0]);
    printf("%s", text + 1);     // !
}

void print_string_2(char *text) {
    if (text[0] == '\0')
        return;
    putchar(text[0]);
    print_string_2(text + 1); // !
}

void task3(void) {
    print_string_1("word");
    print_string_2("word");
}


void array_print_forward(int a[], int n) {
  if(n > 0) {                      /* If it is not empty */
    printf("%d ", a[0]);           /* Print first element */
    array_print_forward(a+1, n-1); /* Print rest of the array by calling the same function */
                                   /* such that we pass a pointer to the next element */
  }
}

void array_print_backward(int a[], int n) {
  if(n > 0) {                       /* If it is not empty */
    array_print_backward(a+1, n-1); /* Print the array except the first element, backwards */
    printf("%d ", a[0]);            /* Print first element */
  }
}

void task4(void) {
    int a5[] = {1,2,3,4,5}, a10[] = {1,2,3,4,5,6,7,8,9,10};

    array_print_forward(a5, 5); printf("\n");
    array_print_backward(a5, 5); printf("\n");
    array_print_forward(a10, 10); printf("\n");
    array_print_backward(a10, 10); printf("\n");
}


void convert(int n, int r) {
    /* If n>r, recursive call */
    if (n / r > 0)
        convert(n / r, r);
    /* print the remainder (last digit) */
    printf("%d", n % r);
}

void convert2(int n, int r) {
    char *s = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    /* If there are more digits to print, recursive call */
    if(n / r > 0)
        convert2(n / r, r);
    /* print the remainder (last digit) */
    putchar(s[n % r]);
}

void task5(void) {
    convert(27, 5);  /* 102 */
    printf("\n");
    convert(13, 2);  /* 1101 */
    printf("\n");
    convert2(64519, 16);  /* FC07 */
}


void printnum(int n) {
    /* If we have more than 3 digits */
    if (n / 1000 > 0) {
        printnum(n / 1000);        /* Cut last 3 digits and call the function recursively */
        printf(" %03d", n % 1000); /* Print a space and the last 3 digits */
    }
    else
        printf("%d", n);           /* Otherwise we print the number without modifications */
}

void task6(void) {
    printnum(16077216);

    return 0;
}

int main(void) {
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
    } else if (task_number == 6) {
        task6();
    } else {
        printf("Invalid task number.\n");
    }
    return 0;
}
