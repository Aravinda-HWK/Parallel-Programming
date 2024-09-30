#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Define the structure for a list node
struct list_node_s
{
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
void ShuffleOperations(char *operations, int total_ops);

// Function to check if a value is a member of the linked list
int Member(int value, struct list_node_s *head_p)
{
    struct list_node_s *curr_p = head_p;

    // Traverse the list to find the value
    while (curr_p != NULL && curr_p->data < value)
        curr_p = curr_p->next;

    return (curr_p != NULL && curr_p->data == value);
}

// Function to insert a new node into the list
int Insert(int value, struct list_node_s **head_pp)
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

// Function to delete a node from the list
int Delete(int value, struct list_node_s **head_pp)
{
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

// Function to shuffle the operations array
void ShuffleOperations(char *operations, int total_ops)
{
    for (int i = total_ops - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        char temp = operations[i];
        operations[i] = operations[j];
        operations[j] = temp;
    }
}

// Function to execute the operations sequentially with randomness
void ExecuteOperations(struct list_node_s **head_pp)
{
    char *operations = malloc(m * sizeof(char));

    // Fill operations array
    for (int i = 0; i < m_member; i++)
        operations[i] = 'M';
    for (int i = 0; i < m_insert; i++)
        operations[m_member + i] = 'I';
    for (int i = 0; i < m_delete; i++)
        operations[m_member + m_insert + i] = 'D';

    // Shuffle operations array
    ShuffleOperations(operations, m);

    for (int i = 0; i < m; i++)
    {
        switch (operations[i])
        {
        case 'M':
            Member(opr_values[i], *head_pp);
            break;
        case 'I':
            Insert(opr_values[i], head_pp);
            break;
        case 'D':
            Delete(opr_values[i], head_pp);
            break;
        }
    }

    free(operations);
}

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

// Function to calculate mean, standard deviation, and confidence interval
void CalculateStats(double *times, int sample_count, double *mean, double *std_dev, double *ci_lower, double *ci_upper)
{
    double sum = 0.0, sum_sq = 0.0;

    for (int i = 0; i < sample_count; i++)
    {
        sum += times[i];
        sum_sq += times[i] * times[i];
    }

    *mean = sum / sample_count;
    *std_dev = sqrt((sum_sq - (sum * sum) / sample_count) / (sample_count - 1));

    // 95% confidence interval
    double z = 1.96; // Z-value for 95% confidence
    double margin_of_error = z * (*std_dev) / sqrt(sample_count);
    *ci_lower = *mean - margin_of_error;
    *ci_upper = *mean + margin_of_error;
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        fprintf(stderr, "Usage: %s <thread_count> <m_member_ratio> <m_insert_ratio> <m_delete_ratio>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    thread_count = 1;
    n = 1000;
    m = 10000;
    m_member = m * atof(argv[2]);
    m_insert = m * atof(argv[3]);
    m_delete = m * atof(argv[4]);
    printf("%d %d %d %d %d\n", n, m, m_member, m_insert, m_delete);

    opr_values = malloc(m * sizeof(int));

    double execution_times[100];
    int initial_runs = 100;

    srand(time(NULL)); // Set different random seed for each execution

    // Initial estimation of standard deviation
    for (int run = 0; run < initial_runs; run++)
    {
        struct list_node_s *head = NULL;
        for (int i = 0; i < n; i++)
        {
            int insert_boolean = Insert(rand() % 65535, &head);
            if (insert_boolean == 0)
            {
                i--;
            }
        }
        for (int i = 0; i < m; i++)
        {
            opr_values[i] = rand() % 65535;
        }

        clock_t start = clock();
        ExecuteOperations(&head);
        clock_t end = clock();

        execution_times[run] = ((double)(end - start)) * 1000000 / CLOCKS_PER_SEC;

        // Free the allocated memory
        struct list_node_s *current = head;
        struct list_node_s *next;
        while (current != NULL)
        {
            next = current->next;
            free(current);
            current = next;
        }
    }

    // Calculate mean and standard deviation from initial runs
    double mean, std_dev, ci_lower, ci_upper;
    CalculateStats(execution_times, initial_runs, &mean, &std_dev, &ci_lower, &ci_upper);

    // Determine required number of samples
    double precision = 0.05 * mean; // 5% precision
    int required_samples = ceil((1.96 * std_dev / precision) * (1.96 * std_dev / precision));

    printf("Initial Mean: %f, Initial Std Dev: %f\n", mean, std_dev);
    printf("Required Samples: %d\n", required_samples);

    double *final_times = malloc(required_samples * sizeof(double));

    for (int run = 0; run < required_samples; run++)
    {
        struct list_node_s *head = NULL;
        for (int i = 0; i < n; i++)
        {
            int insert_boolean = Insert(rand() % 65535, &head);
            if (insert_boolean == 0)
            {
                i--;
            }
        }
        for (int i = 0; i < m; i++)
        {
            opr_values[i] = rand() % 65535;
        }

        clock_t start = clock();
        ExecuteOperations(&head);
        clock_t end = clock();

        final_times[run] = ((double)(end - start)) * 1000000 / CLOCKS_PER_SEC;

        // Free the allocated memory
        struct list_node_s *current = head;
        struct list_node_s *next;
        while (current != NULL)
        {
            next = current->next;
            free(current);
            current = next;
        }
    }

    // Calculate final stats
    CalculateStats(final_times, required_samples, &mean, &std_dev, &ci_lower, &ci_upper);

    printf("Final Mean: %f, Final Std Dev: %f\n", mean, std_dev);
    printf("95%% Confidence Interval: [%f, %f]\n", ci_lower, ci_upper);

    // Save the results to a file
    FILE *file = fopen("results/results_serial.txt", "a");
    if (file == NULL)
    {
        fprintf(stderr, "Error opening file for writing\n");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "n: %d, m: %d, m_member: %d, m_insert: %d, m_delete: %d, thread_count: %d, mean: %f, std_dev: %f, ci_lower: %f, ci_upper: %f\n",
            n, m, m_member, m_insert, m_delete, thread_count, mean, std_dev, ci_lower, ci_upper);
    fclose(file);

    free(final_times);
    free(opr_values);

    return 0;
}
