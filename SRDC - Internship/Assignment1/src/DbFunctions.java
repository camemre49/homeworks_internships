import java.sql.*;
import java.util.ArrayList;
import java.util.Calendar;


public class DbFunctions {

    private Connection conn;

    public Connection getConn() {
        return conn;
    }

    public void createUserTable()
    {
        Statement statement;
        try {
            String query = "create table users (username varchar(200), password varchar(200), name varchar(200)," +
                    "surname varchar(200), gender varchar(200), email varchar(200), city varchar(200), birthdate date, admin boolean, primary key(username));";
            statement = conn.createStatement();
            statement.executeUpdate(query);
            System.out.println("Table Created.");

        }
        catch (Exception e)
        {
            System.out.println("Table exists.");
        }
    }

    public void createMessageTable()
    {
        Statement statement;
        try {
            String query = "create table messages (index serial , towho varchar(200), title varchar(200)," +
                    "message varchar(1000), fromwho varchar(200), date timestamp, deletedfromsender boolean default false, deletedfromreceiver boolean default false,  primary key(index));";
            statement = conn.createStatement();
            statement.executeUpdate(query);
            System.out.println("Table Created.");

        }
        catch (Exception e)
        {
            System.out.println("Table exists.");
        }
    }

    public ArrayList<User>  listUsers() // AYARLA
    {
        Statement statement;
        ArrayList<User> users = new ArrayList<User>();
        ResultSet resultSet = null;
        try {
            String query = String.format("select * from users");
            statement = conn.createStatement();
            resultSet = statement.executeQuery(query);
            while(resultSet.next()){
                String username = resultSet.getString("username");
                String password = resultSet.getString("password");
                String name = resultSet.getString("name");
                String surname = resultSet.getString("surname");
                String gender = resultSet.getString("gender");
                String email = resultSet.getString("email");
                String city = resultSet.getString("city");
                String birthdate = resultSet.getString("birthdate");
                String admin = resultSet.getString("admin");
                User u = new User(username, password, name, surname, gender, email, city, birthdate, admin);
                users.add(u);
            }
        }
        catch (Exception e){
            System.out.println(e);
            return null;
        }
        return users;
    }

    public String updateinfo(User old, String[] fields, String[] info)
    {
        Statement statement;
        String oldusername = old.getUsername();
        int index = 1;
        try
        {
           while(index < info.length){

               String query = String.format("update users set %s ='%s' where username='%s'", fields[index + 1], info[index], old.getUsername());
               if(fields[index + 1].equals("username")){
                   old.setUserneme(info[index]);
               }
               statement = conn.createStatement();
               statement.executeUpdate(query);
               index++;
           }
           return oldusername + "'s info has updated";
        }
        catch (Exception e)
        {
            System.out.println(e);
        }
        return null;
    }

    public User searchbyName(String username)
    {
        Statement statement;
        ResultSet resultSet = null;
        try
        {
            String query = String.format("select * from users where username = '%s'", username);
            statement = conn.createStatement();
            resultSet = statement.executeQuery(query);
            if (resultSet.next()) {
                String password = resultSet.getString("password");
                String name = resultSet.getString("name");
                String surname = resultSet.getString("surname");
                String gender = resultSet.getString("gender");
                String email = resultSet.getString("email");
                String city = resultSet.getString("city");
                String birthdate = resultSet.getString("birthdate");
                String admin = resultSet.getString("admin");

                return new User(username, password, name, surname, gender, email, city, birthdate, admin);

            } else {
                return new User();
            }
        }
        catch (Exception e)
        {
            System.out.println(e);
        }
        return new User();
    }

    public void deleteRowbyName(String username)
    {
        Statement statement;

        try
        {
            String query = String.format("delete from users where username = '%s'", username);
            statement = conn.createStatement();
            statement.executeUpdate(query);
        }
        catch (Exception e)
        {
            System.out.println(e);
        }
    }

    public boolean insertUser(User user){
        try
        {
            String insertUser = "INSERT INTO users" + " (username, password, name, surname, gender, email, city, birthdate, admin) VALUES " + " (?, ?, ?, ?, ?, ?, ?, ?, ?);";
            try (PreparedStatement preparedStatement = conn.prepareStatement(insertUser)) {
                preparedStatement.setString(1, user.getUsername());
                preparedStatement.setString(2, user.getPassword());
                preparedStatement.setString(3, user.getName());
                preparedStatement.setString(4, user.getSurname());
                preparedStatement.setString(5, user.getGender());
                preparedStatement.setString(6, user.getEmail());
                preparedStatement.setString(7, user.getCity());
                String[] birthdate = user.getBirthdate().split("-");
                Calendar calendar = Calendar.getInstance();
                calendar.set(Calendar.YEAR, Integer.parseInt(birthdate[0]));
                calendar.set(Calendar.MONTH, Integer.parseInt(birthdate[1]) - 1);
                calendar.set(Calendar.DAY_OF_MONTH, Integer.parseInt(birthdate[2]));
                java.sql.Date date = new java.sql.Date(calendar.getTime().getTime());
                preparedStatement.setDate(8, date);
                boolean admin = user.getAdmin().equals("y");
                preparedStatement.setBoolean(9, admin);
                preparedStatement.executeUpdate();
                System.out.println("New person " + user.getUsername() + " added to the server as "+ (admin ? "admin" : "user") + ".");
                return true;
        }
        }
        catch (Exception e){
            System.out.println(user.getUsername() + " is already registered.");
            return false;
        }
    }

    public int countUsers(String tablename) throws Exception{
        Statement s = conn.createStatement();
        ResultSet r = s.executeQuery("SELECT COUNT(*) AS recordCount FROM " + tablename);
        r.next();
        int count = r.getInt("recordCount");
        r.close();
        return count;
    }

