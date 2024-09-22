package com.example.demo.Message;
import com.example.demo.Message.Message;
import com.example.demo.Message.MessageRepository;
import com.example.demo.Message.MessageService;
import org.springframework.boot.CommandLineRunner;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

import java.util.List;

@Configuration
public class MessageConfiguration {

    @Bean
    CommandLineRunner messageline(MessageRepository repository){
        return args -> {Message test = new Message("root" , "123", "Hello", "nobody");
            repository.saveAll(
                    List.of(test)
            );
        };

    }

}
