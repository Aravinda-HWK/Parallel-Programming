#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Define the structure for a list node
struct list_node_s {
    int data;
    struct list_node_s* next;
};

// Mutex for the linked list
pthread_mutex_t list_mutex;

// Function to check if a value is a member of the linked list
int Member(int value, struct list_node_s* head_p) {
    struct list_node_s* curr_p = head_p;
    pthread_mutex_lock(&list_mutex);

    // Traverse the list to find the value
    while (curr_p != NULL && curr_p->data < value)
        curr_p = curr_p->next;

    int found = (curr_p != NULL && curr_p->data == value);

    pthread_mutex_unlock(&list_mutex);
    return found;
}

// Function to insert a new node into the list
int Insert(int value, struct list_node_s** head_pp) {
    pthread_mutex_lock(&list_mutex);

    struct list_node_s* curr_p = *head_pp;
    struct list_node_s* pred_p = NULL;
    struct list_node_s* temp_p;

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
int Delete(int value, struct list_node_s** head_pp) {
    pthread_mutex_lock(&list_mutex);

    struct list_node_s* curr_p = *head_pp;
    struct list_node_s* pred_p = NULL;

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
void* ThreadFunction(void* arg) {
    struct list_node_s** head_pp = (struct list_node_s**)arg;

    // Example operations
    Insert(3, head_pp);
    Insert(1, head_pp);
    Insert(2, head_pp);
    printf("Is 2 in the list? %d\n", Member(2, *head_pp)); // Output: 1
    printf("Is 4 in the list? %d\n", Member(4, *head_pp)); // Output: 0
    Delete(2, head_pp);
    printf("Is 2 in the list after deletion? %d\n", Member(2, *head_pp)); // Output: 0

    return NULL;
}

int main() {
    // Initialize mutex
    pthread_mutex_init(&list_mutex, NULL);

    // Create linked list head pointer
    struct list_node_s* head = NULL;

    // Create threads
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, ThreadFunction, (void*)&head);
    pthread_create(&thread2, NULL, ThreadFunction, (void*)&head);

    // Wait for threads to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // Destroy mutex
    pthread_mutex_destroy(&list_mutex);

    // Free the allocated memory
    struct list_node_s* current = head;
    struct list_node_s* next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    return 0;
}
