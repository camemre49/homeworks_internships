package com.example.demo.User;

import com.example.demo.Token;
import io.jsonwebtoken.Claims;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.List;
import java.util.Optional;

@CrossOrigin
@RestController
@RequestMapping(path = "/api/user")
public class UserController {
    private final UserService userService;

    @Autowired
    public UserController(UserService userService) {
        this.userService = userService;
    }

    @GetMapping()
    public ResponseEntity<List<User>> getUsers(@RequestHeader(value="Authorization") String acceptHeader){
        Optional<User> u;
        try{
            u = userService.getId(acceptHeader);
        }
        catch(Exception e) {
            return new ResponseEntity<>(HttpStatus.UNAUTHORIZED);
        }
        if(u.isEmpty()){
            return new ResponseEntity<>(HttpStatus.NOT_FOUND);
        }
        if(!u.get().getAdmin()){
            return new ResponseEntity<>(HttpStatus.METHOD_NOT_ALLOWED);
        }
        return userService.getUsers();
    }

    @GetMapping(path ="/")
    public ResponseEntity<User> getUserByUsername(@RequestHeader(value="Authorization") String acceptHeader,@RequestParam("username") String username){
        Optional<User> u = userService.getId(acceptHeader);
        if(u.isEmpty()){
            return new ResponseEntity<>(HttpStatus.NOT_FOUND);
        }
        return userService.getUserByUsername(username);
    }

    @PostMapping()
    public ResponseEntity<User> registerNewUser(@RequestHeader(value="Authorization") String acceptHeader, @RequestBody User user){
        Optional<User> u = userService.getId(acceptHeader);
        if(u.isEmpty()){
            return new ResponseEntity<>(HttpStatus.NOT_FOUND);
        }
        if(!u.get().getAdmin()){
            return new ResponseEntity<>(HttpStatus.METHOD_NOT_ALLOWED);
        }
        return userService.addNewUser(user);

    }

    @DeleteMapping()
    public ResponseEntity<User> deleteUser(@RequestHeader(value="Authorization") String acceptHeader, @RequestParam("username") String username){
        Optional<User> u = userService.getId(acceptHeader);
        if(u.isEmpty()){
            return new ResponseEntity<>(HttpStatus.NOT_FOUND);
        }
        if(!u.get().getAdmin()){
            return new ResponseEntity<>(HttpStatus.METHOD_NOT_ALLOWED);
        }
        return userService.deleteUser(username);
    }

    @PutMapping(path = "/{username}")
    public ResponseEntity<User> updateUserByUsername(@RequestHeader(value="Authorization") String acceptHeader, @PathVariable("username") String oldusername,
                                     @RequestBody User user){
        Optional<User> u = userService.getId(acceptHeader);
        if(u.isEmpty()){
            return new ResponseEntity<>(HttpStatus.NOT_FOUND);
        }
        if(!u.get().getAdmin()){
            return new ResponseEntity<>(HttpStatus.METHOD_NOT_ALLOWED);
        }
        return userService.updateUser(null, oldusername, user);
    }

    @PostMapping(path = "/login")
    public ResponseEntity<User> userLogin(@RequestBody User user){
        return userService.loginUser(user.getUsername(), user.getPassword());
    }

    @GetMapping(path = "/control")
    public ResponseEntity<User> controlUser(@RequestHeader(value="Authorization") String acceptHeader){
        if(acceptHeader.equals(" ")){
            return new ResponseEntity<>(HttpStatus.NOT_FOUND);
        }
        else{
            Token t = new Token();
            Claims claims = t.DecodeToken(acceptHeader);
            return new ResponseEntity<>(new User(claims.getSubject(), (String) claims.get("password"),(String) claims.get("role"),""), HttpStatus.OK);
        }
    }

}
