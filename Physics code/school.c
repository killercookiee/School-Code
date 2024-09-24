#include <stdio.h>
#include <math.h>  // For the sqrt() function

void task2() {
    // Variables to store the coordinates of two points
    double x1, y1, x2, y2;
    
    // Read the coordinates of the first point
    printf("x1: ");
    scanf("%lf", &x1);
    printf("y1: ");
    scanf("%lf", &y1);
    printf("x2: ");
    scanf("%lf", &x2);
    printf("y2: ");
    scanf("%lf", &y2);

    // Calculate the Euclidean distance using the Pythagorean formula
    double distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    
    // Output the distance
    printf("The Euclidean distance between the points (%.2lf, %.2lf)-(%2.lf, %.2lf) is: %.6lf\n", x1, y1, x2, y2, distance);
}

void task3() {
    // Variables to store the coordinates of two points
    double a=0, b=0, c=0;

    printf("a: ");
    scanf("%lf", &a);
    printf("b: ");
    scanf("%lf", &b);
    printf("c: ");
    scanf("%lf", &c);

    // Calculate the discriminant
    double discriminant = b * b - 4 * a * c;
    
    // Calculate the two solutions using the quadratic formula
    double X1 = (-b + sqrt(discriminant)) / (2 * a);
    double X2 = (-b - sqrt(discriminant)) / (2 * a);
    
    // Print the result in one line
    printf("%.0lfx^2%+.0lfx%+.0lf=0\tx1=%.1lf, x2=%.1lf\n", a, b, c, X1, X2);    
}

void task4() {
    int upperbound, lowerbound;

    printf("upperbound: ");
    scanf("%d", &upperbound);
    printf("lower: ");
    scanf("%d", &lowerbound);    

    // Loop from the lower bound to the upper bound (inclusive)
    while (lowerbound <= upperbound) {
        printf("%d\n", lowerbound);  // Print the current number
        lowerbound++;  // Advance the number by 1
    }
}

void task5() {
    int length;
    
    // Read the length of the line from the user
    printf("Length of the line? \n");
    scanf("%d", &length);
    
    // Print the starting '+'
    printf("+");
    
    // Print the '-' characters
    for (int i = 0; i < length; i++) {
        printf("-");
    }
    
    // Print the ending '+'
    printf("+\n");
}

void task6() {
    int N;
    
    // Read the number of square numbers to print
    printf("Enter the number of square numbers to print: ");
    scanf("%d", &N);
    
    // Print the first N square numbers
    for (int i = 1; i <= N; i++) {
        printf("%d ", i * i);
    }
    printf("\n\n");
    

    int N2;
    
    // Read the upper bound value N
    printf("Enter the upper bound (N2): ");
    scanf("%d", &N2);
    
    // Print all square numbers less than N
    int i = 1;
    while (i * i < N2) {
        printf("%d ", i * i);
        i++;
    }
    printf("\n\n");    


    double radius = 10.0;  // Starting radius in cm
    double volume;
    
    // Convert 1 cubic meter to cubic cm (1 cubic meter = 1,000,000 cubic cm)
    const double max_volume_cm3 = 1000000.0;
    
    printf("Volumes of spheres with radius starting from 10 cm, increasing by 10 cm each time, and volume less than 1 cubic meter:\n");
    
    // Loop to calculate volumes for radii starting from 10 cm, increasing by 10 cm
    while (1) {
        volume = (4.0 / 3.0) * M_PI * pow(radius, 3);  // Volume in cubic cm
        
        if (volume >= max_volume_cm3) {
            break;  // Exit the loop if volume is greater than or equal to 1 cubic meter
        }
        
        printf("Radius: %.2f cm, Volume: %.2f cubic meters\n", radius, volume / 1000000.0);  // Convert volume to cubic meters
        
        radius += 10.0;  // Increase radius by 10 cm
    }    
}

int main() {
    int task_number;
    
    printf("Task number: ");
    scanf("%d", &task_number);  // Use %d for integer input, and pass the address of task_number
    
    // Call the appropriate function based on the task number
    if (task_number == 2) {
        task2();  // Call task2
    }
    else if (task_number == 3) {
        task3();  // Call task3
    }
    else if (task_number == 4) {
        task4();  // Call task4
    }
    else if (task_number == 5) {
        task5();  // Call task4
    }
    else if (task_number == 6) {
        task6();  // Call task4
    }        
    else {
        printf("Invalid task number.\n");  // Handle invalid task numbers
    }


    return 0;
}
