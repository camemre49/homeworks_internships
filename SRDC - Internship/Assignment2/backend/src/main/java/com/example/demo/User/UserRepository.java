package com.example.demo.User;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.Optional;

@Repository
public interface UserRepository extends JpaRepository<User, Long> {

    // @Query("SELECT * FROM Student s WHERE s.email = ?1")
    Optional<User> findUserByEmail(String email);
    Optional<User> findUserByUsername(String username);
    boolean existsUserByUsername(String username);

    Optional<User> findUserByUsernameAndPassword(String username, String password);

}
