var mysql = require('mysql');

var host = 'localhost';
var user = 'root';
var password = 'password'; //hardcoded.. for now
var dbName = 'alohomora';
// var sqlInfo = {
//   host: 'localhost',
//   user: 'root',
//   password: 'bixhsu',
//   database: 'alohomora'
// };

var connection = mysql.createConnection({
  host: host,
  user: user,
  password: password,
  database: dbName
});

var userTable = 'users';
var lockTable = 'locks';
var keyTable = 'keys';

connection.connect(function(err){
  if(err) {
    console.error("error connecting to mysql: " + err.stack);
    return;
  }

  console.log('connected as id' + connection.threadId);
});

exports.updateUser = function(callback) {
  var sql = 'SELECT * FROM users WHERE id = 1';
};

exports.updateLock = function(callback) {
  var sql = 'SELECT * FROM locks WHERE id = 1';
};

exports.updateKey = function(callback) {
  var sql = 'SELECT * FROM keys WHERE id = 1';
};
