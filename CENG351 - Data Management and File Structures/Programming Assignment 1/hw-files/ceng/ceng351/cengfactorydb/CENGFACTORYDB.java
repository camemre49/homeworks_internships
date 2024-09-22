package ceng.ceng351.cengfactorydb;

import java.sql.*;
import java.util.ArrayList;
import java.util.List;

public class CENGFACTORYDB implements ICENGFACTORYDB {

    private static Connection connection = null;
    private static final String host = "144.122.71.128";
    private static final  String database = "db2518843";
    private static final  String port = "8080";
    private static final  String url = "jdbc:mysql://" + host + ":" + port + "/" + database;
    private static final  String user = "e2518843";
    private static final  String password = "ejOJgB+C6tXL";

    /**
     * Place your initialization code inside if required.
     *
     * <p>
     * This function will be called before all other operations. If your implementation
     * need initialization , necessary operations should be done inside this function. For
     * example, you can set your connection to the database server inside this function.
     */
    public void initialize() {
        try {
            Class.forName("com.mysql.cj.jdbc.Driver");
            connection = DriverManager.getConnection(url, user, password);
        } catch (ClassNotFoundException | SQLException e) {
            e.printStackTrace();
        }

        if (!(connection == null)) {
            System.out.println("Successfully Connected!");
        }
    }


    public static void main(String[] args) {
        CENGFACTORYDB cengfactorydb = new CENGFACTORYDB();
        cengfactorydb.initialize();
        cengfactorydb.createTables();
        cengfactorydb.insertFactory(FileOperations.readFactoryFile("data/Test_FactoryFile.txt"));
        cengfactorydb.insertEmployee(FileOperations.readEmployeeFile("data/Test_EmployeeFile.txt"));
        cengfactorydb.insertWorksIn(FileOperations.readWorksInFile("data/Test_WorksInFile.txt"));
        cengfactorydb.insertProduct(FileOperations.readProductFile("data/Test_ProductFile.txt"));
        cengfactorydb.insertProduce(FileOperations.readProduceFile("data/Test_ProduceFile.txt"));
        cengfactorydb.insertShipment(FileOperations.readShipmentFile("data/Test_ShipmentFile.txt"));
        //cengfactorydb.getFactoriesForGivenCountry("Turkey");
        //cengfactorydb.getFactoriesWithoutAnyEmployee();
        //cengfactorydb.getFactoriesProducingAllForGivenType("Technology");
        //cengfactorydb.getProductsProducedNotShipped();
        //cengfactorydb.getAverageSalaryForFactoryDepartment(100, "Sales");
        //cengfactorydb.getMostValueableProductsOnFactory();
        //cengfactorydb.getLowSalaryEmployeesForDepartments();
        //cengfactorydb.getMostValueableProducts();
        //cengfactorydb.increaseCost("Technology", 5);
        //cengfactorydb.deleteNotWorkingEmployees("2015-01-01");
        //cengfactorydb.dropTables();
    }



