const mongoose = require('mongoose');
const Schema = mongoose.Schema;

// Define collection and schema
let Message = new Schema({
  title: {
    type: String,
  },
  message: {
    type: String,
  },
  sender: {
    type: String,
    required: true
  },
  receiver: {
    type: String,
    required: true
  },
  deletedFromReceiver: {
    type: Boolean,
    default: false,
  },
  deletedFromSender: {
    type: Boolean,
    default: false,
  },
  date: {
    type: Date,
  }
}, {
  collection: 'Messages'
})


module.exports = mongoose.model('Message', Message)
