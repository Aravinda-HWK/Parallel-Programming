#include <stdio.h>
#include <stdlib.h>
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

int m_member, m_insert, m_delete;
int n;
int m;
int *opr_values;
int count, count_m, count_i, count_d = 0;

// Function to check if a value is a member of the linked list
int Member(int value, struct list_node_s *head_p)
{
    struct list_node_s *curr_p = head_p;
    count_m++;

    // Traverse the list to find the value
    while (curr_p != NULL && curr_p->data < value)
        curr_p = curr_p->next;

    // Check if the value is found
    if (curr_p == NULL || curr_p->data > value)
    {
        return 0; // Value is not in the list
    }
    else
    {
        return 1; // Value is in the list
    }
}

// Function to insert a new node into the list
int Insert(int value, struct list_node_s **head_pp)
{
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
        return 1;
    }
    else
    {
        // Value already in list
        return 0;
    }
}

// Function to delete a node from the list
int Delete(int value, struct list_node_s **head_pp)
{
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
        return 1;
    }
    else
    {
        // Value isn't in list
        return 0;
    }
}

int main(int argc, char *argv[])
{
    // Example usage of the Member, Insert, and Delete functions

    n = 1000;
    m = 10000;
    m_member = m * (float)atof(argv[1]);
    m_insert = m * (float)atof(argv[2]);
    m_delete = m * (float)atof(argv[3]);
    printf("%d %d %d \n", m_member, m_insert, m_delete);
    opr_values = malloc(m * sizeof(int));
    int i, ins_value;

    // Create linked list nodes
    struct list_node_s *head = NULL;

    for (i = 0; i < n; i++)
    {
        ins_value = rand() % 65535; // value should be between 2^16 - 1
        Insert(ins_value, &head);
    }

    print_list(head);
    for (i = 0; i < m; i++)
    {
        opr_values[i] = rand() % 65535; // value should be between 2^16 - 1
    }

    count_i = 0;
    int j = count++;

    clock_t start, end;
    double cpu_time_used;
    start = clock();
    while (j < m)
    {
        if (count_m < m_member)
        {
            printf("Looking for %d in %d in %d: ", opr_values[j], count_i, count_m);
            printf("%d\n", Member(opr_values[j], head));
        }
        else if (count_i < m_insert)
        {
            printf("Inserting %d\n", opr_values[j]);
            Insert(opr_values[j], &head);
        }
        else
        {
            printf("Deleting %d\n", opr_values[j]);
            Delete(opr_values[j], &head);
        }
        j = count++;
    }

    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Time taken: %f\n", cpu_time_used);

    // Insert values into the list
    // Insert(3, &head);
    // Insert(1, &head);
    // Insert(2, &head);

    // Check if values are in the list
    // printf("Is 2 in the list? %d\n", Member(2, head)); // Output: 1
    // printf("Is 4 in the list? %d\n", Member(4, head)); // Output: 0

    // Delete a value from the list
    // Delete(2, &head);
    // printf("Is 2 in the list after deletion? %d\n", Member(2, head)); // Output: 0

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
