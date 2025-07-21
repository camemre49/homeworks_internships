public class ScribesMonitor implements Runnable{
    private int SCRIBE_NUMBER;
    private static Pen PEN_OBJECT;

    private static Bottle BOTTLE_OBJECT;

    public static void main(String[] args) {
        int scribes = Integer.parseInt(args[0]);
        int pens = Integer.parseInt(args[1]);
        int bottles = Integer.parseInt(args[2]);

        PEN_OBJECT = new Pen(pens);
        BOTTLE_OBJECT = new Bottle(bottles);

        for(int i = 0; i < scribes; i++) {
            new Thread(new ScribesMonitor(i+1)).start();
        }
    }


    public ScribesMonitor(int id) {
        this.SCRIBE_NUMBER = id;
    }


    @Override
    public void run() {
        while(true) {
            try {
                BOTTLE_OBJECT.takeBottle(SCRIBE_NUMBER);
                if(!PEN_OBJECT.takePen(SCRIBE_NUMBER)) {
                    BOTTLE_OBJECT.putBottle(SCRIBE_NUMBER);
                    continue;
                }
                Actions.write(SCRIBE_NUMBER);
                PEN_OBJECT.putPen(SCRIBE_NUMBER);
                BOTTLE_OBJECT.putBottle(SCRIBE_NUMBER);

            }
            catch (Exception ignored){}
        }
    }


    private static class Bottle {
        private static int BOTTLE_COUNT;

        public Bottle(int bottleCount){
            BOTTLE_COUNT = bottleCount;
        }

        public synchronized void takeBottle(int id) throws InterruptedException {
            while(BOTTLE_COUNT == 0) {
                wait();
            }

            Actions.takeBottle(id);
            BOTTLE_COUNT--;
        }

        public synchronized void putBottle(int id) {
            BOTTLE_COUNT++;
            Actions.putBottle(id);
            notify();
        }

    }

    private static class Pen {
        private static int PEN_COUNT;

        public Pen(int penCount) {
            PEN_COUNT = penCount;
        }

        public synchronized boolean takePen(int id) {
            if(PEN_COUNT > 0) {
                Actions.takePen(id);
                PEN_COUNT--;
                return true;
            }
            else {
                return false;
            }
        }

        public synchronized void putPen(int id) {
            PEN_COUNT++;
            Actions.putPen(id);
        }

    }

}