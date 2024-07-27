#include <stdio.h>
#include <stdlib.h>

// Define the structure for a list node
struct list_node_s {
    int data;
    struct list_node_s* next;
};

// Function to check if a value is a member of the linked list
int Member(int value, struct list_node_s* head_p) {
    struct list_node_s* curr_p = head_p;

    // Traverse the list to find the value
    while (curr_p != NULL && curr_p->data < value)
        curr_p = curr_p->next;

    // Check if the value is found
    if (curr_p == NULL || curr_p->data > value) {
        return 0; // Value is not in the list
    } else {
        return 1; // Value is in the list
    }
}

// Function to insert a new node into the list
int Insert(int value, struct list_node_s** head_pp) {
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
        return 1;
    } else {
        // Value already in list
        return 0;
    }
}

// Function to delete a node from the list
int Delete(int value, struct list_node_s** head_pp) {
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
        return 1;
    } else {
        // Value isn't in list
        return 0;
    }
}

int main() {
    // Example usage of the Member, Insert, and Delete functions

    // Create linked list nodes
    struct list_node_s* head = NULL;

    // Insert values into the list
    Insert(3, &head);
    Insert(1, &head);
    Insert(2, &head);

    // Check if values are in the list
    printf("Is 2 in the list? %d\n", Member(2, head)); // Output: 1
    printf("Is 4 in the list? %d\n", Member(4, head)); // Output: 0

    // Delete a value from the list
    Delete(2, &head);
    printf("Is 2 in the list after deletion? %d\n", Member(2, head)); // Output: 0

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
