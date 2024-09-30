import matplotlib.pyplot as plt

# Function to read the data from the file
def read_data(file_name):
    cases = {}
    
    with open(file_name, 'r') as file:
        for line in file:
            # Extract the relevant fields from each line
            parts = line.strip().split(', ')
            thread_count = int(parts[5].split(': ')[1])  # Extract thread_count
            mean = float(parts[6].split(': ')[1])  # Extract mean value
            m_member = float(parts[2].split(': ')[1])  # Extract m_member as a float
            
            # Use m_member as a key to group data by cases
            if m_member not in cases:
                cases[m_member] = {'threads': [], 'means': []}
            
            cases[m_member]['threads'].append(thread_count)
            cases[m_member]['means'].append(mean)
    
    return cases

# Read the data from the file
file_name = '../results_read_write.txt'  # Replace this with your actual file path
cases = read_data(file_name)

# Create the plot
plt.figure(figsize=(10, 6))

# Plot each case
for m_member, data in cases.items():
    # Convert m_member to percentage and round to two decimal places
    m_member_percent = round((m_member / 10000) * 100)  # Assuming m_member is out of 10000
    plt.plot(data['threads'], data['means'], marker='o', label=f'm_member: {m_member_percent}%')

# Add titles and labels
plt.title('Average Execution Time vs Number of Threads-Read Write Lock')
plt.xlabel('Number of Threads')
plt.ylabel('Average Execution Time (seconds)')
plt.legend()
plt.grid(True)

# Show the plot
plt.show()
