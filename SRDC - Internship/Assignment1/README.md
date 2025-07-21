# Java Socket Programming Email Application

This project is a socket programming assignment that implements a simple email system in Java. It consists of a server and clients communicating over TCP sockets, with PostgreSQL as database.

---

##  Prerequisites

- Java 11 or later installed  
- PostgreSQL installed and running  

---

## PostgreSQL Setup

### Change PostgreSQL url (hardcoded as localhost:5432 in this app) if necessary.

### DB and user setup:

```bash
sudo -u postgres psql
CREATE USER mailuser WITH PASSWORD 'mailpassword';
CREATE DATABASE maildb;
\c maildb
GRANT ALL PRIVILEGES ON DATABASE maildb TO mailuser;
GRANT USAGE ON SCHEMA public TO mailuser;
GRANT CREATE ON SCHEMA public TO mailuser;
\q
```
---

## Compile and Run

```bash
javac -cp .:postgresql-42.7.7.jar Server.java
java -cp .:postgresql-42.7.7.jar Server
```

You should see an output like:
```bash
Connection Established.
Table exists.
Table exists.
root is already registered.
```

### Client Connection
You can connect with multiple clients.

```bash
javac Client.java
java Client
```

## Usage
To connect as the root user:

```bash
LOGIN
root # username
admin # password
```

Write:
```bash
HELP # To see the currently available commands.
```

