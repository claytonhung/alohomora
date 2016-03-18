
var sqlConnection = require('./sqlConnection');
var md5 = require('MD5');
var sql = require('./sqlREST');

exports.updateUser = function(req, res) {
  var user = {};
  user._id = req.body.id;
  user.username = req.body.username;
  user.email = req.body.email;
  user.password = req.body.password;

  sql.updateUser(user, function() {
    res.send("User updated");
  });
};

exports.updateLock = function(req, res) {
  var lock = {};
  lock._id = req.body.id;
  //lock.address = req.body.address;

  sql.updateLock(lock, function() {
    res.send("Lock Updated");
  });
};

exports.updateKey = function(req, res) {
  var key = {};
  key._id = req.body.id;
  //lock id
  //user id
  //duration
  //start time
  //end time

  sql.updateKey(key, function() {
    res.send("Key updated");
  });
};
