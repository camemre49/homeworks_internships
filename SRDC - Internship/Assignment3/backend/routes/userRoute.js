const express = require("express");
const app = express();
const userRoute = express.Router();
const cors = require("cors");
const bcrypt = require('bcrypt');
const crypto = require('crypto')
const jwt = require('jsonwebtoken')
const secretKey = require('./../config');

var whitelist = ["http://localhost:4200", "http://localhost:4000", "http://localhost:62085"];
var corsOptionsDelegate = function (req, callback) {
  var corsOptions;
  if (whitelist.indexOf(req.header("Origin")) !== -1) {
    corsOptions = {
      origin: "*",
      methods: "GET,HEAD,PUT,PATCH,POST,DELETE",
    };
  } else {
    corsOptions = { origin: false };
  }
  callback(null, corsOptions);
};


// User model
let User = require("../models/User");
let ActiveTokens = require("../models/activeTokens")
let Log = require("../models/Log");
const {log} = require("winston");
let url = "http://localhost:4000/api/user"

async function createUser() {
  try {
    let defaultAdmin = {
      username: "root",
      password: Buffer.from("admin").toString('base64'),
      name: "root",
      surname: "root",
      email: "root.com",
      city: "earth",
      dateOfBirth: new Date(),
      admin: true
    };
    await User.create(defaultAdmin);
    console.log('Root admin created.');
  } catch (err) {
    // Handle the error here
    console.log("Root admin exists.");
  }
}
createUser();


userRoute.route("/login").post(async (req, res, next) => {
  let route = url + "/login"
    try {
    const username = req.body.username;
    const password = req.body.password;

    const user = await User.findOne({username: username });
    if (!user) {
      await Log.create(createLog(req.body.username, "NOT REGISTERED", req.ip, req.headers["user-agent"], "POST", 404, route))
      return res.status(404).json({ error: 'User not found!' });
    }
    else if(Buffer.from(user.password, 'base64').toString('utf-8') !== password){
      await Log.create(createLog(req.body._id, req.body.username, req.ip, req.headers["user-agent"], "POST", 401, route))
      return res.status(401).json({error: 'Password Wrong!'});
    }
      const options = {
        expiresIn: '1h', // Token will expire in 1 hour
      };

    const payload = {
      userId: user._id,
      username: user.username,
      role: user.admin,
    };
    const token = jwt.sign(payload, secretKey, options);

    let newToken = {
      token: token
    }
    try{
      await ActiveTokens.create(newToken);
    }
    catch (err){
      console.log(err)
    }
    await Log.create(createLog(user._id, user.username, req.ip, req.headers["user-agent"], "POST", 200, route))
    res.setHeader("Content-Type", "application/json")
    user.username = token;
    user.password = "";
    res.json({
      data:user,
      }
    )
    res.end();
  } catch (error) {
    console.error('Error during login:', error.message);
      await Log.create(createLog(req.body._id, req.body.username, req.ip, req.headers["user-agent"], "POST", 500, route))
    res.status(500).json({ error: 'Server error' });
  }
});



// Add User
userRoute.route("/create").post(async (req, res, next) =>
{
  let route = url + "/create"
  const token = req.headers["authorization"];
  const response = await control(token);
  if(response["value"] === -1){
    await Log.create(createLog("TOKEN INVALID", "TOKEN INVALID", req.ip, req.headers["user-agent"], "POST", 403, route))
    return res.status(403).json({ message: "Invalid token." });
  }
  if (response["value"] === 0) {
    await Log.create(createLog("NOT REGISTERED", response["decodedToken"].username, req.ip, req.headers["user-agent"], "POST", 404, route))
    return res.status(406).json({ message: "User not found!" });
  }
  if (response["value"] === 1) {
    await Log.create(createLog(response["decodedToken"].userId, response["decodedToken"].username, req.ip, req.headers["user-agent"], "POST", 401, route))
    return res.status(401).json({ message: "Not Authorized" });
  }
  req.body.password = Buffer.from(req.body.password).toString('base64')
  await Log.create(createLog(response["decodedToken"].userId, response["decodedToken"].username, req.ip, req.headers["user-agent"], "POST", 200, route))
  await User.create(req.body)
    .then((result) => {
      res.json({
        data: result,
        message: "Data successfully added!",
        status: 200,
      });
    })
    .catch((err) => {
      return res.status(405).json({ message: "Username or email is used." });
    });
});


