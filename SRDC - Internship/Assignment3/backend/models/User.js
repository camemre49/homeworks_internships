const mongoose = require('mongoose');
const Schema = mongoose.Schema;

// Define collection and schema
let User = new Schema({
   name: {
      type: String,
   },
   email: {
      type: String,
     unique: true,
   },
   city: {
      type: String

   },
   username: {
      type: String,
     unique: true,
   },
  surname: {
    type: String,
  },
  gender: {
    type: String,
  },
  admin: {
    type: Boolean
  },
  password: {
    type: String,
  },
  dateOfBirth: {
     type: Date
  }
}, {
   collection: 'Users'
})

module.exports = mongoose.model('User', User)
