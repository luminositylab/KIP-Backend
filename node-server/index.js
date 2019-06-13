const i2c = require('i2c-bus');
const ADDR = 0x08;
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
const i2c1 = i2c.openSync(1);


function joystickToDiff(x, y, minJoystick, maxJoystick, minSpeed, maxSpeed) {
	if (x == 0 && y == 0) {
		return [0,0];
	}
	var z = Math.sqrt(x * x + y * y); // compute hyotonuse
	var rad = Math.acos(Math.abs(x) / z); // compute angle in radians
	var angle = (rad * 180) / Math.PI;
	
	var tcoeff = -1 + (angle/90) * 2;
	var turn = tcoeff * Math.abs(Math.abs(y) -Math.abs(x));
	//turn = Math.round(turn * 100)/100;
	mov = Math.max(Math.abs(y), Math.abs(x));
	var rawLeft, rawRight;
	if ((x >= 0 && y >= 0) || (x < 0 && y < 0)) {
		rawLeft = mov;
		rawRight = turn;
	} else {
		rawRight = mov;
		rawLeft = turn;
	}
	if (y < 0) {
		rawLeft = 0 - rawLeft;
		rawRight = 0 - rawRight;
	}
	// normalize to -1.0, 1.0
	var rightOut = rawRight / 0.17;
	var leftOut = rawLeft / 0.17;
	return [rightOut, leftOut]
}
function direction(n) {
 if (n >= 0) {
 	return 1;
 } else {
 	return 0;
 } 
}
function send_packet(left_speed, right_speed, arm_pos) {
	i2c1.writeByteSync(ADDR, 255, direction(left_speed));
	i2c1.writeByteSync(ADDR, Math.abs(left_speed), direction(right_speed));
	i2c1.writeByteSync(ADDR, Math.abs(right_speed), Math.abs(arm_pos));
}

app.get('/', function(req, res){
  res.sendFile(__dirname + '/index.html');
});

var lm = 0;
var rm = 0;
var arm = 0;
io.on('connection', function(socket){
  socket.on("message", function(data) {
	  parsed_data = JSON.parse(data);
	  var motors = joystickToDiff(parsed_data.x, parsed_data.y, -1.0, 1.0, -127.0, 127.0);
	  //console.log('left: ' + motors[0]);
	  //console.log('right:' + motors[1]);
	  lm = parseInt((motors[0]) * 100);
	  rm = parseInt((motors[1]) * 100);
	  arm = parseInt(parsed_data.arm * 100);
	  console.log('left: ' + lm);
	  console.log('right: ' + rm);
	  console.log('arm: ' + arm);
	  send_packet(lm, rm, arm);
	  //console.log("x: " + parsed_data.x);
	  //console.log("y: " + parsed_data.y);
  });
  socket.on("arm", function(data){
  	var _data = JSON.parse(data);
	arm = parseInt(_data.arm * 100);
	console.log("arm " + arm);	
	send_packet(lm, rm, arm);

  });
  console.log('a user connected');
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});
i2c1.closeSync();