userRoute
  .route("/", cors(corsOptionsDelegate))
  .get(async (req, res, next) => {
    let route = url + "/" + req.params.query;
    const token = req.headers["authorization"];
    const response = await control(token);
    if(response["value"] === -1){
      await Log.create(createLog("TOKEN INVALID", "TOKEN INVALID", req.ip, req.headers["user-agent"], "Get", 403, route))
      return res.status(403).json({ message: "Invalid token." });
    }
    if (response["value"] === 0) {
      await Log.create(createLog("NOT REGISTERED", response["decodedToken"].username, req.ip, req.headers["user-agent"], "GET", 404, route))
      return res.status(406).json({ message: "User not found!" });
    }
    if(req.headers.query){
      const query = {};
      query["username"] = { $regex: req.headers.query, $options: "i" };
      await User.find(query).limit(10).then(
        (result) => {
          res.writeHead(201, { "Content-Type": "application/json" });
          const usernames = result.map((user) => user["username"]);
          res.end(JSON.stringify(usernames));
        })
        .catch((err) => {
          return next(err);
        });
    }
    else {
      if (response["value"] === 1) {
        await Log.create(createLog(response["decodedToken"].userId, response["decodedToken"].username, req.ip, req.headers["user-agent"], "GET", 401, route))
        return res.status(401).json({ message: "Not Authorized" });
      }
      let limit = parseInt(req.headers["limit"]) || 10;
      let page = parseInt(req.headers["page"]) || 1;
      const totalUsers = await User.countDocuments();
      const totalPages = Math.ceil(totalUsers / limit);
      const skip = (page - 1) * limit;
      const sortCriteria = {};
      let items;
      if(req.headers["sort"] === ""){
        items = await User.find().skip(skip).limit(limit);
      }
      else{
        sortCriteria[req.headers["sort"]] = parseInt(req.headers["reverse"]);
        items = await User.find().sort(sortCriteria).skip(skip).limit(limit);
      }
      res.status(200).json({
        currentPage: page,
        totalPages: totalPages,
        totalItems: totalUsers,
        users: items
      });
    }
  });

userRoute
  .route("/loginCheck", cors(corsOptionsDelegate))
  .get(async (req, res, next) => {
    let route = url + "/loginCheck"
    const token = req.headers["authorization"];
    const response = await control(token);
    if(response["value"] === -1){
      await Log.create(createLog("TOKEN INVALID", "TOKEN INVALID", req.ip, req.headers["user-agent"], "POST", 403, route))
      return res.status(403).json({ message: "Invalid token." });
    }
    if (response["value"] === 0) {
      await Log.create(createLog("NOT REGISTERED", response["decodedToken"].username, req.ip, req.headers["user-agent"], "GET", 404, route))
      return res.status(406).json({ message: "User not found!" });
    }
    try{
      let user = await User.findById(response["decodedToken"].userId)
      await Log.create(createLog(response["decodedToken"].userId, response["decodedToken"].username, req.ip, req.headers["user-agent"], "GET", 200, route))
      return res.status(200).json({username: user.username, bool: user.admin})
    }
    catch(error){
      return res.status(404).json({})
    }
  });

