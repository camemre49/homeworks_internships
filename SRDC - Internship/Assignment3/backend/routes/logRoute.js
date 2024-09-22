const express = require("express");
const logRoute = express.Router();
const cors = require("cors");
const jwt = require("jsonwebtoken");
const secretKey = require("../config");
const User = require("../models/User");
const ActiveTokens = require("../models/activeTokens");
const Log = require("../models/Log");

var whitelist = ["http://localhost:4200", "http://localhost:4000", "http://localhost:62085"];
var corsOptionsDelegate = function (req, callback) {
  var corsOptions;
  if (whitelist.indexOf(req.header("Origin")) !== -1) {
    corsOptions = {
      origin: "*",
      methods: "GET,HEAD,PUT,PATCH,POST,DELETE",
    };
  } else {
    corsOptions = { origin: false }; // disable CORS for this request
  }
  callback(null, corsOptions);
};


logRoute
  .route("/", cors(corsOptionsDelegate))
  .get(async (req, res, next) => {
    let route = "http://localhost:4000/api/log"
    const token = req.headers["authorization"];
    const response = await control(token);
    if(response["value"] === -1){
      await Log.create(createLog("TOKEN INVALID", "TOKEN INVALID", req.ip, req.headers["user-agent"], "GET", 403, route))
      return res.status(403).json({ message: "Invalid token." });
    }
    if (response["value"] === 0) {
      await Log.create(createLog("NOT REGISTERED", response["decodedToken"].username, req.ip, req.headers["user-agent"], "GET", 404, route))
      return res.status(406).json({ message: "User not found!" });
    }
    if (response["value"] === 1) {
      await Log.create(createLog(response["decodedToken"].userId, response["decodedToken"].username, req.ip, req.headers["user-agent"], "GET", 401, route))
      return res.status(401).json({ message: "Not Authorized" });
    }
    const sortCriteria = {};
    let items;
    let totalLog;
    let totalPages
    let limit = parseInt(req.headers["limit"]) || 20;
    let page = parseInt(req.headers["page"]) || 1;
    if(req.headers.query){
      const query = {};
      query["username"] = { $regex: req.headers.query, $options: "i" };
      totalLog = await Log.countDocuments(query);
      totalPages = Math.ceil(totalLog / limit);
      const skip = (page - 1) * limit;
      if(req.headers["sort"]){
        items = await Log.find(query).sort(sortCriteria).skip(skip).limit(limit)
      }
      else{
        items = await Log.find(query).skip(skip).limit(limit)
      }
    }
    else{
      if(req.headers["sort"] === ""){
        items = await Log.find().skip(skip).limit(limit);
      }
      else{
        totalLog = await Log.countDocuments();
        totalPages = Math.ceil(totalLog / limit);
        const skip = (page - 1) * limit;
        sortCriteria[req.headers["sort"]] = parseInt(req.headers["reverse"]);
        items = await Log.find().sort(sortCriteria).skip(skip).limit(limit);
      }
    }
    res.status(200).json({
      currentPage: page,
      totalPages: totalPages,
      totalItems: totalLog,
      logs: items
    });
  });

async function control(token) {
  try{
    const decodedToken = jwt.verify(token, secretKey);
    const user = await User.findById(decodedToken.userId);
    const isActive = await ActiveTokens.findOne({token: token})
    if(!user || !isActive){
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
  return {
    "username": username,
    "date": new Date(),
    "userId": userId,
    "userIp": userIp,
    "userAgent": userBrowser,
    "method": method,
    "responseStatus": status,
    "route": route
  };
}



module.exports = logRoute;
