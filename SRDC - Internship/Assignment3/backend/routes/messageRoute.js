const express = require("express");
const messageRoute = express.Router();
const cors = require("cors");

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

const secretKey = require('./../config');
let Message = require("../models/Message");
let User = require("../models/User")
const jwt = require("jsonwebtoken");
let Log = require("../models/Log");
const ActiveTokens = require("../models/activeTokens");
let url = "http://localhost:4000/api/message"
messageRoute.route("/send").post(async (req, res, next) =>
{
  let route = url + "/send"
  const token = req.headers["authorization"];
  const response = await control(token);
  if(response["value"] === -1){
    await Log.create(createLog("TOKEN INVALID", "TOKEN INVALID", req.ip, req.headers["user-agent"], "POST", 403, route))
    return res.status(403).json({ message: "Invalid token." });
  }
  if (response['value'] === 0) {
    await Log.create(createLog("NOT REGISTERED", response["decodedToken"].username, req.ip, req.headers["user-agent"], "POST", 404, route))
    return res.status(406).json({ message: "User not found!" });
  }
  let receiver = req.body.receiver;
  const user = await User.findOne({username: receiver });
  if(!user){
    await Log.create(createLog(response["decodedToken"].userId, response["decodedToken"].username, req.ip, req.headers["user-agent"], "POST", 404, route))
    return res.status(404).json({ error: 'User not found!' });
  }
  await Log.create(createLog(response["decodedToken"].userId, response["decodedToken"].username, req.ip, req.headers["user-agent"], "POST", 200, route))
  await Message.create(req.body)
    .then((result) => {
      res.json({
        data: result,
        message: "Data successfully added!",
        status: 200,
      });
    })
    .catch((err) => {
      return next(err);
    });
});

messageRoute
  .route("/:inOrOut/:username", cors(corsOptionsDelegate))
  .get(async (req, res) => {
    let route = url + "/" + req.params.inOurOut + "/" + req.params.username;
    const token = req.headers["authorization"];
    const response = await control(token);
    if(response["value"] === -1){
      await Log.create(createLog("TOKEN INVALID", "TOKEN INVALID", req.ip, req.headers["user-agent"], "GET", 403, route))
      return res.status(403).json({ message: "Invalid token." });
    }
    if (response['value'] === 0) {
      await Log.create(createLog("NOT REGISTERED", response["decodedToken"].username, req.ip, req.headers["user-agent"], "GET", 404, route))
      return res.status(406).json({ message: "User not found!" });
    }
    let filter = "deletedFromSender";
    let person = "sender";
    if(req.params.inOrOut === "inbox"){
      filter = "deletedFromReceiver"
      person = "receiver";
    }

    req.query.page = parseInt(req.headers["page"]);
    req.query.limit = parseInt(req.headers["limit"]);
    req.query.sortType = req.headers["sort"];
    req.query.reverse = req.headers["reverse"];

    const page = parseInt(req.query.page) || 1;
    const limit = parseInt(req.query.limit) || 10;

    try {
      const query = {};
      query[filter] = false;
      query[person] = req.params.username;
      const totalItems = await Message.countDocuments(query);
      const totalPages = Math.ceil(totalItems / limit);
      const skip = (page - 1) * limit;
      const sortCriteria = {};
      sortCriteria[req.query.sortType] = parseInt(req.query.reverse);
      let items;
      if(req.query.sortType === ""){
        items = await Message.find(query).skip(skip).limit(limit);
      }
      else{
        items = await Message.find(query).sort(sortCriteria).skip(skip).limit(limit);
      }
      await Log.create(createLog(response["decodedToken"].userId, response["decodedToken"].username, req.ip, req.headers["user-agent"], "GET", 200, route))
      res.status(200).json({
        currentPage: page,
        totalPages: totalPages,
        totalItems: totalItems,
        items: items,
      });
    } catch (error) {
      res.status(500).json({ error: 'Internal server error' });
    }
  });

messageRoute
  .route("/:inOrOut/:id", cors(corsOptionsDelegate))
  .delete(async (req, res) => {
    let route = url + "/" + req.params.inOrOut + "/" + req.params.id;
    const token = req.headers["authorization"];
    const response = await control(token);
    if(response["value"] === -1){
      await Log.create(createLog("TOKEN INVALID", "TOKEN INVALID", req.ip, req.headers["user-agent"], "DELETE", 403, route))
      return res.status(403).json({ message: "Invalid token." });
    }
    if (response['value'] === 0) {
      await Log.create(createLog("NOT REGISTERED", response["decodedToken"].username, req.ip, req.headers["user-agent"], "DELETE", 404, route))
      return res.status(406).json({ message: "User not found!" });
    }
    try {
      const message = await Message.findById(req.params.id);
      if (!message) {
        await Log.create(createLog("NOT REGISTERED", response["decodedToken"].username, req.ip, req.headers["user-agent"], "DELETE", 404, route))
        return res.status(404).json({ message: 'Message not found' });
      }
      if(req.params.inOrOut === "inbox"){
        message.deletedFromReceiver = true;
      }
      else{
        message.deletedFromSender = true;
      }
      await message.save();
      await Log.create(createLog(response["decodedToken"].userId, response["decodedToken"].username, req.ip, req.headers["user-agent"], "DELETE", 200, route))
      res.status(200).json(message);
    } catch (err) {
      res.status(500).json({ error: 'Internal server error' });
    }
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

module.exports = messageRoute;
