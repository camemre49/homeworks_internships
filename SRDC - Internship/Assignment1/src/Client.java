import com.sun.source.tree.BreakTree;

import javax.crypto.Cipher;
import java.awt.desktop.SystemSleepEvent;
import java.io.*;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Date;
import java.util.Scanner;

public class Client
{
    private Socket socket;
    private BufferedReader bufferedReader;
    private BufferedWriter bufferedWriter;
    private String username;
    private String password;
    private String name;
    private String surname;
    private Date date;
    private String gender;
    private String email;

    private String birthdate;
    private String city;

    private boolean adminflag;

    public static void main(String[] args)
    {
        try {
            Scanner scanner = new Scanner(System.in);
            Socket socket = new Socket("localhost", 9095);
            while (socket.isConnected()){
                System.out.println("Welcome to the server. You can write your commands.");
                String command = scanner.nextLine();
                String serverAnswer;
                if (command.equals("LOGIN")) {
                    System.out.println("You can login to the server.\nIf you want to go to back write '! ' to the username or password");
                    System.out.print("Enter your username: ");
                    String username = scanner.nextLine();
                    if (username.equals("!"))
                    {
                        continue;
                    }
                    System.out.print("Enter your password: ");
                    String password = scanner.nextLine();
                    if (password.equals("!"))
                    {
                        continue;
                    }
                    Client client = new Client(socket, username, password);
                    boolean[] flags = client.login();
                    if (flags[0] && flags[1]) {
                        if (client.loggedIn(flags[2]))
                        {
                            continue;
                        }
                    }
                }

                else if (command.equals("LOGOUT"))
                {
                    System.out.println("You are exiting from the server.");
                    socket.close();
                    break;
                }
                else if(command.equals("HELP")){
                    System.out.println("This is the login screen.\nLOGIN: to login the server.\nLOGOUT: to close connection.");
                }
                else{
                    System.out.println("Command not recognized!");
                }
            }
        }
        catch(Exception e)
        {
            System.out.println(e);
        }
    }
    public boolean[] login(){
        boolean[] flags = {false, false, false};
        try{
            bufferedWriter.write("LOGIN:::" + username + ":::" + password);
            bufferedWriter.newLine();
            bufferedWriter.flush();
            String serverAnswer = bufferedReader.readLine();
            System.out.println(serverAnswer);
            if(serverAnswer.charAt(0) == 'Y'){ // If server answer is You entered... return 1 1
                if(serverAnswer.charAt(26) == 'a')
                {
                    flags[2] = true;
                }
                flags[0] = true;
                flags[1] = true;
                String accountInfo = bufferedReader.readLine();
                String[] info = accountInfo.split(":::");
                name = info[2];
                surname = info[3];
                gender = info[4];
                email = info[5];
                birthdate = info[7];
                city = info[6];
            }
            else if(serverAnswer.charAt(0) == 'W'){ // Wrong Password... return 1 0
                flags[1] = true;
            }
            else {
                return flags; // return 0 0
            }
        }
        catch (Exception e)
        {
            System.out.println(e);
        }
        return  flags;
    }
    public Client(Socket socket, String username, String password) {
        try {
            this.socket = socket;
            this.bufferedWriter = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()));
            this.bufferedReader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            this.username = username;
            this.password = password;
        } catch (Exception e) {
            System.out.println(e);
        }
    }
    public boolean loggedIn(boolean adminflag){
        try {
            this.adminflag = adminflag;
            bufferedWriter.write("IN:::"+ username + (adminflag ? 1 : 0));
            bufferedWriter.newLine();
            bufferedWriter.flush();
            while (socket.isConnected()) {
                Scanner scanner = new Scanner(System.in);
                System.out.println("Write your command.");
                String command = scanner.nextLine();
                if(controluser()){
                    return true;
                }
                if (command.equals("LOGOUT")){
                    bufferedWriter.write("OUT:::" + username + (adminflag? 1: 0));
                    bufferedWriter.newLine();
                    bufferedWriter.flush();
                    System.out.println("Good bye " + this.name + ".");
                    return true;
                }

                else if(command.equals("ADDUSER")){
                    if(!adminflag){
                        System.out.println("You are not an admin " + username);
                    }
                    else{
                        adduser();
                    }
                }

                else if(command.equals("LISTUSERS")){
                    if(!adminflag){
                        System.out.println("You are not an admin " + username);
                    }
                    else{
                        bufferedWriter.write("LISTUSERS");
                        bufferedWriter.newLine();
                        bufferedWriter.flush();
                        String userCount = bufferedReader.readLine();
                        int count = Integer.parseInt(userCount);
                        String userinfo;
                        int user;
                        for(user = 0; user < count; user++){
                            userinfo = bufferedReader.readLine();
                            String[] lines = userinfo.split(":::");
                            int a;
                            for(a = 0; a < 4; a++){
                                System.out.println(lines[a]);
                            }
                            System.out.println("\n\n*** *** *** *** *** *** *** *** *** *** *** ***");
                        }
                        System.out.println("All users are listed. There are " + count + " users.");
                    }
                }

                else if(command.equals("REMOVEUSER")){
                    if(!adminflag){
                        System.out.println("You are not an admin " + username);
                    }
                    else{
                        removeuser();
                    }
                }

                else if(command.equals("UPDATEUSER")){
                    if(!adminflag){
                        System.out.println("You are not an admin " + username);
                    }
                    else{
                        updateuser();
                    }
                }

                else if(command.equals("SENDMESSAGE")){
                    sendmessage();
                }

                else if(command.equals("INBOX")){
                    inbox();
                }

                else if(command.equals("OUTBOX")){
                    outbox();
                }

                else if(command.equals("DELETEMESSAGE")){
                    System.out.println("To delete from INBOX write 'IN'");
                    System.out.println("To delete from OUTBOX write 'OUT'");
                    String answer  = scanner.nextLine();
                    while(!(answer.equals("IN") || (answer.equals("OUT"))))
                    {
                        answer  = scanner.nextLine();
                    }
                    System.out.println("What is the message id: ");
                    deleteMessage(answer + ":::" + scanner.nextLine());
                }

                else if(command.equals("HELP")){
                    if(adminflag){
                        System.out.println("This is the command screen.\nYou are an admin.");
                        System.out.println("LOGOUT: to back to the login screen");
                        System.out.println("SENDMESSAGE: to send a message");
                        System.out.println("DELETEMESSAGE: to delete a message from your inbox or outbox");
                        System.out.println("INBOX: to check inbox\nOUTBOX: to check outbox");
                        System.out.println();
                        System.out.println("As an admin you can do:");
                        System.out.println("ADDUSER: to add a user");
                        System.out.println("REMOVEUSER: to remove a user");
                        System.out.println("UPDATEUSER: to update an account info");
                        System.out.println("LISTUSERS: to list all users");
                    }
                    else{
                        System.out.println("This is the command screen.\nYou are a user.");
                        System.out.println("LOGOUT: to back to the login screen");
                        System.out.println("SENDMESSAGE: to send a message");
                        System.out.println("DELETEMESSAGE: to delete a message from your inbox or outbox");
                        System.out.println("INBOX: to check inbox\nOUTBOX: to check outbox");
                    }
                }
                else{
                    System.out.println("Command not recognized!");
                }

                System.out.println();
                System.out.println("You are in the command menu!");
            }
            System.out.println("Connection is lost.");
        }
        catch (Exception e){
            System.out.println("Connection is lost.");
        }
        return true;
    }
    public String makeProtocol(String[] accinfo){
        String clientinfo = "";
        for (String i : accinfo){
            clientinfo = clientinfo.concat(i);
            clientinfo = clientinfo.concat(":::");
        }
        return clientinfo;
    }
    public void adduser() throws IOException{
        Scanner scanner = new Scanner(System.in);
        String[] accountInfo = {"", "", "", "", "", "", "", "", ""};
        System.out.println("To cancel write '!' to any field or leave empty.");
        System.out.print("Username: ");
        accountInfo[0] = scanner.nextLine();
        usernameException(accountInfo[0]);
        if (accountInfo[0].equals("!") || accountInfo[0].equals("")){
            System.out.println("Canceled!");
            return;
        }

        System.out.print("Password: ");
        accountInfo[1] = scanner.nextLine();
        passwordException(accountInfo[1]);
        if (accountInfo[1].equals("!") || accountInfo[1].equals("")){
            System.out.println("Canceled!");
            return;
        }

        System.out.print("Name: ");
        accountInfo[2] = scanner.nextLine();
        nameException(accountInfo[2]);
        if (accountInfo[2].equals("!") || accountInfo[2].equals("")){
            System.out.println("Canceled!");
            return;
        }

        System.out.print("Surname: ");
        accountInfo[3] = scanner.nextLine();
        surnameException(accountInfo[3]);
        if (accountInfo[3].equals("!") || accountInfo[3].equals("")){
            System.out.println("Canceled!");
            return;
        }

        System.out.print("E-mail: ");
        accountInfo[4] = scanner.nextLine();
        emailException(accountInfo[4]);
        if (accountInfo[4].equals("!") || accountInfo[4].equals("")){
            System.out.println("Canceled!");
            return;
        }

        System.out.print("Gender: ");
        accountInfo[5] = scanner.nextLine();
        genderException(accountInfo[5]);
        if (accountInfo[5].equals("!") || accountInfo[5].equals("")){
            System.out.println("Canceled!");
            return;
        }

        System.out.print("City: ");
        accountInfo[6] = scanner.nextLine();
        cityException(accountInfo[6]);
        if (accountInfo[6].equals("!") || accountInfo[6].equals("")){
            System.out.println("Canceled!");
            return;
        }

        System.out.print("Birthdate: ");
        System.out.println("Please write in the form YEAR-MONTH-DAY: ");
        accountInfo[7] = scanner.nextLine();
        birthdateException(accountInfo[7]);
        if (accountInfo[7].equals("!") || accountInfo[7].equals("")){
            System.out.println("Canceled!");
            return;
        }

        System.out.println("Is the user admin? (Press 'y' if yes, 'n' for no)");
        accountInfo[8] = scanner.nextLine();
        while (!(accountInfo[8].equals("y") || accountInfo[8].equals("n"))){
            System.out.println("Is the user admin? (Press 'y' if yes, 'n' for no)");
            accountInfo[8] = scanner.nextLine();
            if (accountInfo[8].equals("!") || accountInfo[8].equals("")){
                System.out.println("Canceled!");
                return;
            }
        }
        if(accountInfo[8].equals("y")){
            accountInfo[8] = "true";
        }
        else{
            accountInfo[8] = "false";
        }
        String userinfo = makeProtocol(accountInfo);
        bufferedWriter.write("ADDUSER:::" + userinfo);
        bufferedWriter.newLine();
        bufferedWriter.flush();
        if(bufferedReader.readLine().equals("Y")){
            System.out.println("New user has added to the server with username '" + accountInfo[0] + "'.");
        }
        else{
            System.out.println("This username is already used!");
        }
    }


        public String usernameException (String str){
        return str;
    }
        public String passwordException (String str){
        return str;
    }
        public String genderException (String str){
        return str;
    }
        public String nameException (String str){
        return str;
    }
        public String surnameException (String str){
        return str;
    }
        public String emailException (String str){
        return str;
    }
        public String birthdateException (String str){
        return str;
    }
        public String cityException (String str){
        return str;
    }

    public void removeuser(){
        Scanner scanner = new Scanner(System.in);
        System.out.println("To cancel you can write '!' or leave empty.");
        System.out.print("Give the username of the account that you want to delete: ");
        String usernameToDelete = scanner.nextLine();
        if(username.equals(usernameToDelete)){
            System.out.println("You cannot delete your own account. Send message to an admin. (Default admin: emre)");
            return;
        }
        try{
            bufferedWriter.write("REMOVEUSER:::" + usernameToDelete);
            bufferedWriter.newLine();
            bufferedWriter.flush();
            String line = bufferedReader.readLine();
            if(line.equals("ERR")) {
                System.out.println("Canceled!");
            }
            else if(line.equals("NOUSER")){
                System.out.println("There is no user with username: " + usernameToDelete);
            }
            else{
                System.out.println("Are you sure ('y' for yes, 'n' for no)");
                String answer = scanner.nextLine();
                while(!(answer.equals("y") || answer.equals("n"))){
                    System.out.println("Please write 'y' or 'n'");
                    answer = scanner.nextLine();
                }

                if(answer.equals("y")){
                    bufferedWriter.write("Y");
                    bufferedWriter.newLine();
                    bufferedWriter.flush();
                    System.out.println("Account " + usernameToDelete + " deleted.");
                }
                else{
                    bufferedWriter.write("N");
                    bufferedWriter.newLine();
                    bufferedWriter.flush();
                    System.out.println("Canceled!");
                }
                line = bufferedReader.readLine();
            }
        }
        catch (Exception e){
            System.out.println(e);
        }
    }

    public void updateuser(){
        Scanner scanner = new Scanner(System.in);
        System.out.println("To cancel you can write '!' or leave empty.");
        System.out.print("Give the username of the account that you want to update: ");
        String usernameToUpdate = scanner.nextLine();

        try{
            bufferedWriter.write("UPDATEUSER:::" + usernameToUpdate);
            bufferedWriter.newLine();
            bufferedWriter.flush();
            String line = bufferedReader.readLine();
            if(line.equals("ERR")) {
                System.out.println("Canceled!");
            }
            else if(line.equals("NOUSER")){
                System.out.println("There is no user with username: " + usernameToUpdate);
            }
            else{
                String propertiesToUpdate = usernameToUpdate;
                ArrayList<String> fields = new ArrayList<String >();
                fields.add("username");
                fields.add("password");
                fields.add("name");
                fields.add("surname");
                fields.add("gender");
                fields.add("email");
                fields.add("city");
                fields.add("birthdate");
                fields.add("admin");
                System.out.println("Which properties do you want to update?" +
                        "\nYou can write 'username', 'password', 'name', 'surname', 'gender', 'email', 'city', 'birthdate', 'admin' (press enter after entering each field)" +
                        "\nYou can write '!' or leave empty to cancel." +
                        "\nAfter entering all the fields you can write 'y'" +
                        "\nIf you want to change all information, write 'all' and press enter.");
                String answer = "";
                while(!(answer.equals("y") || answer.equals("!") || answer.equals("all"))){
                    answer = scanner.nextLine();
                    if(!fields.contains(answer)){
                        if((answer.equals("y"))) {
                            break;
                        }
                        else if(answer.equals("!")){
                            propertiesToUpdate = "CANCEL";
                            System.out.println("Canceled!");
                            break;
                        }
                        else if(answer.equals("all")){
                            propertiesToUpdate = ":::username:::password:::name:::surname:::gender:::email:::city:::birthdate:::admin";
                            break;
                        }
                        System.out.println("Field is not recognized" + "\nYou can write 'username', 'password', 'name', 'surname', 'gender', 'email', 'city', 'birthdate', 'admin' (press enter after entering each field)");
                    }
                    else{
                        propertiesToUpdate = propertiesToUpdate.concat(":::" + answer);
                        System.out.println(answer + " will be changed");
                    }
                }
                bufferedWriter.write(usernameToUpdate + ":::" + propertiesToUpdate);
                bufferedWriter.newLine();
                bufferedWriter.flush();
                String[] info = propertiesToUpdate.split(":::");
                String newinfo = "";
                int index = 1;
                String in;
                while(index < info.length){
                    if(info[index].equals("admin")){
                        System.out.println("Is he admin? (y for yes, n for no)");
                        in = scanner.nextLine();
                        while (!(in.equals("y") || in.equals("n"))){
                            in = scanner.nextLine();
                        }
                    }
                    else{
                        System.out.print(info[index] + ": ");
                        in = scanner.nextLine();
                    }
                    if (in.equals("!") || in.equals("")){
                        System.out.println("Canceled!");
                        bufferedWriter.write("CANCEL");
                        bufferedWriter.newLine();
                        bufferedWriter.flush();
                        return;
                    }
                    else{
                        newinfo = newinfo.concat(":::" + in);
                    }
                    index++;
                }
                bufferedWriter.write(newinfo);
                bufferedWriter.newLine();
                bufferedWriter.flush();
                System.out.println("User has updated!");
            }
        }
        catch (Exception e){
            System.out.println(e);
        }

    }

    public void sendmessage(){
        Scanner scanner = new Scanner(System.in);
        String protocol = "";
        String answer = "";
        System.out.println("To cancel write '!' to any field or leave empty.");
        System.out.println("Write the username to whom the message will be sent: ");
        answer = scanner.nextLine();
        if (answer.equals("!") || answer.equals("")){
            System.out.println("Canceled!");
            return;
        }
        protocol = protocol.concat(answer + ":::");
        System.out.println("Write the title of the message: ");
        answer = scanner.nextLine();
        if (answer.equals("!") || answer.equals("")){
            System.out.println("Canceled!");
            return;
        }
        protocol = protocol.concat(answer + ":::");
        System.out.println("Write the message");
        answer = scanner.nextLine();
        if (answer.equals("!") || answer.equals("")){
            System.out.println("Canceled!");
            return;
        }
        protocol = protocol.concat(answer + ":::");

        protocol = protocol.concat(username + ":::");
        try{
            bufferedWriter.write("SENDMESSAGE:::" + protocol);
            bufferedWriter.newLine();
            bufferedWriter.flush();
            String serveranswer = bufferedReader.readLine();
            System.out.println(serveranswer);
        }
        catch (Exception e){
            System.out.println(e);
        }
    }

    public void inbox() throws IOException{
        bufferedWriter.write("MINBOX:::"+username);
        bufferedWriter.newLine();
        bufferedWriter.flush();
        String serveranswer = bufferedReader.readLine();
        {
            int count = 0;
            while (!serveranswer.equals("")){

                String[] message = serveranswer.split(":::");
                if(message.length != 5){
                    break;
                }
                System.out.println(message[0] + "-) " + message[1]);
                System.out.println(message[2]);
                System.out.println("From: "+ message[3]);
                String[] time = message[4].split(" ");
                System.out.println("Date: " + time[0]);
                System.out.println("Time: " + time[1].substring(0,8));
                System.out.println();
                serveranswer = bufferedReader.readLine();
                count++;
            }
            if(count == 0){
                System.out.println("Inbox is empty!");
            }
        }
    }
    public void outbox() throws IOException{
        bufferedWriter.write("MOUTBOX:::" + username);
        bufferedWriter.newLine();
        bufferedWriter.flush();
        String serveranswer = bufferedReader.readLine();;
        int count = 0;
        {
            while (!serveranswer.equals("")){
                String[] message = serveranswer.split(":::");
                if(message.length != 5){
                    break;
                }
                System.out.println(message[0] + "-) " + message[1]);
                System.out.println(message[2]);
                System.out.println("To: "+ message[3]);
                String[] time = message[4].split(" ");
                System.out.println("Date: " + time[0]);
                System.out.println("Time: " + time[1].substring(0,8));
                System.out.println();
                serveranswer = bufferedReader.readLine();
                count++;
            }
        }
        if(count == 0){
            System.out.println("Outbox is empty!");
        }
    }

    public void deleteMessage(String mes) throws  Exception{
        String[] info = mes.split(":::");
        String id = info[1];
        if(Integer.parseInt(id) <= 0) {
            System.out.println("Invalid id, it must be greater than 0!");
            return;
        }
        bufferedWriter.write("DELETE:::" + info[0] + ":::" + id + ":::" + username);
        bufferedWriter.newLine();
        bufferedWriter.flush();
        String serveranswer = bufferedReader.readLine();
        if(serveranswer.equals("ERR")){
            System.out.println("En error occured, process canceled!");
        }
        else if(serveranswer.equals("DEL")){
            System.out.println("Message has deleted!");
        }
        else {
            System.out.println("There is no message with index "+ id);
        }
    }

    public boolean controluser() throws IOException{
        bufferedWriter.write("CONTROL:::" + username);
        bufferedWriter.newLine();
        bufferedWriter.flush();
        String serveranswer = bufferedReader.readLine();
        if(serveranswer.equals("N")){
            bufferedWriter.write("OUT:::" + username + (adminflag? 1: 0));
            bufferedWriter.newLine();
            bufferedWriter.flush();
            System.out.println("Your account has deleted!");
            return true;
        }
        return false;
    }
}
