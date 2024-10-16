#include <stdio.h>
#include <math.h>



// Task 1
void func1(int i2) {
    i2 *= 2;
    printf("func1()... i2 = %d\n", i2);     // i2 here is the copy of the value of i1 to a different address
}

void func2(int *p3) {
    *p3 *= 2;
    printf("func2()... *p3 = %d\n", *p3);   // p3 points to i1 and *p3 *= 2 doubles value of i1 at its address
    p3 = NULL;                              
    printf("func2()... p3 = %p\n", p3);     // p3 now points to null
}

void task1() {
    int i1 = 2;
    int *p1 = NULL;
    int *p2 = NULL;
    printf("Where do p1, p2 point to?\n"); // Both point to null

    p1 = &i1;
    printf("&i1 = %p, p1 = %p\n", &i1, p1);
    printf("i1 = %d, *p1 = %d\n", i1, *p1); // p1 point to i1, p2 null

    i1 = 3;  printf("*p1 = %d\n", *p1);
    *p1 = 4; printf("i1 = %d\n", i1);

    p2 = p1;
    *p2 = 5;
    printf("i1 = %d, p2 = %p\n", i1, p2);   // p1 point to i1, p2 point to i1
    printf("-----\n");

    func1(i1);
    printf("main()... i1 = %d - but why?\n", i1);   // funct1 does not change the input adress's value
    printf("-----\n");

    func2(&i1);
    printf("main()... i1 = %d - why?\n", i1);       // funct2 changes the input address's value
    printf("-----\n");

    func2(p2);
    printf("main()... i1 = %d - why?\n", i1);       // funct2 changes the input address's value
    printf("main()... p2 = %p - why?\n", p2);       // p2 is unaffected by p3
}



// Task 2
void cube(double edge, double *parea, double *pvolume) {
    *parea = 6 * edge * edge;
    *pvolume = edge * edge * edge;
}

void task2() {
    double edge_length;
    double surface_area, volume;

    printf("Edge length: ");
    scanf("%lf", &edge_length);

    cube(edge_length, &surface_area, &volume);

    printf("Surface area = %.2f\n", surface_area);
    printf("Volume = %.2f\n", volume);
}



// Task 3
void calculate(double num1, double num2, double *psum, double *pproduct) {
    *psum = num1 + num2;
    *pproduct = num1 * num2;
}

void task3() {
    double number1, number2;
    double sum, product;

    printf("Number 1: ");
    scanf("%lf", &number1);    

    printf("Number 2: ");
    scanf("%lf", &number2);        

    calculate(number1, number2, &sum, &product);

    printf("Sum = %.2f\n", sum);
    printf("Product = %.2f\n", product);
}



// Task 4
int find_index(int *arr, int size, int value) {
    for (int *p = arr; p < arr + size; ++p) {
        if (*p == value) {
            return p - arr;
        }
    }
    return -1;
}

int *find_address(int *arr, int size, int value) {
    for (int *p = arr; p < arr + size; ++p) {
        if (*p == value) {
            return p;
        }
    }
    return NULL;
}
void task4() {
    int n;
    printf("Enter the size of the array: ");
    scanf("%d", &n);
    int array[n];

    printf("Enter %d elements for the array:\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &array[i]);
    }

    int value;
    printf("Enter a single integer value: ");
    scanf("%d", &value);

    printf("Array elements: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", array[i]);
    }

    int size = sizeof(array) / sizeof(array[0]);
    size = n;
    int *address;

    printf("Array elements with their indices:\n");
    for (int i = 0; i < size; i++) {
        printf("Index %d: Value = %d\n", i, array[i]);
    }
    
    int index = find_index(array, size, value);
    if (index != -1) {
        printf("Value %d found at index %d.\n", value, index);
    } else {
        printf("Value %d not found.\n", value);
    }

    address = find_address(array, size, value);
    if (address != NULL) {
        printf("Value %d found at memory address %p, which corresponds to index %ld.\n", 
               value, (void*)address, address - array);
    } else {
        printf("Value %d not found.\n", value);
    }
}



// Task 5
void separate_even_odd(int *arr, int size, int *even_arr, int *odd_arr, int *even_count, int *odd_count) {
    for (int i = 0; i < size; i++) {
        if (arr[i] % 2 == 0) {
            even_arr[*even_count] = arr[i];
            (*even_count)++;
        } else {
            odd_arr[*odd_count] = arr[i];
            (*odd_count)++;
        }
    }
}

void print_array(int *arr, int size, const char *label) {
    printf("%s: ", label);
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void task5() {
    int n;
    printf("Enter the size of the array: ");
    scanf("%d", &n);
    int array[n];

    printf("Enter %d elements for the array:\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &array[i]);
    }

    int size = n;

    int even_array[size], odd_array[size];
    int even_count = 0, odd_count = 0;

    separate_even_odd(array, size, even_array, odd_array, &even_count, &odd_count);

    print_array(array, size, "Original array");
    print_array(even_array, even_count, "Even elements");
    print_array(odd_array, odd_count, "Odd elements");
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