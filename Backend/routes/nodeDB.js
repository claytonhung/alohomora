var jsonDB = require('node-json-db')
var db = new jsonDB("db", true, true);

exports.logData = function(user) {
  var data = db.getData(String(user));
  console.log(data);
};

exports.addUser = function(name, username, keyID, unlockID) {

};

exports.updateUser = function(user_number, field, value) {

};

exports.removeUser = function(user_number) {
};

exports.addKey = function(key_number) {

};

exports.updateKey = function(key_number, field, value) {

};

exports.removeKey = function(key_number) {

};

exports.getData = function(number) {
	console.log("Get Data called");
	return db.getData("/users[0]");
};