// Update employee
userRoute.route("/update/:id").put(async (req, res, next) => {
  let user = await User.findById(req.body._id);
  if(!req.body.username){
    req.body.username = user.username;
  }
  if(!req.body.password){
    req.body.password = user.password;
  }
  else{
    req.body.password = Buffer.from(req.body.password).toString('base64')
  }
  if(!req.body.name){
    req.body.name = user.username;
  }
  if(!req.body.surname){
    req.body.surname = user.surname;
  }
  if(!req.body.email){
    req.body.email = user.email;
  }
  if(!req.body.admin){
    req.body.admin = user.admin;
  }
  if(!req.body.city){
    req.body.city = user.city
  }
  if(!req.body.dateOfBirth){
    req.body.dateOfBirth = user.dateOfBirth;
  }
  if(!req.body.gender){
    req.body.gender = user.gender;
  }
  const token = req.headers["authorization"];
  const response = await control(token);
  let route = url + "/update/"+response["decodedToken"].userId
  if(response["value"] === -1){
    await Log.create(createLog("TOKEN INVALID", "TOKEN INVALID", req.ip, req.headers["user-agent"], "POST", 403, route))
    return res.status(403).json({ message: "Invalid token." });
  }
  if (response["value"] === 0) {
    await Log.create(createLog("NOT REGISTERED", response["decodedToken"].username, req.ip, req.headers["user-agent"], "PUT", 404, route))
    return res.status(406).json({ message: "User not found!" });
  }
  if (response["value"] === 1) {
    await Log.create(createLog(response["decodedToken"].userId, response["decodedToken"].username, req.ip, req.headers["user-agent"], "PUT", 401, route))
    return res.status(401).json({ message: "Not Authorized" });
  }
  await Log.create(createLog(response["decodedToken"].userId, response["decodedToken"].username, req.ip, req.headers["user-agent"], "PUT", 200, route))
  await User.findByIdAndUpdate(req.params.id,
  {$set: req.body},
  { new: true })
    .then((result) => {
      result.password= "";
      res.json({
        data: result,
        msg: "Data successfully updated.",
      });
    })
    .catch((err) => {
      return res.status(405).json({ message: "Username or email is used." });
    });
});

// Delete employee
userRoute.route("/delete/:username").delete(async (req, res) => {
  let route = url + "/delete/" + req.params.username;
  const token = req.headers["authorization"];
  const response = await control(token);
  if(response["value"] === -1){
    await Log.create(createLog("TOKEN INVALID", "TOKEN INVALID", req.ip, req.headers["user-agent"], "POST", 403, route))
    return res.status(403).json({ message: "Invalid token." });
  }
  if (response["value"] === 0) {
    await Log.create(createLog("NOT REGISTERED", response["decodedToken"].username, req.ip, req.headers["user-agent"], "DELETE", 404, route))
    return res.status(406).json({ message: "User not found!" });
  }
  if (response["value"] === 1) {
    await Log.create(createLog(response["decodedToken"].userId, response["decodedToken"].username, req.ip, req.headers["user-agent"], "DELETE", 401, route))
    return res.status(401).json({ message: "Not Authorized" });
  }
  try {
    const deletedUser = await User.findOneAndDelete({ username: req.params.username });

    if (!deletedUser) {
      await Log.create(createLog(response["decodedToken"].userId, response["decodedToken"].username, req.ip, req.headers["user-agent"], "DELETE", 404, route))
      return res.status(404).json({ message: `User with username '${req.params.username}' not found.` });
    }
    await Log.create(createLog(response["decodedToken"].userId, response["decodedToken"].username, req.ip, req.headers["user-agent"], "DELETE", 200, route))
    return res.status(200).json({ message: `User with username '${req.params.username}' deleted.`, deletedUser });
  } catch (error) {
    await Log.create(createLog(response["decodedToken"].userId, response["decodedToken"].username, req.ip, req.headers["user-agent"], "DELETE", 500, route))
    return res.status(500).json({ message: 'Internal server error.' });
  }
});

async function control(token) {
  try{
    const decodedToken = jwt.verify(token, secretKey);
    const user = await User.findById(decodedToken.userId);
    const isActive = await ActiveTokens.find({token: token})
    if(!(user && isActive.length > 0)){
      return {value: 0,
              decodedToken: decodedToken};
    }
    if(user.admin !== true){
      return {value: 1,
        decodedToken: decodedToken};
    }
    return {value: 5, decodedToken: decodedToken}
  }
  catch(error) {
    return {value: -1,
      decodedToken: ""};
  }
}

function createLog(userId, username, userIp, userBrowser, method, status, route){
  let log = {
    "username": username,
    "date": new Date(),
    "userId": userId,
    "userIp": userIp,
    "userAgent": userBrowser,
    "method": method,
    "responseStatus": status,
    "route": route
  };
  return log;
}

userRoute.route("/logout").get(async (req, res) => {
  let route = url + "/logout";
  const token = req.headers["authorization"];
  const response = await control(token);
  await Log.create(createLog(response["decodedToken"].userId ? response["decodedToken"].userId : "INVALID TOKEN" , response["decodedToken"].username ? response["decodedToken"].username : "INVALID TOKEN", req.ip, req.headers["user-agent"], "GET", 200, route))
  await ActiveTokens.deleteMany({token: token})
  return res.status(200).json({message: "Token deleted."});
})


module.exports = userRoute;
