var express = require('express');
var mysql = require('./routes/sqlConnection');
var path = require('path');
var bodyParser = require('body-parser');
var handler = require('./routes/handler');
var app = express();


var allowCrossDomain = function(req, res, next) {
    res.header('Access-Control-Allow-Origin', '*');
    res.header('Access-Control-Allow-Methods', 'GET, POST');
    res.header('Access-Control-Allow-Headers', 'Content-Type');
    next();
};

app.configure(function() {
    app.set('port', process.env.PORT || 3000);
    app.use(express.bodyParser());
    app.use(express.methodOverride());
    app.use(app.router);

    app.use(express.static(path.join(__dirname, 'public')));
    app.use(allowCrossDomain);

    console.log("Server is running!");
});

app.post('/api/v1/user', handler.updateUser);

app.post('/api/v1/lock', handler.updateLock);

app.post('/api/v1/key', handler.updateKey);

app.listen(3000);