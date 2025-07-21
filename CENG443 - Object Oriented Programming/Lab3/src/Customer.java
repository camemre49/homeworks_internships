public class Customer {
  static final int NAME_I = 0;
  static final int AGE_I = 1;
  static final int DATE_I = 2;
  static final int P_BREAD_I = 3;
  static final int Q_BREAD_I = 4;
  static final int P_MILK_I = 5;
  static final int Q_MILK_I = 6;
  static final int P_EGG_I = 7;
  static final int Q_EGG_I = 8;
  static final int P_POTATO_I = 9;
  static final int Q_POTATO_I = 10;
  static final int P_TOMATO_I = 11;
  static final int Q_TOMATO_I = 12;


  String name;
  int age;
  String date;
  double breadPrice;
  double milkPrice;
  double eggPrice;
  double potatoPrice;
  double tomatoPrice;
  int breadQuantity;
  int milkQuantity;
  int eggQuantity;
  int potatoQuantity;
  int tomatoQuantity;

  public double findMaxPrice() {
    return Double.max(breadPrice, Double.max(eggPrice, Double.max(milkPrice, Double.max(potatoPrice, tomatoPrice))));
  }

  public double findTotalPayment() {
    return
        breadQuantity * breadPrice +
            eggQuantity * eggPrice +
            milkQuantity * milkPrice +
            potatoQuantity * potatoPrice +
            tomatoQuantity * tomatoPrice;
  }

  public boolean isBeforeYear(int year, String date) {
    String[] dateParts = date.split("-");
    return Integer.parseInt(dateParts[0]) - year < 0;
  }
}
