package com.example.demo.User;

import com.example.demo.Token;
import io.jsonwebtoken.Claims;
import jakarta.transaction.Transactional;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Service;
import org.springframework.web.bind.annotation.GetMapping;

import java.time.LocalDate;
import java.util.List;
import java.util.Objects;
import java.util.Optional;

@Service
public class UserService {

    private final UserRepository userRepository;
    private final Token token;

    @Autowired
    public UserService(UserRepository userRepository){
        this.userRepository = userRepository;
        this.token = new Token();
    }


    @GetMapping
    public ResponseEntity<List<User>> getUsers(){
        return new ResponseEntity<>(userRepository.findAll(), HttpStatus.OK);
    }


    public ResponseEntity<User> getUserByUsername(String username){
        Optional<User> userOptional = userRepository.findUserByUsername(username);
        return userOptional.map(user -> new ResponseEntity<>(user, HttpStatus.OK)).orElseGet(() -> new ResponseEntity<>(HttpStatus.NOT_FOUND));
    }

    public ResponseEntity<User> addNewUser(User user) {
        Optional<User> userOptional = userRepository.findUserByEmail(user.getEmail());
        if(userOptional.isPresent()){
            return new ResponseEntity<>(HttpStatus.NOT_ACCEPTABLE); //406
        }
        Optional<User> userOptional2 = userRepository.findUserByUsername(user.getUsername());
        if(userOptional2.isPresent()){
            return new ResponseEntity<>(HttpStatus.METHOD_NOT_ALLOWED); //405
        }
        userRepository.save(user);
        return new ResponseEntity<>(user, HttpStatus.CREATED); // 201
    }

    public ResponseEntity<User> deleteUser(String username){
        if(username.equals("root")){
            return new ResponseEntity<>(HttpStatus.NOT_ACCEPTABLE); //406
        }
        Optional<User> user = userRepository.findUserByUsername(username);
        if(user.isPresent()){
            userRepository.deleteById(user.get().getId());
            return new ResponseEntity<>(HttpStatus.NO_CONTENT); //204
        }
        else{
            return new ResponseEntity<>(HttpStatus.METHOD_NOT_ALLOWED); // 405
        }
    }

    @Transactional
    public ResponseEntity<User> updateUser(Long userId, String oldusername, User newInfo){
        String newname = newInfo.getName();
        String newsurname = newInfo.getSurname();
        String newemail = newInfo.getEmail();
        String newusername = newInfo.getUsername();
        String newpassword = newInfo.getPassword();
        String newgender = newInfo.getGender();
        String newcity = newInfo.getCity();
        LocalDate dob = newInfo.getDateofBirth();
        boolean admin = newInfo.getAdmin();
        if(oldusername.equals("root")){
            return new ResponseEntity<>(HttpStatus.NOT_ACCEPTABLE); //406
        }
        if(userId == null){
            Optional<User> user = userRepository.findUserByUsername(oldusername);
            if(user.isPresent()){
                userId = user.get().getId();
            }
            else{
                throw new IllegalStateException("No user with provided username");
            }
        }

        User user = userRepository.findById(userId).orElseThrow(() -> new IllegalStateException("Account is not present"));
        if(newname != null && newname.length() > 0 && !Objects.equals(user.getName(), newname)){
            user.setName(newname);
        }

        if(newsurname != null && newsurname.length() > 0 && !Objects.equals(user.getSurname(), newsurname)){
            user.setSurname(newsurname);
        }

        if(newusername != null && newusername.length() > 0){
            Optional<User> studentOptional = userRepository.findUserByUsername(newusername);
            if(studentOptional.isPresent()){
                return new ResponseEntity<>(HttpStatus.METHOD_NOT_ALLOWED);
            }
            else{
                user.setUsername(newusername);
            }
        }

        if(newpassword != null && newpassword.length() > 0 && !Objects.equals(user.getPassword(), newpassword)){
            user.setPassword(newpassword);
        }

        if(newemail != null && newemail.length() > 0){
            Optional<User> studentOptional = userRepository.findUserByEmail(newemail);
            if(studentOptional.isPresent()){
                return new ResponseEntity<>(HttpStatus.METHOD_NOT_ALLOWED);
            }
            else{
                user.setEmail(newemail);
            }
        }

        if(newgender != null && newgender.length() > 0 && !Objects.equals(user.getGender(), newgender)){
            switch (newgender) {
                case "M", "B", "F" -> user.setGender(newgender);
                default -> throw new IllegalStateException("No gender like that.");
            }
        }

        if(newcity != null && newcity.length() > 0 && !Objects.equals(user.getCity(), newcity)){
            user.setCity(newcity);
        }

        if(dob != null){
                user.setDateofBirth(dob);
            }
            else{
                throw new IllegalStateException("Wrong date syntax");
            }

        user.setAdmin(admin);
        return new ResponseEntity<>(user, HttpStatus.OK);
    }

    public ResponseEntity<User> loginUser(String username, String password){
        Optional<User> user =  userRepository.findUserByUsernameAndPassword(username, password);
        if(user.isPresent()){
            String role;
            if(user.get().getAdmin()){
                role = "admin";
            }
            else{
                role = "user";
            }
            String userToken = token.CreateToken(user.get().getUsername(), role, user.get().getPassword(),user.get().getId().toString());
            User us = new User(userToken+ ":::" +role, "", "", "");
            return new ResponseEntity<>(us, HttpStatus.OK);
        }
        else{
            return new ResponseEntity<>(HttpStatus.NOT_FOUND);

        }
    }

    public Optional<User> getById(Long id){
        return userRepository.findById(id);
    }

    public Optional<User> getId(String acceptHeader){
        Token t = new Token();
        Claims claims = t.DecodeToken(acceptHeader);
        int id = Integer.parseInt((String) claims.get("Id"));
        Long longid = (long) id;
        return getById(longid);
    }
}
