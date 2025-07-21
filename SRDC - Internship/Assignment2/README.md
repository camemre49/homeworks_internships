# Advanced Email App – Spring Boot + PostgreSQL + Vanilla JS

This is an advanced email application built using:
- **Spring Boot** (backend)
- **Vanilla JavaScript, HTML, and CSS** (frontend)
- **PostgreSQL** (as the database)
- **Maven** (as the build tool)

---

## Prerequisites

- Java 11+
- Maven
- PostgreSQL installed and running

---

## Database Setup (PostgreSQL)

Open your terminal and connect to PostgreSQL:

```bash
psql -U postgres
```

Then run the following SQL commands to create the database and user:

```sql
-- Create the database
CREATE DATABASE messaging;

-- Create a user (if not already created)
CREATE USER mailuser WITH PASSWORD 'mailpassword';

-- Connect to the new database
\c messaging

-- Grant privileges
GRANT ALL PRIVILEGES ON DATABASE messaging TO mailuser;
GRANT USAGE ON SCHEMA public TO mailuser;
GRANT CREATE ON SCHEMA public TO mailuser;
```

> Make sure PostgreSQL is listening on port 5432 (default). You can configure postgresql if needed in `application.properties`.

---

##  Run the App

### Compile and Package

```bash
mvn clean install -DskipTests
```

### Run the Application

```bash
java -jar target/demo-0.0.1-SNAPSHOT.jar
```

---

##  Frontend

The frontend is built with vanilla HTML/CSS/JS. You can use login.html to use the application.

## Usage
To connect as the root user:

```bash
LOGIN
root # username
admin # password
```

---

## Notes

- `spring.jpa.hibernate.ddl-auto=create-drop` drops and recreates the schema every time the app starts. Change it to `update` or `none` in production.
- This guide assumes PostgreSQL is running locally. Update connection settings if hosted elsewhere.

---