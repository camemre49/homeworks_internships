import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

public class Scribes implements Runnable {

    private int scribeNumber = 0;
    private static int SCRIBE_COUNT = 0;
    private static int PEN_COUNT = 0;
    private static ReentrantLock penLock = new ReentrantLock(true);
    private static int BOTTLE_COUNT = 0;
    private static ReentrantLock bottleLock = new ReentrantLock(true);
    private static Condition bottleAvailable = bottleLock.newCondition();

    public static void main(String[] args) {
        Scribes.SCRIBE_COUNT = Integer.parseInt(args[0]);
        Scribes.PEN_COUNT = Integer.parseInt(args[1]);
        Scribes.BOTTLE_COUNT = Integer.parseInt(args[2]);

        for (int i = 0; i < SCRIBE_COUNT; i++) {
            Scribes scribe = new Scribes(i + 1);
            Thread thread = new Thread(scribe);
            thread.start();
        }
    }

    public Scribes(int scribeNumber) {
        this.scribeNumber = scribeNumber;
    }

    @Override
    public void run() {
        while (true) {
            this.takeBottle();
            boolean penTaken = this.takePen();
            if(penTaken) {
                this.write();
                this.putPen();
                this.putBottle();
            }
        }
    }

    public void takeBottle() {
        bottleLock.lock();
        try {
            while (BOTTLE_COUNT == 0) {
                bottleAvailable.await(); // Wait until a bottle is available
            }
            BOTTLE_COUNT--;
            //System.out.println((BOTTLE_COUNT + 1) + "->" + BOTTLE_COUNT);
            Actions.takeBottle(scribeNumber);
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt(); // Restore interrupt status
        } finally {
            bottleLock.unlock();
        }
    }

    public boolean takePen() {
        penLock.lock();
        if(PEN_COUNT == 0) {
            this.putBottle();
            penLock.unlock();
            return false;
        }
        PEN_COUNT--;
        //System.out.println((PEN_COUNT + 1) + "->" + PEN_COUNT);
        Actions.takePen(scribeNumber);
        penLock.unlock();
        return true;
    }

    public void write() {
        Actions.write(scribeNumber);
    }

    public void putPen() {
        penLock.lock();
        try {
            PEN_COUNT++;
            //System.out.println((PEN_COUNT - 1) + "->" + PEN_COUNT);
            Actions.putPen(scribeNumber);
        } finally {
            penLock.unlock();
        }
    }

    public void putBottle() {
        bottleLock.lock();
        try {
            BOTTLE_COUNT++;
            //System.out.println((BOTTLE_COUNT - 1) + "->" + BOTTLE_COUNT);
            Actions.putBottle(scribeNumber);
            bottleAvailable.signal(); // Notify one waiting thread
        } finally {
            bottleLock.unlock();
        }
    }
}
