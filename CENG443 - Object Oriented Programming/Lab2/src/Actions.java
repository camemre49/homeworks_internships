import java.util.concurrent.ThreadLocalRandom;
import java.util.concurrent.atomic.AtomicLong;
import java.lang.Thread;

public class Actions {

    private static int WAIT_MIN = 500;
    private static int WAIT_MAX = 1000;
    private static AtomicLong counter = new AtomicLong(0);

    private static void action(String msg) {
        int min_time = WAIT_MIN;
        int max_time = WAIT_MAX;

        long action_no = counter.incrementAndGet();
        assert min_time <= max_time;

        if (max_time > 0) {
            try {
                int wait_time;
                if (min_time == max_time) {
                    wait_time = max_time;
                } else {
                    wait_time = ThreadLocalRandom.current().nextInt(min_time, max_time + 1);
                }
                Thread.sleep(wait_time);
            } catch (InterruptedException e) {
            }
        }
        System.out.println(
                msg + " (on Thread " + Thread.currentThread().getId() + ") (Action number: " + action_no + ")");

    }

    static void takeBottle(int scribe_number) {
        action("Scribe " + scribe_number + " takes a bottle");
    }

    static void putBottle(int scribe_number) {
        action("Scribe " + scribe_number + " puts the bottle back");
    }

    static void takePen(int scribe_number) {
        action("Scribe " + scribe_number + " takes a pen");
    }

    static void putPen(int scribe_number) {
        action("Scribe " + scribe_number + " puts the pen back");
    }

    static void write(int scribe_number) {
        action("Scribe " + scribe_number + " writes a record");
    }
}
