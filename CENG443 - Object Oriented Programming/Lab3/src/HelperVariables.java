public class HelperVariables {
  static String[] products = {"Bread", "Milk", "Egg", "Potatoes", "Tomatoes"};
  static int TOTAL_BREAD_Q = 0;
  static int TOTAL_BREAD_AGE = 0;
  static int BREAD_Q_TIMES_AGE = 0;
  static int TOTAL_MILK_Q = 0;
  static int TOTAL_MILK_AGE = 0;
  static int MILK_Q_TIMES_AGE = 0;
  static int TOTAL_EGG_Q = 0;
  static int TOTAL_EGG_AGE = 0;
  static int EGG_Q_TIMES_AGE = 0;
  static int TOTAL_POTATO_Q = 0;
  static int TOTAL_POTATO_AGE = 0;
  static int POTATO_Q_TIMES_AGE = 0;
  static int TOTAL_TOMATO_Q = 0;
  static int TOTAL_TOMATO_AGE = 0;
  static int TOMATO_Q_TIMES_AGE = 0;


  static public void clearQuantities() {
    HelperVariables.TOTAL_BREAD_Q = 0;
    HelperVariables.TOTAL_MILK_Q = 0;
    HelperVariables.TOTAL_EGG_Q = 0;
    HelperVariables.TOTAL_POTATO_Q = 0;
    HelperVariables.TOTAL_TOMATO_Q = 0;
    HelperVariables.TOTAL_BREAD_AGE = 0;
    HelperVariables.TOTAL_MILK_AGE = 0;
    HelperVariables.TOTAL_EGG_AGE = 0;
    HelperVariables.TOTAL_POTATO_AGE = 0;
    HelperVariables.TOTAL_TOMATO_AGE = 0;
    HelperVariables.BREAD_Q_TIMES_AGE = 0;
    HelperVariables.MILK_Q_TIMES_AGE = 0;
    HelperVariables.EGG_Q_TIMES_AGE = 0;
    HelperVariables.POTATO_Q_TIMES_AGE = 0;
    HelperVariables.TOMATO_Q_TIMES_AGE = 0;
  }

  static void printMin(double... averages) {
    int minIndex = 0;
    for (int i = 1; i < averages.length; i++) {
      if (averages[i] < averages[minIndex]) {
        minIndex = i;
      }
    }
    System.out.println(HelperVariables.products[minIndex]);
  }
}
