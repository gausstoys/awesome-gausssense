var express = require('express');
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

var SerialPort = require("serialport").SerialPort;

require("serialport").list(function (err, ports) {
  ports.forEach(function(port) {
    console.log(port.comName);
    // console.log(port.pnpId);
    // console.log(port.manufacturer);
  });
});

var serialPort = new SerialPort("/dev/cu.usbmodem1421", {
  baudrate: 115200,
  parser: require("serialport").parsers.readline("\n")
});

serialPort.on("open", function () {
  console.log('open');
  serialPort.on('data', function(data) {
    io.emit('bipolar', data);
  });
});

app.use(express.static('public'));

app.get('/', function(req, res){
	res.sendFile(__dirname + '/index.html');
});

io.on('connection', function(socket){
  console.log('socket connected');

  socket.on('disconnect', function(){
    console.log('socket disconnected');
  });
});

http.listen(5000, function(){
  console.log('listening on *:5000');
});