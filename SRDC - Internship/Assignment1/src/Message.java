import java.sql.Timestamp;
import java.util.Calendar;
import java.util.Date;

public class Message {
    private int index;
    private String message;
    private String to;
    private String from;
    private String title;
    private java.sql.Timestamp date;
    private boolean deletedFromSender;
    private boolean deletedFromReceiver;

    public int getindex(){
        return index;
    }

    public String getTitle(){
        return title;
    }
    public String getMessage(){
        return message;
    }
    public String getFrom(){
        return from;
    }

    public String getTo(){
        return to;
    }

    public Timestamp getDate(){
        return date;
    }
    public Message(String to, String title, String message, String from){
        this.to = to;
        this.title = title;
        this.message = message;
        this.from = from;
        Timestamp timestamp = new Timestamp(System.currentTimeMillis());
        date = timestamp;
    }
    public Message(int index, String to, String title, String message, String from, Timestamp date, boolean d1, boolean d2){
        this.index = index;
        this.deletedFromReceiver = d1;
        this.deletedFromSender = d2;
        this.to = to;
        this.title = title;
        this.message = message;
        this.from = from;
        this.date = date;
    }

    public Message() {
        this(1, "", "", "", "", null, false, false);
    }

    public void setMessage(String message) {
        this.message = message;
    }

    public void setIndex(int index) {
        this.index = index;
    }
}
