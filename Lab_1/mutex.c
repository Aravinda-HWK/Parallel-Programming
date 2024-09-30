#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

// Define the structure for a list node
struct list_node_s
{
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
typedef struct
{
    struct list_node_s **head_pp;
    int start;
    int end;
    double member_ratio;
    double insert_ratio;
    double delete_ratio;
} thread_data_t;

// Function prototypes
void print_list(struct list_node_s *head);
int Member(int value, struct list_node_s *head_p);
int Insert(int value, struct list_node_s **head_pp);
int Delete(int value, struct list_node_s **head_pp);
void *ThreadFunction(void *arg);

// Function to check if a value is a member of the linked list
int Member(int value, struct list_node_s *head_p)
{

    struct list_node_s *curr_p = head_p;

    // Traverse the list to find the value
    while (curr_p != NULL && curr_p->data < value)
        curr_p = curr_p->next;

    int found = (curr_p != NULL && curr_p->data == value);

    return found;
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

// Function executed by each thread
void *ThreadFunction(void *arg)
{
    thread_data_t *data = (thread_data_t *)arg;
    struct list_node_s **head_pp = data->head_pp;
    int start = data->start;
    int end = data->end;
    double member_ratio = data->member_ratio;
    double insert_ratio = data->insert_ratio;
    double delete_ratio = data->delete_ratio;

    int total_ops = end - start;
    int member_ops = total_ops * member_ratio;
    int insert_ops = total_ops * insert_ratio;
    int delete_ops = total_ops * delete_ratio;

    char *operations = malloc(total_ops * sizeof(char));
    for (int i = 0; i < member_ops; i++)
        operations[i] = 'M';
    for (int i = 0; i < insert_ops; i++)
        operations[member_ops + i] = 'I';
    for (int i = 0; i < delete_ops; i++)
        operations[member_ops + insert_ops + i] = 'D';

    // Shuffle the operations array
    for (int i = total_ops - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        char temp = operations[i];
        operations[i] = operations[j];
        operations[j] = temp;
    }

    for (int i = start, j = 0; i < end; i++, j++)
    {
        switch (operations[j])
        {
        case 'M':
            pthread_mutex_lock(&list_mutex);
            Member(opr_values[i], *head_pp);
            pthread_mutex_unlock(&list_mutex);
            break;
        case 'I':
            pthread_mutex_lock(&list_mutex);
            Insert(opr_values[i], head_pp);
            pthread_mutex_unlock(&list_mutex);
            break;
        case 'D':
            pthread_mutex_lock(&list_mutex);
            Delete(opr_values[i], head_pp);
            pthread_mutex_unlock(&list_mutex);
            break;
        }
    }

    free(operations);
    return NULL;
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

    thread_count = strtol(argv[1], NULL, 10);
    n = 1000;
    m = 10000;
    double member_ratio = atof(argv[2]);
    double insert_ratio = atof(argv[3]);
    double delete_ratio = atof(argv[4]);

    m_member = m * member_ratio;
    m_insert = m * insert_ratio;
    m_delete = m * delete_ratio;
    printf("%d %d %d %d %d\n", n, m, m_member, m_insert, m_delete);

    opr_values = malloc(m * sizeof(int));
    int i, ins_value;

    // Create linked list head pointer
    struct list_node_s *head = NULL;

    srand(time(NULL)); // Set different random seed for each execution

    for (i = 0; i < n; i++)
    {
        ins_value = rand() % 65535; // value should be between 2^16 - 1
        Insert(ins_value, &head);
    }
    for (i = 0; i < m; i++)
    {
        opr_values[i] = rand() % 65535; // value should be between 2^16 - 1
    }

    // Initialize mutex
    pthread_mutex_init(&list_mutex, NULL);

    int initial_runs = 100;
    double execution_times[initial_runs];

    // Perform initial runs to estimate mean and standard deviation
    for (int run = 0; run < initial_runs; run++)
    {
        pthread_t *thread_handles = malloc(thread_count * sizeof(pthread_t));
        thread_data_t *thread_data_array = malloc(thread_count * sizeof(thread_data_t));

        int ops_per_thread = m / thread_count;

        clock_t start, end;
        start = clock();
        for (long thread = 0; thread < thread_count; thread++)
        {
            thread_data_array[thread].head_pp = &head;
            thread_data_array[thread].start = thread * ops_per_thread;
            thread_data_array[thread].end = (thread + 1) * ops_per_thread;
            if (thread == thread_count - 1)
            {
                thread_data_array[thread].end = m;
            }
            thread_data_array[thread].member_ratio = member_ratio;
            thread_data_array[thread].insert_ratio = insert_ratio;
            thread_data_array[thread].delete_ratio = delete_ratio;
            pthread_create(&thread_handles[thread], NULL, ThreadFunction, (void *)&thread_data_array[thread]);
        }
        for (long thread = 0; thread < thread_count; thread++)
        {
            pthread_join(thread_handles[thread], NULL);
        }
        end = clock();
        execution_times[run] = ((double)(end - start)) * 1000000 / CLOCKS_PER_SEC;

        free(thread_handles);
        free(thread_data_array);
    }

    // Calculate mean and standard deviation
    double mean, std_dev, ci_lower, ci_upper;
    CalculateStats(execution_times, initial_runs, &mean, &std_dev, &ci_lower, &ci_upper);

    // Determine the required number of samples
    double precision = 0.05 * mean; // 5% precision
    int required_samples = ceil((1.96 * std_dev / precision) * (1.96 * std_dev / precision));

    printf("Initial Mean: %f, Initial Std Dev: %f\n", mean, std_dev);
    printf("Required Samples: %d\n", required_samples);

    // Re-run with required sample count
    double *final_times = malloc(required_samples * sizeof(double));

    for (int run = 0; run < required_samples; run++)
    {
        pthread_t *thread_handles = malloc(thread_count * sizeof(pthread_t));
        thread_data_t *thread_data_array = malloc(thread_count * sizeof(thread_data_t));

        int ops_per_thread = m / thread_count;

        clock_t start, end;
        start = clock();
        for (long thread = 0; thread < thread_count; thread++)
        {
            thread_data_array[thread].head_pp = &head;
            thread_data_array[thread].start = thread * ops_per_thread;
            thread_data_array[thread].end = (thread + 1) * ops_per_thread;
            if (thread == thread_count - 1)
            {
                thread_data_array[thread].end = m;
            }
            thread_data_array[thread].member_ratio = member_ratio;
            thread_data_array[thread].insert_ratio = insert_ratio;
            thread_data_array[thread].delete_ratio = delete_ratio;
            pthread_create(&thread_handles[thread], NULL, ThreadFunction, (void *)&thread_data_array[thread]);
        }
        for (long thread = 0; thread < thread_count; thread++)
        {
            pthread_join(thread_handles[thread], NULL);
        }
        end = clock();
        final_times[run] = ((double)(end - start)) * 1000000 / CLOCKS_PER_SEC;

        free(thread_handles);
        free(thread_data_array);
    }

    // Calculate final statistics
    CalculateStats(final_times, required_samples, &mean, &std_dev, &ci_lower, &ci_upper);

    printf("Final Mean: %f, Final Std Dev: %f\n", mean, std_dev);
    printf("95%% Confidence Interval: [%f, %f]\n", ci_lower, ci_upper);

    // Save the results to a file
    FILE *file = fopen("results/results_mutex.txt", "a");
    if (file == NULL)
    {
        fprintf(stderr, "Error opening file for writing\n");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "n: %d, m: %d,type: %s, m_member: %d, m_insert: %d, m_delete: %d, thread_count: %d, mean: %f, std_dev: %f, ci_lower: %f, ci_upper: %f\n",
            n, m, "Read Write Lock", m_member, m_insert, m_delete, thread_count, mean, std_dev, ci_lower, ci_upper);
    fclose(file);

    // Destroy mutex
    pthread_mutex_destroy(&list_mutex);
    free(final_times);

    // Free the allocated memory
    struct list_node_s *current = head;
    while (current != NULL)
    {
        struct list_node_s *temp = current;
        current = current->next;
        free(temp);
    }
    free(opr_values);

    return 0;
}
