import java.net.Socket;
import java.io.*;
import java.sql.Connection;
import java.sql.ResultSet;
import java.util.ArrayList;

public class ClientHandler implements  Runnable {

    private Socket socket;
    private Connection conn;
    private BufferedReader bufferedReader;
    private BufferedWriter bufferedWriter;
    private DbFunctions db;
    private String username;
    public static ArrayList<ClientHandler> clients = new ArrayList<>();

    public void run(){
        String UserCommand;
        while (socket.isConnected())
        {
            try
            {
                UserCommand = bufferedReader.readLine();
                if(UserCommand.startsWith("LOGIN")) {
                    login(UserCommand);
                }
                else if(UserCommand.startsWith("IN")){
                    if(UserCommand.charAt(UserCommand.length() - 1) == '0')
                    {
                        System.out.println("User " + UserCommand.substring(5, UserCommand.length() - 1) + " has connected to the server.");
                    }
                    else {
                        System.out.println("Admin " + UserCommand.substring(5,UserCommand.length() - 1) + " has connected to the server.");
                    }
                    username = UserCommand.substring(5,UserCommand.length() - 1);
                }
                else if(UserCommand.startsWith("OUT")){
                    if(UserCommand.charAt(UserCommand.length() - 1) == '0')
                    {
                        System.out.println("User " + UserCommand.substring(6, UserCommand.length() - 1) + " has disconnected from the server.");
                    }
                    else {
                        System.out.println("Admin " + UserCommand.substring(6,UserCommand.length() - 1) + " has disconnected from the server.");
                    }
                }
                else if(UserCommand.startsWith("ADDUSER")){
                    String[] infos = UserCommand.split(":::");
                    User user = new User(infos[1], infos[2], infos[3], infos[4], infos[5], infos[6], infos[7], infos[8], infos[9]);
                    boolean isUpdated  = db.insertUser(user);
                    if(!isUpdated){
                        sendNotification("N", "");
                    }
                    else{
                        sendNotification("Y", "");
                    }
                }
                else if(UserCommand.equals("LISTUSERS")){
                    Integer count = db.countUsers("users");
                    sendNotification(count.toString(), "");
                    ArrayList<User> users = db.listUsers();
                    String userinfo;
                    int index = 1;
                    for (int i = 0; i < users.size(); i++)
                    {
                        userinfo = String.format("%d-) Username: %s,   Password: %s:::Name: %s,   Surname: %s,   Gender: %s:::E-mail: %s,   City: %s,   Birthdate: %s:::Is he admin: %s",
                                index,
                                users.get(i).getUsername(),
                                users.get(i).getPassword(),
                                users.get(i).getName(),
                                users.get(i).getSurname(),
                                users.get(i).getGender(),
                                users.get(i).getEmail(),
                                users.get(i).getCity(),
                                users.get(i).getBirthdate(),
                                users.get(i).getAdmin());
                                sendNotification(userinfo, "");
                                index++;
                    }


                    System.out.println("User list has sent to the admin");

                }
                else if(UserCommand.startsWith("REMOVEUSER")){
                    String[] inp = UserCommand.split(":::");
                    if (inp.length != 2 || inp[1].equals("!")){
                        sendNotification("ERR", "");
                    }
                    else {
                        String usernameToDeleted = inp[1];
                        User olduser = db.searchbyName(usernameToDeleted);
                        if(!olduser.getUsername().equals("")){
                            sendNotification("SURE", "");
                            String answer = bufferedReader.readLine();
                            if(answer.equals("Y")){
                                db.deleteRowbyName(usernameToDeleted);
                                sendNotification("DELETED", "");
                            }
                            else{
                                sendNotification("CANCELED", "");
                            }

                        }
                        else{
                            sendNotification("NOUSER", "");
                        }
                    }

                }
                else if(UserCommand.startsWith("UPDATEUSER")){
                    String[] inp = UserCommand.split(":::");
                    if (inp.length != 2 || inp[1].equals("!")){
                        sendNotification("ERR", "");
                    }
                    else {
                        String usernameToUpdated = inp[1];
                        User olduser = db.searchbyName(usernameToUpdated);
                        if(!olduser.getUsername().equals("")){
                            sendNotification("ASK", "");
                            String answer = bufferedReader.readLine();
                            if (answer.equals("CANCEL")){

                            }
                            else{
                                String[] info = answer.split(":::");
                                String newinfo = bufferedReader.readLine();
                                if(newinfo.equals("CANCELED")){
                                    System.out.println();
                                    continue;
                                }
                                else{
                                    String[] in = newinfo.split(":::");
                                    System.out.println(db.updateinfo(olduser, info, in));
                                }

                            }
                        }
                        else{
                            sendNotification("NOUSER", "");
                        }
                    }

                }
                else if(UserCommand.startsWith("SEND")){
                    String protocol = UserCommand.substring(14);
                    String[] P = protocol.split(":::");
                    if(protocol.split(":::").length != 4){
                        sendNotification("ERROR", "");
                    }
                    else{
                        Message messagetoSend = new Message(P[0], P[1], P[2], P[3]);
                        User user = db.searchbyName(messagetoSend.getTo());
                        if(!user.getUsername().equals("")){
                            db.insertMessage(messagetoSend);
                            sendNotification("Message has sent to ", messagetoSend.getTo() + " !");
                        }
                        else{
                            sendNotification("There is no user with the username: ", messagetoSend.getTo());
                        }
                    }

            }
                else if(UserCommand.startsWith("MINBOX")){

                    ArrayList<Message> messages = db.messageQuery(true, UserCommand.substring(9));
                    for(Message m : messages){
                        sendNotification(m.getindex() + ":::"+
                                        m.getTitle() + ":::" +
                                        m.getMessage() + ":::" +
                                        m.getFrom() +":::" +
                                        m.getDate().toString()
                                , "");
                    }
                    bufferedWriter.write("");
                    bufferedWriter.newLine();
                    bufferedWriter.flush();
                }
                else if(UserCommand.startsWith("MOUTBOX")){
                    ArrayList<Message> messages = db.messageQuery(false, UserCommand.substring(10));
                    for(Message m : messages){
                        sendNotification(m.getindex() + ":::"+
                                        m.getTitle() + ":::" +
                                        m.getMessage() + ":::" +
                                        m.getTo() +":::" +
                                        m.getDate().toString()
                                , "");
                    }
                    bufferedWriter.write("");
                    bufferedWriter.newLine();
                    bufferedWriter.flush();
                }
                else if(UserCommand.startsWith("DEL")){
                    String[] info = UserCommand.split(":::");
                    if(info.length != 4){
                        sendNotification("ERR", "");
                    }
                    else{
                        Message m = new Message();
                        m.setIndex(Integer.parseInt(info[2]));
                        m.setMessage(info[3]);
                        System.out.println(info[3]);
                        if(info[1].equals("IN")){
                            Message message = db.messageQuerybyIndex(m, true);
                            if(!message.getTo().equals("")){
                                db.deleteMessage(info[2], true);
                                sendNotification("DEL", "");
                            }
                            else {
                                sendNotification("NOMESSAGE", "");
                            }
                        }
                        else{
                            Message message = db.messageQuerybyIndex(m, false);
                            if(!message.getFrom().equals("")){
                                db.deleteMessage(info[2], false);
                                sendNotification("DEL", "");
                            }
                            else {
                                sendNotification("NOMESSAGE", "");
                            }
                        }

                    }
                }
                else if(UserCommand.startsWith("CONTROL")){
                    String username = UserCommand.substring(10);
                    User user = db.searchbyName(username);
                    if(user.getUsername().equals(username)){
                        sendNotification("Y", "");
                    }
                    else{
                        sendNotification("N", "");
                    }

                }

            }
            catch (Exception e)
            {
                closeEverything(socket, bufferedReader, bufferedWriter);
                System.out.println("A socket is closed.");
                System.out.println(e);
                break;
            }
        }

    }

