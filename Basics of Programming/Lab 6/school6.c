#include <stdio.h>
#include <math.h>



// Task 1
typedef struct Date {
    int year, month, day;
} Date;

typedef struct Competitor {
    char name[31];
    Date birth;
    int rank;
} Competitor;

void Date_print(Date d) {
    /* print year, month and day */
    printf("%d-%02d-%02d\n", d.year, d.month, d.day);
}

void Competitor_print(Competitor c) {
    /* print all data of the competitor */
    printf("Name: %s, Rank: %d, Birthdate: ", c.name, c.rank);
    Date_print(c.birth);
}

void task1() {
    Competitor competitors[5] = {
        { "Am, Erica", {1984, 5, 6}, 1 },
        { "Abnorm, Al", {1982, 9, 30}, 3 },
        { "Pri, Mary", {1988, 8, 25}, 2 },
        { "Duck, Ling", {1979, 6, 10}, 5 },
        { "Mac, Donald", {1992, 4, 5}, 4 },
    };

    /* name of competitor 0 - printf %s */
    printf("Name of competitor 0: %s\n", competitors[0].name);

    /* rank of competitor 2 */
    printf("Rank of competitor 2: %d\n", competitors[2].rank);

    /* birth date of competitor 4, use the given function */
    printf("Birth date of competitor 4: ");
    Date_print(competitors[4].birth);

    /* the first letter of the name of competitor 1 (a string is an array of characters) */
    printf("First letter of competitor 1's name: %c\n", competitors[1].name[0]);

    /* is competitor 1 among the best three? yes/no, may use ?: operator */
    printf("Is competitor 1 among the best three? %s\n", competitors[1].rank <= 3 ? "yes" : "no");

    /* is competitor 4 faster than competitor 3? */
    printf("Is competitor 4 faster than competitor 3? %s\n", competitors[4].rank < competitors[3].rank ? "yes" : "no");

    /* was competitor 1 born in the same year as competitor 2? */
    printf("Was competitor 1 born in the same year as competitor 2? %s\n", competitors[1].birth.year == competitors[2].birth.year ? "yes" : "no");

    /* complete the Competitor_print() function,
     * then print all data of competitor 1 */
    printf("Data of competitor 1:\n");
    Competitor_print(competitors[1]);

    /* at last print all data of all competitors. */
    for (int i = 0; i < 5; i++) {
        Competitor_print(competitors[i]);
    }
}



// Task 2
typedef struct {
    float x;
    float y;
    float r;
} Circle;

Circle create_circle() {
    Circle c;
    printf("Enter x coordinate of the center: ");
    scanf("%f", &c.x);
    printf("Enter y coordinate of the center: ");
    scanf("%f", &c.y);
    printf("Enter the radius: ");
    scanf("%f", &c.r);
    return c;
}

float distance_between_centers(Circle c1, Circle c2) {
    return sqrtf((c2.x - c1.x) * (c2.x - c1.x) + (c2.y - c1.y) * (c2.y - c1.y));
}

int do_circles_overlap(Circle c1, Circle c2) {
    float distance = distance_between_centers(c1, c2);
    return distance < (c1.r + c2.r);
}

void task2() {
    printf("Enter the parameters for the first circle:\n");
    Circle circle1 = create_circle();

    printf("Enter the parameters for the second circle:\n");
    Circle circle2 = create_circle();

    if (do_circles_overlap(circle1, circle2)) {
        printf("The circles overlap.\n");
    } else {
        printf("The circles do not overlap.\n");
    }
}



// Task 3
typedef struct {
    float x;
    float y;
} Point;

float dist(Point p1, Point p2) {
    return sqrtf((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}

int equal(Point p1, Point p2) {
    return (p1.x == p2.x) && (p1.y == p2.y);
}

Point read_point() {
    Point p;
    printf("Enter x coordinate: ");
    scanf("%f", &p.x);
    printf("Enter y coordinate: ");
    scanf("%f", &p.y);
    return p;
}

void task3() {
    Point start_point, current_point, previous_point;
    float total_length = 0.0;

    printf("Enter the coordinates of the first point (starting point):\n");
    start_point = read_point();
    previous_point = start_point;

    while (1) {
        printf("Enter the coordinates of the next point (or the same as the first to finish):\n");
        current_point = read_point();

        if (equal(current_point, start_point)) {
            total_length += dist(previous_point, start_point);
            break;
        }

        total_length += dist(previous_point, current_point);
        previous_point = current_point;
    }

    printf("The total length of the fence required is: %.2f\n", total_length);
}



// Task 4
typedef struct {
    float vx;
    float vy;
} Vector;

float vector_length(Vector v) {
    return sqrtf(v.vx * v.vx + v.vy * v.vy);
}

Vector vector_sum(Vector v1, Vector v2) {
    Vector result;
    result.vx = v1.vx + v2.vx;
    result.vy = v1.vy + v2.vy;
    return result;
}

void task4() {
    Vector v1 = {1.0, 2.0};
    Vector v2 = {-0.5, 3.0};

    Vector sum = vector_sum(v1, v2);
    printf("Sum of vectors: vx = %.2f m/s, vy = %.2f m/s\n", sum.vx, sum.vy);

    float length = vector_length(sum);
    printf("Length of the sum vector: %.2f m/s\n", length);

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
    else {
        printf("Invalid task number.\n");
    }
    return 0;
}