    /**
     * Should create the necessary tables when called.
     *
     * @return the number of tables that are created successfully.
     */
    public int createTables() {
        try {
            Statement statement = connection.createStatement();

            String createFactoryTableQuery = "CREATE TABLE IF NOT EXISTS Factory (" +
                    "factoryId INT, " +
                    "factoryName TEXT, " +
                    "factoryType TEXT, " +
                    "country TEXT, " +
                    "PRIMARY KEY (factoryId))";
            statement.executeUpdate(createFactoryTableQuery);
            // System.out.println("Factory Table Created.");

            String createEmployeeTableQuery = "CREATE TABLE IF NOT EXISTS Employee (" +
                    "employeeId INT, " +
                    "employeeName TEXT, " +
                    "department TEXT, " +
                    "salary INT, " +
                    "PRIMARY KEY (employeeId))";
            statement.executeUpdate(createEmployeeTableQuery);
            // System.out.println("Employee Table Created.");

            String createWorksInTableQuery = "CREATE TABLE IF NOT EXISTS Works_In (" +
                    "factoryId INT, " +
                    "employeeId INT, " +
                    "startDate DATE, " +
                    "PRIMARY KEY (factoryId, employeeId), " +
                    "FOREIGN KEY (factoryId) REFERENCES Factory(factoryId), " +
                    "FOREIGN KEY (employeeId) REFERENCES Employee(employeeId) ON DELETE CASCADE)";
            statement.executeUpdate(createWorksInTableQuery);
            // System.out.println("Works In Table Created.");

            String createProductTableQuery = "CREATE TABLE IF NOT EXISTS Product (" +
                    "productId INT, " +
                    "productName TEXT, " +
                    "productType TEXT, " +
                    "PRIMARY KEY (productId))";
            statement.executeUpdate(createProductTableQuery);
            // System.out.println("Product Table Created.");

            String createProduceTableQuery = "CREATE TABLE IF NOT EXISTS Produce (" +
                    "factoryId INT, " +
                    "productId INT, " +
                    "amount INT, " +
                    "productionCost INT, " +
                    "PRIMARY KEY (factoryId, productId), " +
                    "FOREIGN KEY (factoryId) REFERENCES Factory(factoryId), " +
                    "FOREIGN KEY (productId) REFERENCES Product(productId))";
            statement.executeUpdate(createProduceTableQuery);
            // System.out.println("Produce Table Created.");

            String createShipmentTableQuery = "CREATE TABLE IF NOT EXISTS Shipment (" +
                    "factoryId INT, " +
                    "productId INT, " +
                    "amount INT, " +
                    "pricePerUnit INT, " +
                    "PRIMARY KEY (factoryId, productId), " +
                    "FOREIGN KEY (factoryId) REFERENCES Factory(factoryId), " +
                    "FOREIGN KEY (productId) REFERENCES Product(productId))";
            statement.executeUpdate(createShipmentTableQuery);
            // System.out.println("Shipment Table Created.");

            System.out.println("All Tables Are Created!");

            statement.close();
            return 6;
        } catch (Exception e) {
            e.printStackTrace();
            return -1;
        }
    }

    /**
     * Should drop the tables if exists when called.
     *
     * @return the number of tables are dropped successfully.
     */
    public int dropTables() {
        try {
            Statement statement = connection.createStatement();

            // Drop tables with foreign key references
            statement.executeUpdate("DROP TABLE IF EXISTS Works_In");
            statement.executeUpdate("DROP TABLE IF EXISTS Produce");
            statement.executeUpdate("DROP TABLE IF EXISTS Shipment");

            // Drop tables without foreign key references
            statement.executeUpdate("DROP TABLE IF EXISTS Employee");
            statement.executeUpdate("DROP TABLE IF EXISTS Product");
            statement.executeUpdate("DROP TABLE IF EXISTS Factory");

            System.out.println("All Tables Are Dropped!");

            statement.close();
            return 6;
        } catch (Exception e) {
            e.printStackTrace();
            return -1;
        }
    }

