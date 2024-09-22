package com.example.demo.Message;
import com.example.demo.Token;
import com.example.demo.User.User;
import com.example.demo.User.UserService;
import io.jsonwebtoken.Claims;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import java.util.List;
import java.util.Optional;

@CrossOrigin
@RestController
@RequestMapping(path = "/api/message")
public class MessageController {
    private final MessageService messageService;

    @Autowired
    private UserService userService;

    @Autowired
    public MessageController(MessageService messageService) {
        this.messageService = messageService;
    }

    @GetMapping("/inbox")
    public ResponseEntity<List<Message>> getInbox(@RequestHeader(value="Authorization") String acceptHeader, @RequestParam String username){
        Optional<User> u = getId(acceptHeader);
        if(u.isEmpty()){
            return new ResponseEntity<>(HttpStatus.NOT_FOUND);
        }
        return messageService.getInbox(username);
    }

    @GetMapping("/outbox")
    public ResponseEntity<List<Message>> getOutbox(@RequestHeader(value="Authorization") String acceptHeader, @RequestParam String username){ ////
        Optional<User> u = getId(acceptHeader);
        if(u.isEmpty()){
            return new ResponseEntity<>(HttpStatus.NOT_FOUND);
        }
        return messageService.getOutbox(username);
    }

    @PostMapping
    public ResponseEntity<Message> sendMessage(@RequestHeader(value="Authorization") String acceptHeader, @RequestBody Message username){
        Optional<User> u = getId(acceptHeader);
        if(u.isEmpty()){
            return new ResponseEntity<>(HttpStatus.NOT_FOUND);
        }
        return messageService.sendMessage(username);
    }

    @DeleteMapping(path = "/inbox")
    public ResponseEntity<Message> deleteInboxMessage(@RequestHeader(value="Authorization") String acceptHeader, @RequestParam Long Id, @RequestParam String receiver){
        Optional<User> u = getId(acceptHeader);
        if(u.isEmpty()){
           return new ResponseEntity<>(HttpStatus.NOT_FOUND);
        }
        messageService.deleteInboxMessage(Id, receiver);
        return new ResponseEntity<>(HttpStatus.NO_CONTENT);
    }

    @DeleteMapping(path = "/outbox")
    public ResponseEntity<Message> deleteOutboxMessage(@RequestHeader(value="Authorization") String acceptHeader, @RequestParam Long Id, @RequestParam String sender){
        Optional<User> u = getId(acceptHeader);
        if(u.isEmpty()){
            return new ResponseEntity<>(HttpStatus.NOT_FOUND);
        }
        messageService.deleteOutboxMessage(Id, sender);
        return new ResponseEntity<>(HttpStatus.NO_CONTENT);
    }

    public Optional<User> getId(String acceptHeader){
        Token t = new Token();
        Claims claims = t.DecodeToken(acceptHeader);
        int id = Integer.parseInt((String) claims.get("Id"));
        Long longid = (long) id;
        return userService.getById(longid);
    }

}
