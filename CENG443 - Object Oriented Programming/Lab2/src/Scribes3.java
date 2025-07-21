import java.util.concurrent.ThreadLocalRandom;
import java.util.concurrent.locks.*;

public class Scribes3 {

    public static void main(String[] args){
        int scribes = Integer.parseInt(args[0]);
        int pens = Integer.parseInt(args[1]);
        int bottles = Integer.parseInt(args[2]);

        pen Pens = new pen(pens);
        bottle Bottles = new bottle(bottles);

        for (int i = 1; i <= scribes; i++) {
            Scribe scribe = new Scribe(i, Pens, Bottles);
            new Thread(scribe).start();
        }
    }

    private static class pen {
        int TotalPenNumber;
        int CurrentPenNumber;
        private static Lock PenLock = new ReentrantLock();
        private static Condition CanTakePen = PenLock.newCondition();

        pen(int number) {
            this.TotalPenNumber = number;
            this.CurrentPenNumber = number;
        }

        public boolean getPen(int id) {
            PenLock.lock();
            try {
                if (CurrentPenNumber != 0) {
                    Actions.takePen(id);
                    CurrentPenNumber--;
                    //System.out.println(CurrentPenNumber);
                    return true;
                }
                return false;
            }
            //catch (InterruptedException ex) {
            //  ex.printStackTrace();
            //}
            finally {
                PenLock.unlock();
            }
        }

        public void dropPen(int id) {
            PenLock.lock();
            try {
                Actions.putPen(id);
                CanTakePen.signalAll();
                CurrentPenNumber++;
                // System.out.println(CurrentPenNumber);
            }
            finally {
                PenLock.unlock();
            }
        }
    }

    private static class bottle {
        int TotalBottleNumber;
        int CurrentBottleNumber;
        private static Lock BottleLock = new ReentrantLock();
        private static Condition CanTakeBottle = BottleLock.newCondition();

        bottle(int number) {
            this.TotalBottleNumber = number;
            this.CurrentBottleNumber = number;
        }

        public void getBottle(int id) {
            BottleLock.lock();
            try {
                while (CurrentBottleNumber == 0) {
                    CanTakeBottle.await();
                }
                Actions.takeBottle(id);
                CurrentBottleNumber--;
                // System.out.println(CurrentBottleNumber);
            }
            catch (InterruptedException ex) {
                ex.printStackTrace();
            }
            finally {
                BottleLock.unlock();
            }
        }

        public void dropBottle(int id) {
            BottleLock.lock();
            try {
                Actions.putBottle(id);
                CanTakeBottle.signalAll();
                CurrentBottleNumber++;
                // System.out.println(CurrentBottleNumber);
            }
            finally {
                BottleLock.unlock();
            }
        }
    }

    private static class Scribe implements Runnable{
        boolean HavePen = false;
        boolean HaveBottle = false;
        int id;
        int numberofwriting;
        pen Pens;
        bottle Bottles;

        Scribe(int id, pen Pens, bottle Bottles) {
            this.id = id;
            this.Pens = Pens;
            this.Bottles = Bottles;
        }

        @Override
        public void run(){
            while (true) {

                if (!HaveBottle) {
                    Bottles.getBottle(id);
                    HaveBottle = true;
                }
                if (!HavePen) {
                    HavePen = Pens.getPen(id);
                    if (!HavePen) {
                        Bottles.dropBottle(id);
                    }
                }
                if (HavePen && HaveBottle) {
                    Actions.write(id);
                    Pens.dropPen(id);
                    HavePen = false;
                    Bottles.dropBottle(id);
                    HaveBottle = false;
                    numberofwriting++;
                    Actions.write(id);
                }
            }
        }
    }
}
