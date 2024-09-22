const mongoose = require('mongoose');
const Schema = mongoose.Schema;

// Define collection and schema
let Log = new Schema({
  userId: {
    type: String,
  },
  username: {
    type: String,
  },
  userIp: {
    type: String,
  },
  method: {
    type: String,
  },
  userAgent: {
    type: String,
  },
  route: {
    type: String,
  },
  responseStatus: {
    type: Number,
  },
  date: {
    type: Date,
  }
}, {
  collection: 'Log'
})


module.exports = mongoose.model('Log', Log)
