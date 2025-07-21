import java.io.File;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Map;

public class Main {

  public static void main(String[] args) {

    // Validate arguments
    if (args.length < 2) {
      System.out.println("Usage: java CSVProcessor <csv_file_name> <question_no>");
      return;
    }

    String csvFileName = args[0];
    String questionNo = args[1];

    try{
      switch (questionNo) {
        case "1":
          int totalQuantity =
              Files.lines(Paths.get(csvFileName))
                  .skip(1)
                  .map(Parser::parseCustomer)
                  .filter(customer -> customer.name.startsWith("A") || customer.name.startsWith("a"))
                  .map(customer -> customer.breadQuantity + customer.milkQuantity + customer.eggQuantity + customer.potatoQuantity + customer.tomatoQuantity)
                  .reduce(Integer::sum)
                  .get();
          System.out.println(totalQuantity);

        case "2":
          double maxP =
              Files.lines(Paths.get(csvFileName))
                  .skip(1)
                  .map(Parser::parseCustomer)
                  .map(Customer::findMaxPrice)
                  .max(Double::compare)
                  .get();
          System.out.println(maxP);

        case "3":
          String maxDate =
              Files.lines(Paths.get(csvFileName))
                  .skip(1)
                  .map(Parser::parseCustomer)
                  .map(customer -> new String[]{customer.date, Double.toString(customer.findTotalPayment())})
                  .max((x,y) -> (int) (Double.parseDouble(x[1]) - Double.parseDouble(y[1])))
                  .get()[0];
          System.out.println(maxDate);

        case "4":
          HelperVariables.clearQuantities();
          Files.lines(Paths.get(csvFileName))
              .skip(1)
              .map(Parser::parseCustomer)
              .filter(customer -> customer.isBeforeYear(2000, customer.date))
              .forEach(customer -> {
                HelperVariables.TOTAL_BREAD_Q += customer.breadQuantity;
                HelperVariables.TOTAL_MILK_Q += customer.milkQuantity;
                HelperVariables.TOTAL_EGG_Q += customer.eggQuantity;
                HelperVariables.TOTAL_POTATO_Q += customer.potatoQuantity;
                HelperVariables.TOTAL_TOMATO_Q += customer.tomatoQuantity;
              });

          String mostSoldProduct = "Bread";
          int maxQuantity = HelperVariables.TOTAL_BREAD_Q;
          if (HelperVariables.TOTAL_MILK_Q > maxQuantity) {
            mostSoldProduct = "Milk";
            maxQuantity = HelperVariables.TOTAL_MILK_Q;
          }
          if (HelperVariables.TOTAL_EGG_Q > maxQuantity) {
            mostSoldProduct = "Egg";
            maxQuantity = HelperVariables.TOTAL_EGG_Q;
          }
          if (HelperVariables.TOTAL_POTATO_Q > maxQuantity) {
            mostSoldProduct = "Potatoes";
            maxQuantity = HelperVariables.TOTAL_POTATO_Q;
          }
          if (HelperVariables.TOTAL_TOMATO_Q > maxQuantity) {
            mostSoldProduct = "Tomatoes";
            maxQuantity = HelperVariables.TOTAL_TOMATO_Q;
          }
          System.out.println(mostSoldProduct);


        case "5":
          HelperVariables.clearQuantities();
          Files.lines(Paths.get(csvFileName))
              .skip(1)
              .map(Parser::parseCustomer)
              .filter(customer -> !customer.isBeforeYear(2000, customer.date))
              .forEach(customer -> {
                HelperVariables.TOTAL_BREAD_Q += customer.breadQuantity;
                HelperVariables.TOTAL_MILK_Q += customer.milkQuantity;
                HelperVariables.TOTAL_EGG_Q += customer.eggQuantity;
                HelperVariables.TOTAL_POTATO_Q += customer.potatoQuantity;
                HelperVariables.TOTAL_TOMATO_Q += customer.tomatoQuantity;
              });

          String leastSoldProduct = "Bread";
          int leastQuantity = HelperVariables.TOTAL_BREAD_Q;

          if (HelperVariables.TOTAL_MILK_Q < leastQuantity) {
            leastSoldProduct = "Milk";
            leastQuantity = HelperVariables.TOTAL_MILK_Q;
          }
          if (HelperVariables.TOTAL_EGG_Q < leastQuantity) {
            leastSoldProduct = "Egg";
            leastQuantity = HelperVariables.TOTAL_EGG_Q;
          }
          if (HelperVariables.TOTAL_POTATO_Q < leastQuantity) {
            leastSoldProduct = "Potatoes";
            leastQuantity = HelperVariables.TOTAL_POTATO_Q;
          }
          if (HelperVariables.TOTAL_TOMATO_Q < leastQuantity) {
            leastSoldProduct = "Tomatoes";
            leastQuantity = HelperVariables.TOTAL_TOMATO_Q;
          }
          System.out.println(leastSoldProduct);


        case "6":
          Map<String, Integer> ageTimesQuantity = getHashMap();
          Map<String, Integer> quantityOnly = getHashMap();
          Map<String, Double> dobleMap = getHashMap();
          for(String key: dobleMap.keySet()) {
            System.out.println(key + " " + dobleMap.get(key));
          }

          Files.lines(Paths.get(csvFileName))
              .skip(1)
              .map(Parser::parseCustomer)
              .forEach(customer -> {
                ageTimesQuantity.put("bread", ageTimesQuantity.getOrDefault("bread", 0) + customer.age * customer.breadQuantity);
                ageTimesQuantity.put("milk", ageTimesQuantity.getOrDefault("milk", 0) + customer.age * customer.milkQuantity);
                ageTimesQuantity.put("egg", ageTimesQuantity.getOrDefault("egg", 0) + customer.age * customer.eggQuantity);
                ageTimesQuantity.put("potatoes", ageTimesQuantity.getOrDefault("potatoes", 0) + customer.age * customer.potatoQuantity);
                ageTimesQuantity.put("tomatoes", ageTimesQuantity.getOrDefault("tomatoes", 0) + customer.age * customer.tomatoQuantity);

                quantityOnly.put("bread", quantityOnly.getOrDefault("bread", 0) + customer.breadQuantity);
                quantityOnly.put("milk", quantityOnly.getOrDefault("milk", 0) + customer.milkQuantity);
                quantityOnly.put("egg", quantityOnly.getOrDefault("egg", 0) + customer.eggQuantity);
                quantityOnly.put("potatoes", quantityOnly.getOrDefault("potatoes", 0) + customer.potatoQuantity);
                quantityOnly.put("tomatoes", quantityOnly.getOrDefault("tomatoes", 0) + customer.tomatoQuantity);
              });

          String youngest = "bread";
          double youngestAvgAge = (double) ageTimesQuantity.get("bread") / quantityOnly.get("bread");
          for(String key: ageTimesQuantity.keySet()) {
            double currentAverage = (double) ageTimesQuantity.get(key) / quantityOnly.get(key);
            if( youngestAvgAge > currentAverage){
              youngest = key;
              youngestAvgAge = currentAverage;
            }
          }
          System.out.println(youngest);

        case "7":
          Map<String, Double> minPrices = new HashMap<>();
          Map<String, Double> maxPrices = new HashMap<>();
          Files.lines(Paths.get(csvFileName))
              .skip(1)
              .map(Parser::parseCustomer)
              .forEach(customer -> {
                if(customer.breadPrice != 0) {
                  minPrices.put("bread", Double.min(minPrices.getOrDefault("bread", Double.MAX_VALUE), customer.breadPrice));
                  maxPrices.put("bread", Double.max(maxPrices.getOrDefault("bread", Double.MIN_VALUE), customer.breadPrice));
                }

                if(customer.milkPrice != 0) {
                  minPrices.put("milk", Double.min(minPrices.getOrDefault("milk", Double.MAX_VALUE), customer.milkPrice));
                  maxPrices.put("milk", Double.max(maxPrices.getOrDefault("milk", Double.MIN_VALUE), customer.milkPrice));
                }

                if(customer.eggPrice != 0) {
                  minPrices.put("egg", Double.min(minPrices.getOrDefault("egg", Double.MAX_VALUE), customer.eggPrice));
                  maxPrices.put("egg", Double.max(maxPrices.getOrDefault("egg", Double.MIN_VALUE), customer.eggPrice));
                }

                if(customer.potatoPrice != 0) {
                  minPrices.put("potatoes", Double.min(minPrices.getOrDefault("potatoes", Double.MAX_VALUE), customer.potatoPrice));
                  maxPrices.put("potatoes", Double.max(maxPrices.getOrDefault("potatoes", Double.MIN_VALUE), customer.potatoPrice));
                }

                if(customer.tomatoPrice != 0) {
                  minPrices.put("tomatoes", Double.min(minPrices.getOrDefault("tomatoes", Double.MAX_VALUE), customer.tomatoPrice));
                  maxPrices.put("tomatoes", Double.max(maxPrices.getOrDefault("tomatoes", Double.MIN_VALUE), customer.tomatoPrice));
                }
              });

          String mostInflated = "bread";
          double biggestDifference = maxPrices.get("bread") - minPrices.get("bread");
          for(String key: minPrices.keySet()) {
            double currentDiff = maxPrices.get(key) - minPrices.get(key);
            if(currentDiff > biggestDifference) {
              mostInflated = key;
              biggestDifference = currentDiff;
            }
          }

          System.out.println(mostInflated);
      }
    }
    catch (Exception e){
      System.out.println(e.getMessage());
    }
  }

  // Helper method to update price tracking
  static void updatePriceForProduct(String product, double price,
                                    Map<String, Double> minPrices, Map<String, Double> maxPrices) {
    if (price != 0) {
      minPrices.put(product, Math.min(minPrices.getOrDefault(product, Double.MAX_VALUE), price));
      maxPrices.put(product, Math.max(maxPrices.getOrDefault(product, Double.MIN_VALUE), price));
    }
  }

  static <T extends Number> Map<String, T> getHashMap() {
    Map<String, T> map = new HashMap<>();
    for(String key: HelperVariables.products) {
      map.put(key, (T) Integer.valueOf(0));
    }
    return map;
  }
}