    public void sendNotification(String notification, String username) throws IOException{
        bufferedWriter.write(notification+ username);
        bufferedWriter.newLine();
        bufferedWriter.flush();
    }

    public ClientHandler(Socket s, Connection conn, DbFunctions dbFunctions) throws IOException
    {
        this.conn = conn;
        this.db = dbFunctions;
        this.socket = s;
        this.bufferedWriter = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()));
        this.bufferedReader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        clients.add(this);
    }

    public boolean[] checkUser(String username, String password) throws Exception{
        boolean[] flags = {false, false, false};
        User user = db.searchbyName(username);
        try
        {
            if (!user.getUsername().equals(""))
            {
                flags[0] = true;
                String inputPassword = user.getPassword();
                if (inputPassword.equals(password))
                {
                    flags[1] = true;
                    if(user.getAdmin().equals("t")){
                        flags[2] = true;
                    }
                    return flags;
                }
                else {
                    return flags;
                }

            }

        }
        catch (Exception e){
            System.out.println(e);
            return flags;
        }
        return flags;
    }

    public void closeEverything(Socket socket, BufferedReader bufferedReader, BufferedWriter bufferedWriter)
    {
        try {
            if(bufferedReader != null)
            {
                bufferedReader.close();
            }
            if (bufferedWriter != null)
            {
                bufferedWriter.close();
            }
            if(socket != null)
            {
                socket.close();
            }
        }
        catch (Exception e)
        {
            System.out.println(e);
        }
    }

    public void login(String userCommand) throws  Exception{
        String[] userinfo = userCommand.split(":::");
        if(userinfo.length != 3){
            sendNotification("Username or password is incorrect!", "");
        }
        else{
            boolean[] flags = checkUser(userinfo[1], userinfo[2]);
            if(!flags[0]){
                sendNotification("There is no user with username: ", userinfo[1]);
            }
            else if(!flags[1]){
                sendNotification("Wrong password for the account: ", userinfo[1]);
            }
            else{
                if (flags[2])
                {
                    sendNotification("You entered the server as admin ", userinfo[1]);
                }
                else {
                    sendNotification("You entered the server as user ", userinfo[1]);
                }
                User user = db.searchbyName(userinfo[1]);
                bufferedWriter.write(user.concatenateUserFields());
                bufferedWriter.newLine();
                bufferedWriter.flush();

            }
        }
    }


    static void close() throws IOException{
        for(ClientHandler client : clients){
            client.socket.close();
        }
    }
}
