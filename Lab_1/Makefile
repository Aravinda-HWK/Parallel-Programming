CC = gcc
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -lpthread  # Ensure XOPEN_SOURCE is defined

# Target executable names
TARGETS = serial_ll mutex read_write

# Create individual targets for each program
all: $(TARGETS)

serial_ll: serial_ll.c
	$(CC) $(CFLAGS) -o serial_ll serial_ll.c -lm

mutex: mutex.c
	$(CC) $(CFLAGS) -o mutex mutex.c -lm

read_write: read_write.c
	$(CC) $(CFLAGS) -o read_write read_write.c -lm

# Run the programs with the specified arguments
run_serial_ll:
	./serial_ll 1 0.99 0.005 0.005
	./serial_ll 1 0.9 0.05 0.05
	./serial_ll 1 0.5 0.25 0.25

run_mutex:
	./mutex 1 0.99 0.005 0.005
	./mutex 1 0.9 0.05 0.05
	./mutex 1 0.5 0.25 0.25
	./mutex 2 0.99 0.005 0.005
	./mutex 2 0.9 0.05 0.05
	./mutex 2 0.5 0.25 0.25
	./mutex 4 0.99 0.005 0.005
	./mutex 4 0.9 0.05 0.05
	./mutex 4 0.5 0.25 0.25
	./mutex 8 0.99 0.005 0.005
	./mutex 8 0.9 0.05 0.05
	./mutex 8 0.5 0.25 0.25

run_read_write:
	./read_write 1 0.99 0.005 0.005
	./read_write 1 0.9 0.05 0.05
	./read_write 1 0.5 0.25 0.25
	./read_write 2 0.99 0.005 0.005
	./read_write 2 0.9 0.05 0.05
	./read_write 2 0.5 0.25 0.25
	./read_write 4 0.99 0.005 0.005
	./read_write 4 0.9 0.05 0.05
	./read_write 4 0.5 0.25 0.25
	./read_write 8 0.99 0.005 0.005
	./read_write 8 0.9 0.05 0.05
	./read_write 8 0.5 0.25 0.25

# Clean up compiled files
clean:
	rm -f $(TARGETS)