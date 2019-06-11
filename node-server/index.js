const i2c = require('i2c-bus');
const ADDR = 0x08;
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
const i2c1 = i2c.openSync(1);

app.get('/', function(req, res){
  res.sendFile(__dirname + '/index.html');
});

io.on('connection', function(socket){
  socket.on("message", function(data) {
	  parsed_data = JSON.parse(data);
	  i2c1.writeByteSync(ADDR, 255, Math.abs(parseInt(parsed_data.x * 127)));
	  i2c1.writeByteSync(ADDR, Math.abs(parseInt(parsed_data.y * 127)), 0);
	  console.log("x: " + parsed_data.x);
	  console.log("y: " + parsed_data.y);
  });
  console.log('a user connected');
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});

//const i2c1 = i2c.openSync(1);
//for(var i = 0; i < 5; i++){
	i2c1.writeByteSync(ADDR, 255, 0);
	
	i2c1.writeByteSync(ADDR, 0, 10);
//}
console.log("done.");
i2c1.closeSync();

