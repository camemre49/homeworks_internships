package com.example.demo.Message;
import jakarta.persistence.*;

import java.sql.Timestamp;

@Entity
@Table (name = "Messages")
public class Message {
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private Long id;

    private String receiver;
    private String title;
    private String message;
    private String sender;
    private Timestamp date;
    boolean deletedFromSender;
    boolean deletedFromReceiver;

    public Message(String towho, String title, String message, String fromwho) {
        this.receiver = towho;
        this.title = title;
        this.message = message;
        this.sender = fromwho;
        deletedFromReceiver = false;
        deletedFromSender = false;
        new Timestamp(System.currentTimeMillis());
        date = new Timestamp(System.currentTimeMillis());
    }

    public Message(String receiver) {
        this.receiver = receiver;
    }

    public Long getId() {
        return id;
    }

    public void setId(Long id) {
        this.id = id;
    }

    public String getReceiver() {
        return receiver;
    }

    public void setReceiver(String receiver) {
        this.receiver = receiver;
    }

    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public String getMessage() {
        return message;
    }

    public void setMessage(String message) {
        this.message = message;
    }

    public String getSender() {
        return sender;
    }

    public void setSender(String sender) {
        this.sender = sender;
    }

    public Timestamp getDate() {
        return date;
    }

    public void setDate(Timestamp date) {
        this.date = date;
    }

    public boolean isDeletedFromSender() {
        return deletedFromSender;
    }

    public void setDeletedFromSender(boolean deletedfromsender) {
        this.deletedFromSender = deletedfromsender;
    }

    public boolean isDeletedFromReceiver() {
        return deletedFromReceiver;
    }

    public void setDeletedFromReceiver(boolean deletedfromreceiver) {
        this.deletedFromReceiver = deletedfromreceiver;
    }

    public Message() {
    }

    @Override
    public String toString() {
        return "Message{" +
                "id=" + id +
                ", receiver='" + receiver + '\'' +
                ", title='" + title + '\'' +
                ", message='" + message + '\'' +
                ", sender='" + sender + '\'' +
                ", date=" + date.toString().split("T")[0]+
                ", time=" + date.toString().split("T")[1] +
                "}\n";
    }
}