    public void insertMessage(Message message){
        try
        {
            String insertUser = "INSERT INTO messages (towho, title, message, fromwho, date) VALUES (?, ?, ?, ?, ?);";
            try (PreparedStatement preparedStatement = conn.prepareStatement(insertUser)) {
                preparedStatement.setString(1, message.getTo());
                preparedStatement.setString(2, message.getTitle());
                preparedStatement.setString(3, message.getMessage());
                preparedStatement.setString(4, message.getFrom());
                preparedStatement.setTimestamp(5, message.getDate());
                preparedStatement.executeUpdate();
                System.out.println("A message has sent!");
            }
            catch (Exception e) {
                System.out.println(e);
            }
        }
        catch (Exception e){
            System.out.println(e);
        }
    }

    public ArrayList<Message>  messageQuery(boolean inOut, String username){
        ArrayList<Message> messages = new ArrayList<Message>();
        Statement statement;
        String inorOut = inOut ? "deletedFromReceiver" : "deletedFromSender"; // true for inbox, false for outbox
        String toorFrom = inOut ? "towho" : "fromwho";
        ResultSet resultSet = null;
        try
        {
            String query = String.format("select * from messages where %s = '%b' and %s = '%s'", inorOut, false, toorFrom, username);
            statement = conn.createStatement();
            resultSet = statement.executeQuery(query);
            while(resultSet.next()){
                Message m = new Message(resultSet.getInt("index"),
                        resultSet.getString("towho"),
                        resultSet.getString("title"),
                        resultSet.getString("message"),
                        resultSet.getString("fromwho"),
                        resultSet.getTimestamp("date"),
                        resultSet.getBoolean("deletedfromreceiver"),
                        resultSet.getBoolean("deletedfromsender"));
                messages.add(m);
            }
            return messages;
        }
        catch (Exception e)
        {
            System.out.println(e);
        }
        return messages ;
    }

    public Message messageQuerybyIndex(Message message, boolean inOut){
        Statement statement;
        String toorFrom = inOut ? "towho" : "fromwho";
        String inorOut = inOut ? "deletedFromReceiver" : "deletedFromSender";
        ResultSet resultSet = null;
        try
        {
            String query = String.format("select * from messages where %s = '%s' and index = '%d' and %s = false", toorFrom, message.getMessage(), message.getindex(), inorOut);
            statement = conn.createStatement();
            resultSet = statement.executeQuery(query);
            if(resultSet.next()){
                Message m = new Message(resultSet.getInt("index"),
                        resultSet.getString("towho"),
                        resultSet.getString("title"),
                        resultSet.getString("message"),
                        resultSet.getString("fromwho"),
                        resultSet.getTimestamp("date"),
                        resultSet.getBoolean("deletedfromreceiver"),
                        resultSet.getBoolean("deletedfromsender"));
                return m;
            }
            else{
                return new Message();
            }
        }
        catch (Exception e)
        {
            System.out.println(e);
        }
        return new Message();
    }

    public void deleteMessage(String id, boolean inOut){
        String inorOut = inOut ? "deletedfromreceiver" : "deletedfromsender";
        try
        {
            String insertUser = "update messages set " + inorOut + " = ? where index = ?;";
            try (PreparedStatement preparedStatement = conn.prepareStatement(insertUser)) {
                preparedStatement.setBoolean(1, true);
                preparedStatement.setInt(2, Integer.parseInt(id));
                preparedStatement.executeUpdate();
            }
            catch (Exception e){
                System.out.println(e);
            }
            System.out.println(String.format("Message with index '%s' has deleted.", id));
        }
        catch (Exception e)
        {
            System.out.println(e);
        }
    }

    public void setConn(Connection conn) {
        this.conn = conn;
    }

    public int searchbyNameActive(String username)
    {
        Statement statement;
        ResultSet resultSet = null;
        try
        {
            String query = String.format("select * from activeusers where username = '%s' and userin = true", username);
            statement = conn.createStatement();
            resultSet = statement.executeQuery(query);
            if(resultSet.next()){
                return resultSet.getInt("index");
            }
            else{
                return -1;
            }

        }
        catch (Exception e)
        {
            System.out.println(e);
        }
        return -1;
    }

    public boolean insertActive(User user){
        try
        {
            String insertUser = "INSERT INTO activeusers" + " (username, index, userin) VALUES " + " (?, ?, ?);";
            try (PreparedStatement preparedStatement = conn.prepareStatement(insertUser)) {
                preparedStatement.setString(1, user.getUsername());
                preparedStatement.setInt(2, user.getIndex());
                preparedStatement.setBoolean(3, true);
                preparedStatement.executeUpdate();
                return true;
            }
        }
        catch (Exception e){
            return false;
        }
    }

    public void deleteRowbyNameActive(String username)
    {
        Statement statement;

        try
        {
            String query = String.format("update activeusers set userin ='false' where username='%s'", username);
            statement = conn.createStatement();
            statement.executeUpdate(query);
        }
        catch (Exception e)
        {
            System.out.println(e);
        }
    }

    public DbFunctions(String dbname, String user, String password) {
        Connection conn = null;
        try
        {
            Class.forName("org.postgresql.Driver");
            conn = DriverManager.getConnection("jdbc:postgresql://localhost:5432/" + dbname, user, password);
            this.conn = conn;
            if(conn != null)
            {
                System.out.println("Connection Established.");
            }
            else {
                System.out.println("Connection Failed.");
            }

        }
        catch (Exception e)
        {
            System.out.println(e);
        }
    }


}
