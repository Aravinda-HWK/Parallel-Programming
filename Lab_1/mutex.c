#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// Define the structure for a list node
struct list_node_s {
    int data;
    struct list_node_s *next;
};

// Mutex for the linked list
pthread_mutex_t list_mutex;
int *opr_values;
int m_member, m_insert, m_delete;
int n;
int m;
int thread_count;

// Structure to hold thread arguments
typedef struct {
    struct list_node_s **head_pp;
    long rank;
} thread_data_t;

// Function prototypes
void print_list(struct list_node_s *head);
int Member(int value, struct list_node_s *head_p);
int InsertWithoutParellel(int value, struct list_node_s **head_pp);
int Insert(int value, struct list_node_s **head_pp);
int Delete(int value, struct list_node_s **head_pp);
void *ThreadFunction(void *arg);

// Function to check if a value is a member of the linked list
int Member(int value, struct list_node_s *head_p) {
    struct list_node_s *curr_p = head_p;
    pthread_mutex_lock(&list_mutex);
    // Traverse the list to find the value
    while (curr_p != NULL && curr_p->data < value)
        curr_p = curr_p->next;

    int found = (curr_p != NULL && curr_p->data == value);

    pthread_mutex_unlock(&list_mutex);
    return found;
}

// Function to insert a new node into the list without parallelism
int InsertWithoutParellel(int value, struct list_node_s **head_pp) {
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

// Function to insert a new node into the list
int Insert(int value, struct list_node_s **head_pp) {
    pthread_mutex_lock(&list_mutex);

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
        pthread_mutex_unlock(&list_mutex);
        return 1;
    } else {
        // Value already in list
        pthread_mutex_unlock(&list_mutex);
        return 0;
    }
}

// Function to delete a node from the list
int Delete(int value, struct list_node_s **head_pp) {
    pthread_mutex_lock(&list_mutex);

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
        pthread_mutex_unlock(&list_mutex);
        return 1;
    } else {
        // Value isn't in list
        pthread_mutex_unlock(&list_mutex);
        return 0;
    }
}

// Function executed by each thread
void *ThreadFunction(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;
    struct list_node_s **head_pp = data->head_pp;
    long rank = data->rank;

    int start = rank;
    int step = thread_count;

    for (int i = start; i < m; i += step) {
        if (i < m_member) {
            printf("Thread %ld: Looking for %d: %d\n", rank, opr_values[i], Member(opr_values[i], *head_pp));
        } else if (i < m_member + m_insert) {
            printf("Thread %ld: Inserting %d\n", rank, opr_values[i]);
            Insert(opr_values[i], head_pp);
        } else {
            printf("Thread %ld: Deleting %d\n", rank, opr_values[i]);
            Delete(opr_values[i], head_pp);
        }
    }
    return NULL;
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

    thread_count = strtol(argv[1], NULL, 10);
    n = 1000;
    m = 10000;
    m_member = m * atof(argv[2]);
    m_insert = m * atof(argv[3]);
    m_delete = m * atof(argv[4]);
    printf("%d %d %d %d %d\n", n, m, m_member, m_insert, m_delete);

    opr_values = malloc(m * sizeof(int));
    int i, ins_value;

    // Create linked list head pointer
    struct list_node_s *head = NULL;

    srand(time(NULL)); // Set different random seed for each execution

    for (i = 0; i < n; i++) {
        ins_value = rand() % 65535; // value should be between 2^16 - 1
        InsertWithoutParellel(ins_value, &head);
    }
    for (i = 0; i < m; i++) {
        opr_values[i] = rand() % 65535; // value should be between 2^16 - 1
    }

    print_list(head);

    // Initialize mutex
    pthread_mutex_init(&list_mutex, NULL);

    // Create threads
    pthread_t *thread_handles = malloc(thread_count * sizeof(pthread_t));
    thread_data_t *thread_data_array = malloc(thread_count * sizeof(thread_data_t));

    clock_t start, end;
    double cpu_time_used;
    start = clock();
    for (long thread = 0; thread < thread_count; thread++) {
        thread_data_array[thread].head_pp = &head;
        thread_data_array[thread].rank = thread;
        printf("Creating thread %ld\n", thread);
        pthread_create(&thread_handles[thread], NULL, ThreadFunction, (void *)&thread_data_array[thread]);
    }
    for (long thread = 0; thread < thread_count; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }
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

    // Destroy mutex
    pthread_mutex_destroy(&list_mutex);
    free(thread_handles);
    free(thread_data_array);

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