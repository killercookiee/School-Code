#include <stdio.h>

void task1() {
  printf("This program prints the character and its ASCII value.\n");
  printf("The while loop makes the user input a number infinitely; %%c represents the character, %%d represents the ASCII value.\n");
  printf("scanf(\"%%c\", &letter) == 1 : makes the input the letter and the function return the number of items read\n\n");


  char letter;
  while (scanf("%c", &letter) == 1) {
      printf("letter='%c', letter=%d\n", letter, letter);
  }
}

void task2() {
  printf("Expected first output: a=1\n b=2\n c=3\n");
  printf("Expected second output: no affect or new values or program crash\n");
  printf("while (true) loop goes infinitely\n");

}

void task3() {
    int array[10] = {25, 69, 54, 8, 77, 6, 29, 10, 3, 98};
    int i;
    int min_index = 0;

    printf("Array:\n");
    for (i = 0; i < 10; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");    


    printf("The array:\n");
    for (i = 0; i < 10; i++) {
        printf("[%d]=%d ", i, array[i]);
    }
    printf("\n");

    for (i = 1; i < 10; i++) {
        if (array[i] < array[min_index]) {
            min_index = i;
        }
    }

    printf("\nThe least: %d\n", array[min_index]);

    printf("Marked: ");
    for (i = 0; i < 10; i++) {
        if (i == min_index) {
            printf("%d[MIN] ", array[i]);
        } else {
            printf("%d ", array[i]);
        }
    }
    printf("\n");
}

void task4() {
    int array[10] = {25, 69, 54, 8, 77, 6, 29, 10, 3, 98};
    int i, j, first;

    for (j = 0; j < 10; j++) {
        for (i = 0; i < 10; i++) {
            printf("%d ", array[i]);
        }
        printf("\n");

        first = array[0];
        for (i = 0; i < 9; i++) {
            array[i] = array[i + 1];
        }
        array[9] = first;
    }
}

void task5() {
    int hour, minute, speed;
    int max_speed[24] = {0};

    printf("Enter 'hours minutes speed' (terminated by 0 0 0):\n");
    while (1) {
        scanf("%d %d %d", &hour, &minute, &speed);
        if (hour == 0 && minute == 0 && speed == 0) {
            break;
        }
        if (hour >= 0 && hour < 24) {
            if (speed > max_speed[hour]) {
                max_speed[hour] = speed;
            }
        }
    }

    printf("\nFastest speeding cars per hour:\n");
    for (int i = 0; i < 24; i++) {
        if (max_speed[i] > 0) {
            printf("%02d:00-%02d:59 -> %d km/h\n", i, i, max_speed[i]);
        }
    }
}

void task6() {
  int array[10] = {25, 69, 54, 8, 77, 6, 29, 10, 3, 98};
  printf("[ ");
  for (int i = 0; i < 9; i++) {
    printf("%d, ", array[i]);
  }
  printf("%d ]\n", array[9]);
  printf("\n");
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
    else if (task_number == 6) {
        task6();  
    }        
    else {
        printf("Invalid task number.\n");
    }


    return 0;
}