import java.util.Random;
import java.util.concurrent.Semaphore;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class BusStop {

    private static final int MAX_CAPACITY = 50;
    private int waitingRiders = 0;
    private final Lock lock = new ReentrantLock(); // Mutex for shared variables
    private final Semaphore busSemaphore = new Semaphore(0); // Controls boarding
    private final Semaphore riderSemaphore = new Semaphore(0); // Controls riders
    private final Semaphore allAboardSemaphore = new Semaphore(0); // To signal when bus is full
    private final Random random = new Random();

    // Method to generate an exponential delay (in milliseconds)
    private long getExponentialDelay(double mean) {
        return (long) (-mean * Math.log(1 - random.nextDouble()) * 1000);
    }

    public void riderArrives(int riderId) throws InterruptedException {
        lock.lock(); // Critical section
        try {
            waitingRiders++;
            System.out.println("Rider " + riderId + " arrives. Waiting riders: " + waitingRiders);
        } finally {
            lock.unlock();
        }

        busSemaphore.acquire(); // Wait for a bus to arrive
        System.out.println("Rider " + riderId + " boards the bus.");
        riderSemaphore.release(); // Signal that rider has boarded
        allAboardSemaphore.acquire(); // Wait for the bus to signal departure
    }

    public void busArrives(int busId) throws InterruptedException {
        lock.lock(); // Critical section
        int boardingRiders = 0;
        try {
            if (waitingRiders == 0) {
                System.out.println("No riders. Bus departs immediately.");
                return; // No riders, bus leaves
            }
            System.out.println("Bus " + busId + " arrives. Riders boarding.");
            boardingRiders = Math.min(waitingRiders, MAX_CAPACITY); // Limit to bus capacity
            for (int i = 0; i < boardingRiders; i++) {
                busSemaphore.release(); // Allow riders to board
                riderSemaphore.acquire(); // Wait for each rider to board
            }
            waitingRiders -= boardingRiders; // Update the number of waiting riders
        } finally {
            lock.unlock();
        }

        System.out.println("Bus departs with " + Math.min(boardingRiders, MAX_CAPACITY) + " riders.");
        allAboardSemaphore.release(boardingRiders); // Signal that the bus is full and departing
    }

    public static void main(String[] args) {
        BusStop busStop = new BusStop();

        // Simulate riders arriving continuously
        new Thread(() -> {
            for (int i = 0; i < 80; i++) {
                int riderId = i + 1;
                new Thread(() -> {
                    try {
                        busStop.riderArrives(riderId);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }).start();

                // Generate an exponential delay with a mean of 30 seconds
                try {
                    Thread.sleep(busStop.getExponentialDelay(0.05));
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }).start();

        // Simulate buses arriving continuously
        new Thread(() -> {
            for (int i = 0; i < 10; i++) {
                int busId = i + 1;
                new Thread(() -> {
                    try {
                        busStop.busArrives(busId);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }).start();

                // Generate an exponential delay with a mean of 20 minutes (1200 seconds)
                try {
                    Thread.sleep(busStop.getExponentialDelay(2));
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }).start();

    }
}
