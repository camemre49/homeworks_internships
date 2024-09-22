package com.example.demo.User;
import org.springframework.boot.CommandLineRunner;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

import java.util.List;

@Configuration
public class UserConfiguration {

    @Bean
    CommandLineRunner commandLineRunner(UserRepository repository){
        return args -> {User root = new User(0, "root", "admin", "root", "admin", "", "admin.com", "earth", null, true);
        repository.saveAll(
                List.of(root)
            );
        };

    }

}
