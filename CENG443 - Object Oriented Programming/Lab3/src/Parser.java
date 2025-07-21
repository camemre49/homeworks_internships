import java.util.Arrays;
import java.util.Objects;

public class Parser {
  static public Customer parseCustomer(String line) {
    String[] values = line.split(",", -1);
    Customer customer = new Customer();

    // Parsing name, age, and date
    customer.name = values[Customer.NAME_I];
    customer.age = Integer.parseInt(values[Customer.AGE_I]);
    customer.date = values[Customer.DATE_I];

    // Parsing bread price and quantity
    customer.breadPrice = Objects.equals(values[Customer.P_BREAD_I], "") ?
        0 : Double.parseDouble(values[Customer.P_BREAD_I]);
    customer.breadQuantity = Objects.equals(values[Customer.Q_BREAD_I], "") ?
        0 : Integer.parseInt(values[Customer.Q_BREAD_I]);

    // Parsing milk price and quantity
    customer.milkPrice = Objects.equals(values[Customer.P_MILK_I], "") ?
        0 : Double.parseDouble(values[Customer.P_MILK_I]);
    customer.milkQuantity = Objects.equals(values[Customer.Q_MILK_I], "") ?
        0 : Integer.parseInt(values[Customer.Q_MILK_I]);

    // Parsing egg price and quantity
    customer.eggPrice = Objects.equals(values[Customer.P_EGG_I], "") ?
        0 : Double.parseDouble(values[Customer.P_EGG_I]);
    customer.eggQuantity = Objects.equals(values[Customer.Q_EGG_I], "") ?
        0 : Integer.parseInt(values[Customer.Q_EGG_I]);

    // Parsing potato price and quantity
    customer.potatoPrice = Objects.equals(values[Customer.P_POTATO_I], "") ?
        0 : Double.parseDouble(values[Customer.P_POTATO_I]);
    customer.potatoQuantity = Objects.equals(values[Customer.Q_POTATO_I], "") ?
        0 : Integer.parseInt(values[Customer.Q_POTATO_I]);

    // Parsing tomato price and quantity
    customer.tomatoPrice = Objects.equals(values[Customer.P_TOMATO_I], "") ?
        0 : Double.parseDouble(values[Customer.P_TOMATO_I]);
    customer.tomatoQuantity = Objects.equals(values[Customer.Q_TOMATO_I], "") ?
        0 : Integer.parseInt(values[Customer.Q_TOMATO_I]);

    return customer;
  }
}
