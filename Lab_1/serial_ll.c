#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Define the structure for a list node
struct list_node_s {
    int data;
    struct list_node_s *next;
};

int *opr_values;
int m_member, m_insert, m_delete;
int n;
int m;
int thread_count;

// Function prototypes
void print_list(struct list_node_s *head);
int Member(int value, struct list_node_s *head_p);
int Insert(int value, struct list_node_s **head_pp);
int Delete(int value, struct list_node_s **head_pp);
void ExecuteOperations(struct list_node_s **head_pp);

// Function to check if a value is a member of the linked list
int Member(int value, struct list_node_s *head_p) {
    struct list_node_s *curr_p = head_p;

    // Traverse the list to find the value
    while (curr_p != NULL && curr_p->data < value)
        curr_p = curr_p->next;

    return (curr_p != NULL && curr_p->data == value);
}

// Function to insert a new node into the list
int Insert(int value, struct list_node_s **head_pp) {
    struct list_node_s *curr_p = *head_pp;
    struct list_node_s *pred_p = NULL;
    struct list_node_s *temp_p;

    // Traverse the list to find the correct position to insert
    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    // If value is not already in the list, insert the new node
    if (curr_p == NULL || curr_p->data > value) {
        temp_p = malloc(sizeof(struct list_node_s));
        temp_p->data = value;
        temp_p->next = curr_p;

        if (pred_p == NULL) {
            // New first node
            *head_pp = temp_p;
        } else {
            pred_p->next = temp_p;
        }
        return 1;
    } else {
        // Value already in list
        return 0;
    }
}

// Function to delete a node from the list
int Delete(int value, struct list_node_s **head_pp) {
    struct list_node_s *curr_p = *head_pp;
    struct list_node_s *pred_p = NULL;

    // Traverse the list to find the node to delete
    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    // If the node is found, delete it
    if (curr_p != NULL && curr_p->data == value) {
        if (pred_p == NULL) {
            // Deleting the first node in the list
            *head_pp = curr_p->next;
            free(curr_p);
        } else {
            pred_p->next = curr_p->next;
            free(curr_p);
        }
        return 1;
    } else {
        // Value isn't in list
        return 0;
    }
}

// Function to execute the operations sequentially
void ExecuteOperations(struct list_node_s **head_pp) {
    for (int i = 0; i < m; i++) {
        if (i < m_member) {
            printf("Looking for %d: %d\n", opr_values[i], Member(opr_values[i], *head_pp));
        } else if (i < m_member + m_insert) {
            printf("Inserting %d\n", opr_values[i]);
            Insert(opr_values[i], head_pp);
        } else {
            printf("Deleting %d\n", opr_values[i]);
            Delete(opr_values[i], head_pp);
        }
    }
}

void print_list(struct list_node_s *head) {
    struct list_node_s *current = head;
    while (current != NULL) {
        printf("%d -> ", current->data);
        current = current->next;
    }
    printf("NULL\n");
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <thread_count> <m_member_ratio> <m_insert_ratio> <m_delete_ratio>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    thread_count = 1;
    n = 1000;
    m = 10000;
    m_member = m * atof(argv[2]);
    m_insert = m * atof(argv[3]);
    m_delete = m * atof(argv[4]);
    printf("%d %d %d %d\n", n, m, m_member, m_insert, m_delete);

    opr_values = malloc(m * sizeof(int));
    int i, ins_value;

    // Create linked list head pointer
    struct list_node_s *head = NULL;

    srand(time(NULL)); // Set different random seed for each execution

    for (i = 0; i < n; i++) {
        ins_value = rand() % 65535; // value should be between 2^16 - 1
        Insert(ins_value, &head);
    }
    for (i = 0; i < m; i++) {
        opr_values[i] = rand() % 65535; // value should be between 2^16 - 1
    }

    print_list(head);

    clock_t start, end;
    double cpu_time_used;
    start = clock();

    ExecuteOperations(&head);

    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Time taken: %f\n", cpu_time_used);

    // Save the results to a file
    FILE *file = fopen("results.txt", "a");
    if (file == NULL) {
        fprintf(stderr, "Error opening file for writing\n");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "n: %d, m: %d, m_member: %d, m_insert: %d, m_delete: %d, thread_count: %d, time: %f\n", n, m, m_member, m_insert, m_delete, thread_count, cpu_time_used);
    fclose(file);

    // Free the allocated memory
    struct list_node_s *current = head;
    struct list_node_s *next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    free(opr_values);

    return 0;
}
