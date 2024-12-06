#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


int *f1(int i) {
    int *dynamic_i = malloc(sizeof(int));
    if (dynamic_i != NULL) {
        *dynamic_i = i;
    }
    return dynamic_i;
}

void task1(void) {
    int *p;
    p = f1(10);
    if (p != NULL) {
        printf("%d\n", *p);
        printf("%d\n", *p);
        free(p);
    }
}


typedef struct ListEl {
    int data;
    struct ListEl *nxt;
} ListEl;

ListEl *list_build(void) {
    int values[] = { 8, 14, 13, 17, 1, 19, 16, 5, 3, 11, 2,
                     15, 9, 10, 6, 22, 4, 7, 18, 27, -15 };
    ListEl *head = NULL;

    int i;
    for (i = 0; values[i] > -1; ++i) {
        ListEl *u;
        u = (ListEl*) malloc(sizeof(ListEl));
        u->nxt = head;
        u->data = values[i];
        head = u;
    }
    return head;
}

void print_list(ListEl* head) {
    ListEl* current = head;
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->nxt;
    }
    printf("\n");
}

void task2(void) {
    ListEl *head;
    
    head = list_build();

    print_list(head);
}


int getLength(struct ListEl* head) {
    int count = 0;
    struct ListEl* temp = head;
    while (temp != NULL) {
        count++;
        temp = temp->nxt;
    }
    return count;
}

void task3(void) {
    ListEl *head;
    head = list_build();
    printf("Length of the list is %d\n", getLength(head));
}


void delete_list(struct ListEl* head) {
    struct ListEl* current = head;
    struct ListEl* next;
    while (current != NULL) {
        next = current->nxt;
        free(current);
        current = next;
    }
}

void task4(void) {
    ListEl *head;
    head = list_build();
    delete_list(head);
}


ListEl *list_insert_athead(struct ListEl *head, int data) {
    ListEl *new_node = (ListEl*) malloc(sizeof(ListEl));
    new_node->data = data;
    new_node->nxt = head;
    return new_node;
}

void task5(void) {
    ListEl *head;
    head = list_build();
    head = list_insert_athead(head, 420);
    head = list_insert_athead(head, 69);
    print_list(head);
    delete_list(head);
}


ListEl* list_insert_atend(ListEl* head, int value) {
    ListEl* new_node = (ListEl*)malloc(sizeof(ListEl));
    new_node->data = value;
    new_node->nxt = NULL;

    if (head == NULL) {
        return new_node;
    }

    ListEl* current = head;
    while (current->nxt != NULL) {
        current = current->nxt;
    }

    current->nxt = new_node;
    return head;
}

void task6() {
    ListEl *head;
    head = list_build();
    head = list_insert_atend(head, 420);
    head = list_insert_atend(head, 69);
    print_list(head);
    delete_list(head);
}


ListEl *list_search(ListEl *head, int what) {
    ListEl *p;
    for (p = head; p != NULL; p = p->nxt)
        if (p->data == what)
            return p;
    return NULL;
}

void task7() {
    ListEl *head;
    head = list_build();
    ListEl *hit = list_search(head, 44);

    delete_list(head);
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
    } else if (task_number == 7) {
        task7();
    } else {
        printf("Invalid task number.\n");
    }
    return 0;
}
