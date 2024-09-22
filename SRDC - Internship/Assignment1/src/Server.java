import javax.print.DocFlavor;
import java.io.*;
import java.net.ServerSocket;
import java.sql.Connection;
import java.net.*;
import java.sql.Statement;

public class Server
{
    DbFunctions db;
    Connection conn;

    public static void main(String[] args) {
        try
        {
            DbFunctions db = new DbFunctions("Assignment1", "postgres", "M.hbG6zx");
            Connection conn = db.getConn();
            db.setConn(conn);
            db.createUserTable();
            db.createMessageTable();
            User root = new User("root", "admin", "admin", "admin", "", "", "", "0-0-0", "y");
            db.insertUser(root);
            Server server = new Server(db, conn);
            server.begin();
        }
        catch (Exception e){
            System.out.println(e);
        }

    }

    public void begin() throws IOException{
        ServerSocket serversocket = new ServerSocket(9095);
        try
        {

            while(!serversocket.isClosed()) {
                serverclose serverClose = new serverclose(serversocket, conn);
                Thread thread = new Thread(serverClose);
                thread.start();
                Socket socket = serversocket.accept();
                System.out.println("New client has connected!");
                ClientHandler clientHandler = new ClientHandler(socket, conn, db);
                thread = new Thread(clientHandler);
                thread.start();
            }

        }
        finally {
            ClientHandler.close();
            System.out.println("Server is closed!");
        }
    }
    public Server(DbFunctions database, Connection c) {
                db = database;
                conn = c;
    }

}