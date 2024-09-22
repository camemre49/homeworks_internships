public class Mthread extends Thread {
    private int number;

    public void run() {
        int a = 5;
        while (a > 0) {
            if (number == 1) {
                System.out.println("A");
                a -= 5;
            } else if (number == 2) {
                System.out.println("B");
                a -= 2;
            } else {
                System.out.println("C");
                a--;
            }
        }
    }

    public Mthread(int n) {
        number = n;
    }


    public static void main(String args[]) {
        Mthread t1 = new Mthread(1);
        t1.start();
        Mthread t2 = new Mthread(2);
        Mthread t3 = new Mthread(3);
        t2.start();
        t3.start();
        try
        {
            sleep(10);
        }
        catch (Exception expn)
        {

        }

        System.out.println(t1.getName());
    }
}