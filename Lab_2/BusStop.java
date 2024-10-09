import java.util.concurrent.Semaphore;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class BusStop {

    private static final int MAX_CAPACITY = 50;
    private int waitingRiders = 0;
    private final Lock lock = new ReentrantLock();  // Mutex for shared variables
    private final Semaphore busSemaphore = new Semaphore(0); // Controls boarding
    private final Semaphore riderSemaphore = new Semaphore(0); // Controls riders
    private final Semaphore allAboardSemaphore = new Semaphore(0); // To signal when bus is full

    public void riderArrives() throws InterruptedException {
        lock.lock();  // Critical section
        try {
            waitingRiders++;
            System.out.println("Rider arrives. Waiting riders: " + waitingRiders);
        } finally {
            lock.unlock();
        }

        busSemaphore.acquire();  // Wait for a bus to arrive
        System.out.println("Rider boards the bus.");
        riderSemaphore.release();  // Signal that rider has boarded
        allAboardSemaphore.acquire();  // Wait for the bus to signal departure
    }

    public void busArrives() throws InterruptedException {
        lock.lock();  // Critical section
        try {
            if (waitingRiders == 0) {
                System.out.println("No riders. Bus departs immediately.");
                return;  // No riders, bus leaves
            }
            System.out.println("Bus arrives. Riders boarding.");
            int boardingRiders = Math.min(waitingRiders, MAX_CAPACITY);  // Limit to bus capacity
            for (int i = 0; i < boardingRiders; i++) {
                busSemaphore.release();  // Allow riders to board
                riderSemaphore.acquire();  // Wait for each rider to board
            }
            waitingRiders -= boardingRiders;  // Update the number of waiting riders
        } finally {
            lock.unlock();
        }

        System.out.println("Bus departs with " + Math.min(waitingRiders, MAX_CAPACITY) + " riders.");
        allAboardSemaphore.release(MAX_CAPACITY);  // Signal that the bus is full and departing
    }

    public static void main(String[] args) {
        BusStop busStop = new BusStop();

        // Simulating riders arriving at different times
        for (int i = 0; i < 100; i++) {
            new Thread(() -> {
                try {
                    busStop.riderArrives();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }).start();
        }

        // Simulating buses arriving at different times
        for (int i = 0; i < 10; i++) {
            new Thread(() -> {
                try {
                    Thread.sleep(2000);  // Simulate bus inter-arrival time
                    busStop.busArrives();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }).start();
        }
    }
}
