const mongoose = require('mongoose');
const Schema = mongoose.Schema;


let activeTokens = new Schema({
  token: {
    type: String,
  },
}, {
  collection: 'activeTokens'
})

module.exports = mongoose.model('ActiveTokens', activeTokens)
