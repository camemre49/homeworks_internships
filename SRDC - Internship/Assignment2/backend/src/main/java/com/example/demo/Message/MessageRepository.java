package com.example.demo.Message;

import org.springframework.data.jpa.repository.JpaRepository;

import java.util.List;
import java.util.Optional;

public interface MessageRepository extends JpaRepository<Message, Long> {
    public List<Message> findMessageByReceiverAndDeletedFromReceiver(String receivername, boolean deleted);
    public List<Message> findMessageBySenderAndDeletedFromSender(String senderName, boolean deleted);
    public Optional<Message> findMessageByIdAndReceiverAndDeletedFromReceiver(Long messageId, String receiver, boolean deleted);
    public Optional<Message> findMessageByIdAndSenderAndDeletedFromSender(Long messageId, String sender, boolean deleted);

}
