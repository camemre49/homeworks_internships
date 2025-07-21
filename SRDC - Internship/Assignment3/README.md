# 📧 MEAN Stack Email App with JWT Authentication

This project is a full-stack Email Application built using the **MEAN stack** (MongoDB, Express.js, Angular, Node.js) with **JSON Web Token (JWT)** based authentication. It is a more advanced one of the previous assignments.

---

## Tech Stack

- **Frontend**: Angular, HTML, CSS, TypeScript
- **Backend**: Node.js, Express.js
- **Database**: MongoDB

---

## Prerequisites

- Node.js (v18+)
- MongoDB installed locally or using MongoDB Atlas
- Angular CLI (`npm install -g @angular/cli`)

---

## Setup Instructions

### Set Up Backend (Express + MongoDB)

```bash
cd backend
npm install
node index.js
```

### Set Up Frontend (Angular)

```bash
cd ../frontend
npm install
ng serve
```

The frontend will run on:  
`http://localhost:4200`

---

## Usage
To connect as the root user:

```bash
LOGIN
root # username
admin # password
```

---

## Notes

- Make sure MongoDB is running locally before starting the backend.
- If using MongoDB Atlas, replace `MONGODB_URI` accordingly in `index.js`.

