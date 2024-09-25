#include <stdio.h>
#include <math.h>

void task1() {
    int product = 1;
    int n = 10;
    while (n >= 2) {
        product = product * n;
        n--;
    }
    printf("Product: %i\n", product);
}

void task2() {
    int input;
    int sum = 1;
    printf("Input number: ");
    scanf("%d", &input);

    printf("1 ");
    for(int i = 2; i<input; i++) {
        if (input % i == 0) {
            printf("+ %d ", i);
            sum += i;
        }
    }
    printf("= %d\n", input);

    if (sum == input) {
        printf("%d is a perfect number\n", input);
    }
}

void task3() {
    double euler = 1;
    double p = 1;
    for (int i = 1; i <= 20; i++) {
        p = p*i;
        euler = euler + (1/p);
    }
    printf("%f\n", euler);
}

void task4() {
    double result = 1;
    int n;
    printf("Value of n: ");
    scanf("%d", &n);
    for (int i = 1; i <= n; i++) {
        result = result * pow((2*i), 2)/((2*i-1)*(2*i+1));
    }
    printf("n = %d: pi/2 = %f\n", n, result);
}

void task5() {
    double result = 0;
    int n;
    int d;
    printf("Value of n: ");
    scanf("%d", &n);
    for (int i = 1; i <= n; i++) {
        d = 2*i - 1;
        if (i % 2 != 0) {
            result = result + 1.0/d;
        }
        else {
            result = result - 1.0/d;
        }
    }
    printf("n = %d: pi/4 = %f\n", n, result);
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