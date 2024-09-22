package com.example.demo;
import io.jsonwebtoken.Claims;
import io.jsonwebtoken.Jwts;
import io.jsonwebtoken.SignatureAlgorithm;
import io.jsonwebtoken.security.Keys;
import java.security.Key;
import java.time.temporal.ChronoUnit;
import java.util.Base64;
import java.security.Key;
import java.util.Date;
import java.util.UUID;

public class Token {
    private static final Key key =  Keys.secretKeyFor(SignatureAlgorithm.HS256);

    public String CreateToken(String username, String role, String password, String id){
        Date now = new Date();
        Date expiration = new Date(now.getTime() + 3600000); // Token will expire in 1 hour

        Claims claims = Jwts.claims();
        claims.setSubject(username); // Set the subject (e.g., username)
        claims.put("role", role); // Add additional claims
        claims.put("password", password);
        claims.put("Id", id);

        String token = Jwts.builder()
                .setClaims(claims)
                .setIssuedAt(now)
                .setExpiration(expiration)
                .signWith(key, SignatureAlgorithm.HS256)
                .compact();
        return token;
    }

    public Claims DecodeToken(String token){
        Claims claims = Jwts.parserBuilder()
                .setSigningKey(key)
                .build()
                .parseClaimsJws(token)
                .getBody();
        return claims;
    }



}
