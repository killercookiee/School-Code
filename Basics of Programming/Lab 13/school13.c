#include <stdio.h>
#include <stdlib.h>

typedef struct BTree {
    int value;
    struct BTree *left, *right;
} BTree;

BTree *insert(BTree *root, int value) {
    if (root == NULL) {
        BTree *new_node = (BTree*) malloc(sizeof(BTree));
        new_node->value = value;
        new_node->left = new_node->right = NULL;
        return new_node;
    }
    if (value < root->value) {        /* insert left */
        root->left = insert(root->left, value);
    }
    else if (value > root->value) {   /* insert right */
        root->right = insert(root->right, value);
    }
    else {
        /* already in the tree */
    }
    return root;
}


void task1() {
    int sample[] = {15, 96, 34, 12, 14, 56, 21, 11, 10, 9, 78, 43, 0};
    BTree *root = NULL;
    int i;
    for (i = 0; sample[i] > 0; i++)
        root = insert(root, sample[i]);

    /* Call your functions here! */

}


// Task 2
void print_tree(BTree *root) {
    if (root != NULL) {
        print_tree(root->left);
        printf("%d ", root->value);
        print_tree(root->right);
    }
}

void free_tree(BTree *root) {
    if (root != NULL) {
        free_tree(root->left);
        free_tree(root->right);
        free(root);
    }
}

void task2() {
    int sample[] = {15, 96, 34, 12, 14, 56, 21, 11, 10, 9, 78, 43, 0};
    BTree *root = NULL;
    int i;
    for (i = 0; sample[i] > 0; i++)
        root = insert(root, sample[i]);

    /* Call your functions here! */
    print_tree(root);
    printf("\n");

    free_tree(root);
}


// Task 3
int count_nodes(BTree *root) {
    if (root == NULL) {
        return 0;
    }
    return 1 + count_nodes(root->left) + count_nodes(root->right);
}

int sum_values(BTree *root) {
    if (root == NULL) {
        return 0;
    }
    return root->value + sum_values(root->left) + sum_values(root->right);
}

void task3() {
    int sample[] = {15, 96, 34, 12, 14, 56, 21, 11, 10, 9, 78, 43, 0};
    BTree *root = NULL;
    int i;
    for (i = 0; sample[i] > 0; i++)
        root = insert(root, sample[i]);

    /* Call your functions here! */
    int node_count = count_nodes(root);
    printf("Node count: %d\n", node_count);

    int sum = sum_values(root);
    printf("Sum: %d\n", sum);

    free_tree(root);
}



// Task 4
BTree *find_value(BTree *root, int value) {
    if (root == NULL) {
        return NULL;
    }
    if (root->value == value) {
        return root;
    } else if (value < root->value) {
        return find_value(root->left, value);
    } else {
        return find_value(root->right, value);
    }
}

void task4() {
    int sample[] = {15, 96, 34, 12, 14, 56, 21, 11, 10, 9, 78, 43, 0};
    BTree *root = NULL;
    int i;
    for (i = 0; sample[i] > 0; i++)
        root = insert(root, sample[i]);

    /* Call your functions here! */
    int value_to_find;
    printf("Enter a value to find: ");
    scanf("%d", &value_to_find);
    BTree *found_node = find_value(root, value_to_find);
    if (found_node != NULL) {
        printf("Value %d found in the tree.\n", found_node->value);
    } else {
        printf("Value %d not found in the tree.\n", value_to_find);
    }

    free_tree(root);
}



// Task 5
void negate_tree(BTree *root) {
    if (root != NULL) {
        root->value *= -1;
        negate_tree(root->left);
        negate_tree(root->right);
    }
}

BTree *find_value_negated(BTree *root, int value) {
    if (root == NULL) {
        return NULL;
    }
    if (root->value == value) {
        return root;
    } else if (value > root->value) {
        return find_value_negated(root->left, value);
    } else {
        return find_value_negated(root->right, value);
    }
}

void task5() {
    int sample[] = {15, 96, 34, 12, 14, 56, 21, 11, 10, 9, 78, 43, 0};
    BTree *root = NULL;
    int i;
    for (i = 0; sample[i] > 0; i++)
        root = insert(root, sample[i]);

    /* Call your functions here! */
    negate_tree(root);
    printf("Negated Tree: ");
    print_tree(root);
    printf("\n");

    int value_to_find;
    printf("Enter a value to find in the negated tree: ");
    scanf("%d", &value_to_find);
    BTree *found_node = find_value_negated(root, value_to_find);
    if (found_node != NULL) {
        printf("Value %d found in the negated tree.\n", found_node->value);
    } else {
        printf("Value %d not found in the negated tree.\n", value_to_find);
    }

    int sum = sum_values(root);
    printf("Sum: %d\n", sum);

    free_tree(root);
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
