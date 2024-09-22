import java.io.IOException;
import java.net.ServerSocket;
import java.security.PrivateKey;
import java.sql.Connection;
import java.sql.Statement;
import java.util.Scanner;

public class serverclose implements Runnable {
    private Connection conn;
    private ServerSocket serverSocket;
    @Override
    public void run() {
        Scanner scanner = new Scanner(System.in);
        String command = scanner.nextLine();
        while(!command.equals("CLOSE")){
            command = scanner.nextLine();
        }
        try{
            serverSocket.close();
        }
        catch (Exception e){
            System.out.println(e);
        }
    }

    serverclose(ServerSocket serverSocket, Connection conn){
        this.serverSocket = serverSocket;
        this.conn = conn;
    }
}