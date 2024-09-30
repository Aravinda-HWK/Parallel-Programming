import matplotlib.pyplot as plt
import numpy as np

# Extract means from files


def extract_means_from_file(file_name):
    means_case_1 = []
    means_case_2 = []
    means_case_3 = []

    with open(file_name, 'r') as file:
        for line in file:
            if "m_member: 9899" in line:  # Case 1 (mMember = 0.99)
                mean_value = float(line.split("mean: ")[1].split(",")[0])
                means_case_1.append(mean_value)
            elif "m_member: 9000" in line:  # Case 2 (mMember = 0.90)
                mean_value = float(line.split("mean: ")[1].split(",")[0])
                means_case_2.append(mean_value)
            elif "m_member: 5000" in line:  # Case 3 (mMember = 0.50)
                mean_value = float(line.split("mean: ")[1].split(",")[0])
                means_case_3.append(mean_value)

    return means_case_1, means_case_2, means_case_3


# Read the data from each file
serial_means_case_1, serial_means_case_2, serial_means_case_3 = extract_means_from_file(
    'Lab_1/results_serial.txt')
mutex_means_case_1, mutex_means_case_2, mutex_means_case_3 = extract_means_from_file(
    'Lab_1/results_mutex.txt')
readwrite_means_case_1, readwrite_means_case_2, readwrite_means_case_3 = extract_means_from_file(
    'Lab_1/results_read_write.txt')

# Threads array (for mutex and readwrite, serial will use a constant mean value)
threads = [1, 2, 4, 8, 16]
bar_width = 0.25

# Function to plot the bar graphs for each case


def plot_case(case_no, title, serial_mean, mutex_means, readwrite_means):
    # Positions for the bars on the x-axis
    r1 = np.arange(len(threads))  # Mutex bars
    r2 = [x + bar_width for x in r1]  # Read-Write bars

    plt.figure(figsize=(10, 6))

    # Serial: Plot a single bar (as it's constant) only for the first thread
    plt.bar([r1[0] - bar_width], [serial_mean],
            width=bar_width, label='Serial')

    # Mutex: Plot the bars for mutex across the thread counts
    plt.bar(r1, mutex_means, width=bar_width, label='Mutex')

    # Read-Write Lock: Plot the bars for read-write lock across the thread counts
    plt.bar(r2, readwrite_means, width=bar_width, label='Read-Write Lock')

    # Add labels and title
    plt.title(f'Case {case_no}: {title}')
    plt.xlabel('Number of Threads')
    plt.ylabel('Average Execution Time')
    plt.xticks([r + bar_width / 2 for r in r1], threads)

    # Add legend
    plt.legend()

    # Display the graph
    plt.show()


# Plot the graphs for each case
plot_case(1, "mMember = 0.99, mInsert = 0.005, mDelete = 0.005",
          serial_means_case_1[0], mutex_means_case_1, readwrite_means_case_1)
plot_case(2, "mMember = 0.9, mInsert = 0.05, mDelete = 0.05",
          serial_means_case_2[0], mutex_means_case_2, readwrite_means_case_2)
plot_case(3, "mMember = 0.5, mInsert = 0.25, mDelete = 0.25",
          serial_means_case_3[0], mutex_means_case_3, readwrite_means_case_3)
