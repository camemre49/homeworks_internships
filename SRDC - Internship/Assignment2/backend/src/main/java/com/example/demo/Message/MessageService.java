package com.example.demo.Message;

import com.example.demo.User.User;
import com.example.demo.User.UserRepository;
import jakarta.transaction.Transactional;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.Optional;

@Service
public class MessageService {
    private final MessageRepository messageRepository;
    private final UserRepository userRepository;

    @Autowired
    public MessageService(MessageRepository messageRepository, UserRepository userRepository){
        this.messageRepository = messageRepository;
        this.userRepository = userRepository;
    }

    public List<Message> getMessages(){
        return messageRepository.findAll();
    }

    public ResponseEntity<List<Message>> getInbox(String receivername){

        if(!userRepository.existsUserByUsername(receivername)){
            throw new IllegalStateException("User is not exists.");
        }
        return new ResponseEntity<>(messageRepository.findMessageByReceiverAndDeletedFromReceiver(receivername, false), HttpStatus.OK);
    }

    public ResponseEntity<List<Message>> getOutbox(String sendername){
        if(!userRepository.existsUserByUsername(sendername)){
            throw new IllegalStateException("User is not exists.");
        }
        return new ResponseEntity<>(messageRepository.findMessageBySenderAndDeletedFromSender(sendername, false), HttpStatus.OK);
    }

    public ResponseEntity<Message> sendMessage(Message message){
        Optional<User> receiver = userRepository.findUserByUsername(message.getReceiver());
        if(receiver.isPresent()){
            messageRepository.save(message);
            return new ResponseEntity<>(HttpStatus.OK);
        }
        else{
            return new ResponseEntity<>(HttpStatus.NOT_FOUND);
        }
    }

    @Transactional
    public void deleteInboxMessage(Long messageId, String receiver) {
        Optional<Message> message = messageRepository.findMessageByIdAndReceiverAndDeletedFromReceiver(messageId, receiver, false);
        if(message.isPresent()){
            message.get().deletedFromReceiver = true;
        }
        else{
            throw new IllegalStateException("Message is not exists.");
        }
    }

    @Transactional
    public void deleteOutboxMessage(Long messageId, String sender) {
        Optional<Message> message = messageRepository.findMessageByIdAndSenderAndDeletedFromSender(messageId, sender, false);
        if(message.isPresent()){
            message.get().deletedFromSender = true;
        }
        else{
            throw new IllegalStateException("Message is not exists.");
        }
    }
}