    /**
     * Should insert an array of Factory into the database.
     *
     * @return Number of rows inserted successfully.
     */
    public int insertFactory(Factory[] factories) {
        var count = 0;
        try {
            Statement statement = connection.createStatement();
            for (Factory factory : factories) {
                String insertFactoryQuery = String.format("INSERT INTO Factory VALUES (%d, '%s', '%s', '%s')",
                        factory.getFactoryId(),
                        factory.getFactoryName(),
                        factory.getFactoryType(),
                        factory.getCountry());
                try {
                    statement.executeUpdate(insertFactoryQuery);
                } catch (Exception e) {
                    if (e.getMessage().startsWith("Duplicate")) {
                        count++;
                    } else throw e;
                    continue;
                }
                count++;
            }
            statement.close();
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
        System.out.printf("Inserted %d %s row.%n", count, "Factory");
        return count;
    }

    /**
     * Should insert an array of Employee into the database.
     *
     * @return Number of rows inserted successfully.
     */
    public int insertEmployee(Employee[] employees) {
        var count = 0;
        try {
            Statement statement = connection.createStatement();
            for (Employee employee : employees) {
                String insertEmployeeQuery = String.format("INSERT INTO Employee VALUES (%d, '%s', '%s', %d)",
                        employee.getEmployeeId(),
                        employee.getEmployeeName(),
                        employee.getDepartment(),
                        employee.getSalary());
                try {
                    statement.executeUpdate(insertEmployeeQuery);
                } catch (Exception e) {
                    if (e.getMessage().startsWith("Duplicate")) {
                        count++;
                    } else throw e;
                    continue;
                }
                count++;
            }
            statement.close();
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
        System.out.printf("Inserted %d %s row.%n", count, "Employee");
        return count;
    }

    /**
     * Should insert an array of WorksIn into the database.
     *
     * @return Number of rows inserted successfully.
     */
    public int insertWorksIn(WorksIn[] worksIns) {
        var count = 0;
        try {
            Statement statement = connection.createStatement();
            for (WorksIn worksIn : worksIns) {
                String insertWorks_inQuery = String.format("INSERT INTO Works_In VALUES (%d, %d, " + "'" + worksIn.getStartDate() + "'" + ")",
                        worksIn.getFactoryId(),
                        worksIn.getEmployeeId());
                try {
                    statement.executeUpdate(insertWorks_inQuery);
                } catch (Exception e) {
                    if (e.getMessage().startsWith("Duplicate")) {
                        count++;
                    } else throw e;
                    continue;
                }
                count++;
            }
            statement.close();
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
        System.out.printf("Inserted %d %s row.%n", count, "Works_In");
        return count;
    }

    /**
     * Should insert an array of Product into the database.
     *
     * @return Number of rows inserted successfully.
     */
    public int insertProduct(Product[] products) {
        var count = 0;
        try {
            Statement statement = connection.createStatement();
            for (Product product : products) {
                String insertProductQuery = String.format("INSERT INTO Product VALUES (%d, '%s', '%s')",
                        product.getProductId(),
                        product.getProductName(),
                        product.getProductType());
                try {
                    statement.executeUpdate(insertProductQuery);
                } catch (Exception e) {
                    if (e.getMessage().startsWith("Duplicate")) {
                        count++;
                    } else throw e;
                    continue;
                }
                count++;
            }
            statement.close();
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
        System.out.printf("Inserted %d %s row.%n", count, "Product");
        return count;
    }


    /**
     * Should insert an array of Produce into the database.
     *
     * @return Number of rows inserted successfully.
     */
    public int insertProduce(Produce[] produces) {
        var count = 0;
        try {
            Statement statement = connection.createStatement();
            for (Produce produce : produces) {
                String insertProduceQuery = String.format("INSERT INTO Produce VALUES (%d, %d, %d, %d)",
                        produce.getFactoryId(),
                        produce.getProductId(),
                        produce.getAmount(),
                        produce.getProductionCost());
                try {
                    statement.executeUpdate(insertProduceQuery);
                } catch (Exception e) {
                    if (e.getMessage().startsWith("Duplicate")) {
                        count++;
                    } else throw e;
                    continue;
                }
                count++;
            }
            statement.close();
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
        System.out.printf("Inserted %d %s row.%n", count, "Produce");
        return count;
    }


    /**
     * Should insert an array of Shipment into the database.
     *
     * @return Number of rows inserted successfully.
     */
    public int insertShipment(Shipment[] shipments) {
        var count = 0;
        try {
            Statement statement = connection.createStatement();
            for (Shipment shipment : shipments) {
                String insertProduceQuery = String.format("INSERT INTO Shipment VALUES (%d, %d, %d, %d)",
                        shipment.getFactoryId(),
                        shipment.getProductId(),
                        shipment.getAmount(),
                        shipment.getPricePerUnit());
                try {
                    statement.executeUpdate(insertProduceQuery);
                } catch (Exception e) {
                    if (e.getMessage().startsWith("Duplicate")) {
                        count++;
                    } else throw e;
                    continue;
                }
                count++;
            }
            statement.close();
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
        System.out.printf("Inserted %d %s row.%n", count, "Shipment");
        return count;
    }

    /**
     * Should return all factories that are located in a particular country.
     *
     * @return Factory[]
     */
    public Factory[] getFactoriesForGivenCountry(String country) {
        List<Factory> factoryList = new ArrayList<>();
        try {
            Statement statement = connection.createStatement();

            String createFactoryTableQuery =
                    "SELECT DISTINCT * " +
                            "FROM Factory F " +
                            "WHERE F.country = " + "'" + country + "' " +
                            "ORDER BY F.factoryId";
            ResultSet f = statement.executeQuery(createFactoryTableQuery);
            while (f.next()) {
                Factory factory = new Factory(f.getInt("factoryId"),
                        f.getString("factoryName"),
                        f.getString("factoryType"),
                        f.getString("country"));
                factoryList.add(factory);
            }
            statement.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
        Factory[] result = factoryList.toArray(new Factory[0]);
        return result;
    }


    /**
     * Should return all factories without any working employees.
     *
     * @return Factory[]
     */
    public Factory[] getFactoriesWithoutAnyEmployee() {
        List<Factory> factoryList = new ArrayList<>();
        try {
            Statement statement = connection.createStatement();

            String createFactoryTableQuery =
                    "SELECT DISTINCT * " +
                            "FROM Factory F " +
                            "WHERE F.factoryId NOT IN (SELECT DISTINCT W.factoryId FROM Works_In W) " +
                            "ORDER BY F.factoryId";
            ResultSet f = statement.executeQuery(createFactoryTableQuery);
            while (f.next()) {
                Factory factory = new Factory(f.getInt("factoryId"),
                        f.getString("factoryName"),
                        f.getString("factoryType"),
                        f.getString("country"));
                factoryList.add(factory);
            }
            statement.close();
        } catch (Exception e) {
            e.printStackTrace();
        }

        Factory[] result = factoryList.toArray(new Factory[0]);
        return result;
    }

    /**
     * Should return all factories that produce all products for a particular productType
     *
     * @return Factory[]
     */
    public Factory[] getFactoriesProducingAllForGivenType(String productType) {
        List<Factory> factoryList = new ArrayList<>();
        try {
            Statement statement = connection.createStatement();

            String createFactoryTableQuery =
                    "SELECT DISTINCT * FROM Factory F WHERE NOT EXISTS " +
                            "(SELECT DISTINCT P.productId FROM Product P WHERE P.productType = " + String.format("'%s'", productType) + " AND P.productId NOT IN " +
                            "(SELECT DISTINCT Pro.productID FROM Produce Pro WHERE F.factoryId = Pro.factoryId)) ORDER BY F.factoryId";

            ResultSet f = statement.executeQuery(createFactoryTableQuery);
            while (f.next()) {
                Factory factory = new Factory(f.getInt("factoryId"),
                        f.getString("factoryName"),
                        f.getString("factoryType"),
                        f.getString("country"));
                factoryList.add(factory);
            }
            statement.close();
        } catch (Exception e) {
            e.printStackTrace();
        }

        return factoryList.toArray(new Factory[0]);
    }


    /**
     * Should return the products that are produced in a particular factory but
     * don’t have any shipment from that factory.
     *
     * @return Product[]
     */
    public Product[] getProductsProducedNotShipped() {
        List<Product> factoryList = new ArrayList<>();
        try {
            Statement statement = connection.createStatement();

            String query =
                    "SELECT DISTINCT P.productId, P.productName, P.productType " +
                            "FROM Produce Pro, Product P " +
                            "WHERE Pro.productId = P.productId AND Pro.factoryId NOT IN (" +
                            "    SELECT S.factoryId" +
                            "    FROM Shipment S" +
                            "    WHERE S.productId = Pro.productId)" +
                            "ORDER BY P.productId";
            ResultSet f = statement.executeQuery(query);
            while (f.next()) {
                Product product = new Product(f.getInt("productId"),
                        f.getString("productName"),
                        f.getString("productType"));
                factoryList.add(product);
            }
            statement.close();
        } catch (Exception e) {
            e.printStackTrace();
        }

        return factoryList.toArray(new Product[0]);
    }


    /**
     * For a given factoryId and department, should return the average salary of
     * the employees working in that factory and that specific department.
     *
     * @return double
     */
    public double getAverageSalaryForFactoryDepartment(int factoryId, String department) {
        double result = 0.0;
        try {
            Statement statement = connection.createStatement();
            String query =
                    "SELECT DISTINCT AVG(E.salary) " +
                            "FROM Factory F, Employee E, Works_In W " +
                            "WHERE F.factoryId = W.factoryId AND E.employeeId = W.employeeId AND " +
                            "      F.factoryId = " + factoryId + " AND E.department = " + String.format("'%s'", department);
            ResultSet f = statement.executeQuery(query);
            f.next();
            result = f.getDouble(1);
            statement.close();
        } catch (Exception e) {
            e.printStackTrace();
        }

        return result;
    }


    /**
     * For each product, return the factories that gather the highest profit
     * for that product
     *
     * @return QueryResult.MostValueableProduct[]
     */
    public QueryResult.MostValueableProduct[] getMostValueableProducts() {
        List<QueryResult.MostValueableProduct> resultList = new ArrayList<>();
        try {
            Statement statement = connection.createStatement();
            String query =
                    "SELECT DISTINCT Pro.factoryId, Pro.productId, P.productName, P.productType, " +
                    "(coalesce(S.amount * S.pricePerUnit, 0) - Pro.amount * Pro.productionCost) as profit " +
                    "FROM Produce Pro " +
                    "LEFT JOIN Shipment S on Pro.factoryId = S.factoryId and Pro.productId = S.productId " +
                    "LEFT JOIN Product P on P.productId = Pro.productId " +
                    "WHERE (coalesce(S.amount * S.pricePerUnit, 0) - Pro.amount * Pro.productionCost) = (" +
                    "    SELECT MAX((coalesce(S2.amount * S2.pricePerUnit, 0) - Pro2.amount * Pro2.productionCost)) as maxProfit " +
                    "    FROM Produce Pro2 " +
                    "             LEFT JOIN Shipment S2 on Pro2.factoryId = S2.factoryId and Pro2.productId = S2.productId " +
                    "    WHERE Pro.factoryId = Pro2.factoryId" +
                    "    ) " +
                    "ORDER BY profit DESC, Pro.factoryId;";
            ResultSet f = statement.executeQuery(query);
            while (f.next()) {
                QueryResult.MostValueableProduct product = new QueryResult.MostValueableProduct(
                        f.getInt("factoryId"),
                        f.getInt("productId"),
                        f.getString("productName"),
                        f.getString("productType"),
                        f.getDouble("profit"));
                resultList.add(product);
            }
            statement.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return resultList.toArray(new QueryResult.MostValueableProduct[0]);
    }


    /**
     * Should return the most profitable products for each factory
     *
     * @return QueryResult.MostValueableProduct[]
     */
    public QueryResult.MostValueableProduct[] getMostValueableProductsOnFactory() {
        List<QueryResult.MostValueableProduct> resultList = new ArrayList<>();
        try {
            Statement statement = connection.createStatement();
            String query =
                    "SELECT DISTINCT Pro.factoryId, Pro.productId, P.productName, P.productType, " +
                    "(COALESCE(S.amount * S.pricePerUnit, 0) - Pro.amount * Pro.productionCost) AS profit " +
                    "FROM Produce Pro " +
                    "LEFT JOIN Shipment S ON Pro.factoryId = S.factoryId AND Pro.productId = S.productId " +
                    "LEFT JOIN Product P ON Pro.productId = P.productId " +
                    "WHERE (COALESCE(S.amount * S.pricePerUnit, 0) - Pro.amount * Pro.productionCost) = (" +
                    "    SELECT MAX(COALESCE(S2.amount * S2.pricePerUnit, 0) - Pro2.amount * Pro2.productionCost) " +
                    "    FROM Produce Pro2 " +
                    "    LEFT JOIN Shipment S2 ON Pro2.factoryId = S2.factoryId AND Pro2.productId = S2.productId " +
                    "    WHERE Pro.productId = Pro2.productId" +
                    ") " +
                    "ORDER BY profit DESC, Pro.productId;";
            ResultSet f = statement.executeQuery(query);
            while (f.next()) {
                QueryResult.MostValueableProduct product = new QueryResult.MostValueableProduct(
                        f.getInt("factoryId"),
                        f.getInt("productId"),
                        f.getString("productName"),
                        f.getString("productType"),
                        f.getDouble("profit"));
                resultList.add(product);
            }
            statement.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return resultList.toArray(new QueryResult.MostValueableProduct[0]);
    }


    /**
     * For each department, should return all employees that are paid under the
     * average salary for that department. You consider the employees
     * that do not work earning 0.
     *
     * @return QueryResult.LowSalaryEmployees[]
     */
    public QueryResult.LowSalaryEmployees[] getLowSalaryEmployeesForDepartments() {
        List<QueryResult.LowSalaryEmployees> resultList = new ArrayList<>();
        try {
            Statement statement = connection.createStatement();
            String query =
                    "SELECT DISTINCT E.employeeId, E.employeeName, E.department, " +
                    "IF(ISNULL(WI.employeeId), 0, salary) AS salary " +
                    "FROM Employee E " +
                    "LEFT JOIN Works_In WI ON E.employeeId = WI.employeeId " +
                    "WHERE IF(ISNULL(WI.employeeId), 0, salary) < (" +
                    "    SELECT AVG(salary2) " +
                    "    FROM (SELECT DISTINCT E2.employeeId, E2.employeeName, E2.department, " +
                    "                 IF(ISNULL(WI2.employeeId), 0, salary) AS salary2 " +
                    "          FROM Employee E2 " +
                    "          LEFT JOIN Works_In WI2 ON E2.employeeId = WI2.employeeId) AS salaries " +
                    "    WHERE salaries.department = E.department" +
                    ") " +
                    "ORDER BY E.employeeId;";
            ResultSet f = statement.executeQuery(query);
            while (f.next()) {
                QueryResult.LowSalaryEmployees product = new QueryResult.LowSalaryEmployees(
                        f.getInt("employeeId"),
                        f.getString("employeeName"),
                        f.getString("department"),
                        f.getInt("salary"));
                resultList.add(product);
            }
            statement.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return resultList.toArray(new QueryResult.LowSalaryEmployees[0]);
    }


    /**
     * For the products of given productType, increase the productionCost of every unit by a given percentage.
     *
     * @return number of rows affected
     */
    public int increaseCost(String productType, double percentage) {
        int affectedRows = 0;
        try {
            Statement statement = connection.createStatement();
            String updateQuery = "UPDATE Produce " +
                    "SET productionCost = Produce.productionCost + productionCost *" + percentage +"/100 " +
                    "WHERE productId IN (" +
                    "    SELECT DISTINCT P.productId " +
                    "    FROM Produce PRO " +
                    "             JOIN Product P ON PRO.productId = P.productId " +
                    "    WHERE productType = " + String.format("'%s'", productType) +
                    ")";
            affectedRows = statement.executeUpdate(updateQuery);
            statement.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
        System.out.println(affectedRows);
        return affectedRows;
    }


    /**
     * Deleting all employees that have not worked since the given date.
     *
     * @return number of rows affected
     */
    public int deleteNotWorkingEmployees(String givenDate) {
        int affectedRows = 0;
        try {
            Statement statement = connection.createStatement();
            String deleteQuery = "DELETE FROM Employee " +
                    "WHERE employeeId NOT IN (" +
                    "    SELECT DISTINCT W.employeeId " +
                    "    FROM Works_In W" +
                    "    WHERE W.startDate > " + String.format("'%s'", givenDate) +
                    ")";
            affectedRows = statement.executeUpdate(deleteQuery);
            statement.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
        System.out.println(affectedRows);
        return affectedRows;
    }


    /**
     * *****************************************************
     * *****************************************************
     * *****************************************************
     * *****************************************************
     *  THE METHODS AFTER THIS LINE WILL NOT BE GRADED.
     *  YOU DON'T HAVE TO SOLVE THEM, LEAVE THEM AS IS IF YOU WANT.
     *  IF YOU HAVE ANY QUESTIONS, REACH ME VIA EMAIL.
     * *****************************************************
     * *****************************************************
     * *****************************************************
     * *****************************************************
     */

    /**
     * For each department, find the rank of the employees in terms of
     * salary. Peers are considered tied and receive the same rank. After
     * that, there should be a gap.
     *
     * @return QueryResult.EmployeeRank[]
     */
    public QueryResult.EmployeeRank[] calculateRank() {
        return new QueryResult.EmployeeRank[0];
    }

    /**
     * For each department, find the rank of the employees in terms of
     * salary. Everything is the same but after ties, there should be no
     * gap.
     *
     * @return QueryResult.EmployeeRank[]
     */
    public QueryResult.EmployeeRank[] calculateRank2() {
        return new QueryResult.EmployeeRank[0];
    }

    /**
     * For each factory, calculate the most profitable 4th product.
     *
     * @return QueryResult.FactoryProfit
     */
    public QueryResult.FactoryProfit calculateFourth() {
        return new QueryResult.FactoryProfit(0, 0, 0);
    }

    /**
     * Determine the salary variance between an employee and another
     * one who began working immediately after the first employee (by
     * startDate), for all employees.
     *
     * @return QueryResult.SalaryVariant[]
     */
    public QueryResult.SalaryVariant[] calculateVariance() {
        return new QueryResult.SalaryVariant[0];
    }

    /**
     * Create a method that is called once and whenever a Product starts
     * losing money, deletes it from Produce table
     *
     * @return void
     */
    public void deleteLosing() {

    }
}
