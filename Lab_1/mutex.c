#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// Define the structure for a list node
struct list_node_s
{
    int data;
    struct list_node_s *next;
};
void print_list(struct list_node_s *head)
{
    struct list_node_s *current = head;
    while (current != NULL)
    {
        printf("%d -> ", current->data);
        current = current->next;
    }
    printf("NULL\n");
}

// Mutex for the linked list
pthread_mutex_t list_mutex, list_mutex1;
int *opr_values;
int count, count_m, count_i, count_d = 0;
int m_member, m_insert, m_delete;
int n;
int m;
int thread_count;

// Function to check if a value is a member of the linked list
int Member(int value, struct list_node_s *head_p)
{
    struct list_node_s *curr_p = head_p;
    pthread_mutex_lock(&list_mutex);
    count_m++;
    // Traverse the list to find the value
    while (curr_p != NULL && curr_p->data < value)
        curr_p = curr_p->next;

    int found = (curr_p != NULL && curr_p->data == value);

    pthread_mutex_unlock(&list_mutex);
    return found;
}

// Function to insert a new node into the list
int InsertWithoutParellel(int value, struct list_node_s **head_pp)
{

    struct list_node_s *curr_p = *head_pp;
    struct list_node_s *pred_p = NULL;
    struct list_node_s *temp_p;

    // Traverse the list to find the correct position to insert
    while (curr_p != NULL && curr_p->data < value)
    {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    // If value is not already in the list, insert the new node
    if (curr_p == NULL || curr_p->data > value)
    {
        temp_p = malloc(sizeof(struct list_node_s));
        temp_p->data = value;
        temp_p->next = curr_p;

        if (pred_p == NULL)
        {
            // New first node
            *head_pp = temp_p;
        }
        else
        {
            pred_p->next = temp_p;
        }
        return 1;
    }
    else
    {
        // Value already in list
        return 0;
    }
}

// Function to insert a new node into the list
int Insert(int value, struct list_node_s **head_pp)
{
    pthread_mutex_lock(&list_mutex);
    count_i++;

    struct list_node_s *curr_p = *head_pp;
    struct list_node_s *pred_p = NULL;
    struct list_node_s *temp_p;

    // Traverse the list to find the correct position to insert
    while (curr_p != NULL && curr_p->data < value)
    {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    // If value is not already in the list, insert the new node
    if (curr_p == NULL || curr_p->data > value)
    {
        temp_p = malloc(sizeof(struct list_node_s));
        temp_p->data = value;
        temp_p->next = curr_p;

        if (pred_p == NULL)
        {
            // New first node
            *head_pp = temp_p;
        }
        else
        {
            pred_p->next = temp_p;
        }
        pthread_mutex_unlock(&list_mutex);
        return 1;
    }
    else
    {
        // Value already in list
        pthread_mutex_unlock(&list_mutex);
        return 0;
    }
}

// Function to delete a node from the list
int Delete(int value, struct list_node_s **head_pp)
{
    pthread_mutex_lock(&list_mutex);
    count_d++;

    struct list_node_s *curr_p = *head_pp;
    struct list_node_s *pred_p = NULL;

    // Traverse the list to find the node to delete
    while (curr_p != NULL && curr_p->data < value)
    {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    // If the node is found, delete it
    if (curr_p != NULL && curr_p->data == value)
    {
        if (pred_p == NULL)
        {
            // Deleting the first node in the list
            *head_pp = curr_p->next;
            free(curr_p);
        }
        else
        {
            pred_p->next = curr_p->next;
            free(curr_p);
        }
        pthread_mutex_unlock(&list_mutex);
        return 1;
    }
    else
    {
        // Value isn't in list
        pthread_mutex_unlock(&list_mutex);
        return 0;
    }
}

// Function executed by each thread
void *ThreadFunction(void *arg)
{
    struct list_node_s **head_pp = (struct list_node_s **)arg;
    pthread_mutex_lock(&list_mutex1);
    int i = count++;
    pthread_mutex_unlock(&list_mutex1);
    while (i < m)
    {
        // only one thread can get access to the linked-list at a time
        if (count_m < m_member)
        {
            printf("Looking for %d: ", opr_values[i]);
            printf("%d\n", Member(opr_values[i], *head_pp));
        }
        else if (count_i < m_insert)
        {
            printf("Inserting %d\n", opr_values[i]);
            Insert(opr_values[i], head_pp);
        }
        else
        {
            printf("Deleting %d\n", opr_values[i]);
            Delete(opr_values[i], head_pp);
        }
        pthread_mutex_lock(&list_mutex1);
        i = count++;
        pthread_mutex_unlock(&list_mutex1);
    }
    // Example operations
    // Insert(3, head_pp);
    // Insert(1, head_pp);
    // Insert(2, head_pp);
    // printf("Is 2 in the list? %d\n", Member(2, *head_pp)); // Output: 1
    // printf("Is 4 in the list? %d\n", Member(4, *head_pp)); // Output: 0
    // Delete(2, head_pp);
    // printf("Is 2 in the list after deletion? %d\n", Member(2, *head_pp)); // Output: 0

    return NULL;
}

int main(int argc, char *argv[])
{
    thread_count = strtol(argv[1], NULL, 10);
    n = 1000;
    m = 10000;
    m_member = m * (float)atof(argv[2]);
    m_insert = m * (float)atof(argv[3]);
    m_delete = m * (float)atof(argv[4]);
    printf(" %d %d %d %d %d\n", n, m, m_member, m_insert, m_delete);

    opr_values = malloc(m * sizeof(int));
    int i, ins_value;

    // Create linked list head pointer
    struct list_node_s *head = NULL;

    for (i = 0; i < n; i++)
    {
        ins_value = rand() % 65535; // value should be between 2^16 - 1
        InsertWithoutParellel(ins_value, &head);
    }
    for (i = 0; i < m; i++)
    {
        opr_values[i] = rand() % 65535; // value should be between 2^16 - 1
    }

    print_list(head);

    // Initialize mutex
    pthread_mutex_init(&list_mutex, NULL);
    pthread_mutex_init(&list_mutex1, NULL);

    // Create threads
    // pthread_t thread1;
    // pthread_create(&thread1, NULL, ThreadFunction, (void *)&head);
    // pthread_create(&thread2, NULL, ThreadFunction, (void *)&head);

    // Wait for threads to finish
    // pthread_join(thread1, NULL);
    // pthread_join(thread2, NULL);

    pthread_t *thread_handles;
    thread_handles = malloc(thread_count * sizeof(pthread_t));
    long thread;

    clock_t start, end;
    double cpu_time_used;
    start = clock();
    for (thread = 0; thread < thread_count; thread++)
    {
        printf("Creating thread %ld\n", thread);
        pthread_create(&thread_handles[thread], NULL, ThreadFunction, (void *)&head);
    }
    for (thread = 0; thread < thread_count; thread++)
    {
        pthread_join(thread_handles[thread], NULL);
    }
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Time taken: %f\n", cpu_time_used);

    // Destroy mutex
    pthread_mutex_destroy(&list_mutex);
    pthread_mutex_destroy(&list_mutex1);
    free(thread_handles);

    // Free the allocated memory
    struct list_node_s *current = head;
    struct list_node_s *next;
    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }

    return 0;
}
