import matplotlib.pyplot as plt

# Function to read the data from the file
def read_data(file_name):
    cases = {}
    
    with open(file_name, 'r') as file:
        for line in file:
            # Extract the relevant fields from each line
            parts = line.strip().split(',')
            thread_count = int(parts[6].split(': ')[1])  # Extract thread_count
            mean = float(parts[7].split(': ')[1])  # Extract mean value
            m_member = parts[3].split(': ')[1]  # Extract m_member
            m = int(parts[1].split(': ')[1]) # Extract m value

            m_member = round(int(m_member)/m, 2)  # Round m_member to 2 decimal places
            
            # Use m_member as a key to group data by cases
            if m_member not in cases:
                cases[m_member] = {'threads': [], 'means': []}
            
            cases[m_member]['threads'].append(thread_count)
            cases[m_member]['means'].append(mean)
    
    return cases

# Read the data from the file
file_name = '../results_mutex.txt'
cases = read_data(file_name)

# Create the plot
plt.figure(figsize=(10, 6))

# Plot each case
for m_member, data in cases.items():
    plt.plot(data['threads'], data['means'], marker='o', label=f'm_member: {m_member}%')

# Add titles and labels
plt.title('Average Execution Time vs Number of Threads-Mutex')
plt.xlabel('Number of Threads')
plt.ylabel('Average Execution Time (seconds)')
plt.legend()
plt.grid(True)

# Show the plot
plt.show()