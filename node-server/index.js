console.log("oof");
const i2c = require('i2c-bus');
var redis = require('redis'),
    client = redis.createClient();
const ADDR = 0x08;
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
const i2c1 = i2c.openSync(1);

client.on('error', function(err) {
	console.log("ERROR: " + err);
});
client.set("user", "disconnected");
//var fs = require('fs')
//const wstream = fs.createWriteStream('./myfifo')

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
function send_packet(left_speed, right_speed, arm_pos, gripper) {
	try {
		console.log(255 + "|" + direction(left_speed) + "|" + Math.abs(left_speed) + "|" + direction(right_speed) + "|" + Math.abs(right_speed) + "|" + Math.abs(arm_pos) + "|" + Math.abs(gripper) + "|" + 0);
		i2c1.writeByteSync(ADDR, 255, direction(left_speed));// Math.abs(left_speed), direction(right_speed), Math.abs(right_speed), Math.abs(arm_pos), Math.abs(gripper));
		i2c1.writeByteSync(ADDR, Math.abs(left_speed), direction(right_speed));
		i2c1.writeByteSync(ADDR, Math.abs(right_speed), Math.abs(arm_pos));
		i2c1.writeByteSync(ADDR, Math.abs(gripper), 0);
	} catch (e) {
		console.log('ERROR AND CRASH' + e);
	}
}

app.get('/', function(req, res){
  res.sendFile(__dirname + '/index.html');
});

var lm = 0;
var rm = 0;
var arm = 0;
var gripper = 0;
io.on('connection', function(socket){
	client.set("user", "connected")
  socket.on("message", function(data) {
	  console.log("redis: " + client.get("foo"));
	  parsed_data = JSON.parse(data);
	  var motors = joystickToDiff(parsed_data.x*0.1, parsed_data.y*0.3, -1.0, 1.0, -127.0, 127.0);
	  //console.log('left: ' + motors[0]);
	  //console.log('right:' + motors[1]);
	  lm = parseInt((motors[0]) * 100);
	  rm = parseInt((motors[1]) * 100);
	  arm = parseInt(parsed_data.arm * 100);
	  //console.log('left: ' + lm);
	  //console.log('right: ' + rm);
	  //console.log('arm: ' + arm);
	  send_packet(lm, rm, arm, gripper);
	  
	  if(Math.abs(parsed_data.x) > 0.16 || Math.abs(parsed_data.y) > 0.16) {
		  client.set("eyes", "fast");
	  } else {
		  client.set("eyes", "slow");
	  }
	  //console.log("x: " + parsed_data.x);
	  //console.log("y: " + parsed_data.y);
  });
  socket.on("arm", function(data){
  	var _data = JSON.parse(data);
	arm = parseInt(_data.arm * 100);
	console.log("arm " + arm);	
	send_packet(lm, rm, arm, gripper);

  });
  socket.on("gripper", function(data){
  	var _data = JSON.parse(data);
	gripper = parseInt(_data.gripper * 100);
	//console.log("gripper: " + gripper);
	send_packet(lm, rm, arm, gripper);
  });
  socket.on("disconnect", function() {
  	client.set("user", "disconnected");
  });
  console.log("a user connected");
});

http.listen(3000, function(){
  //console.log('listening on *:3000');
});
i2c1.closeSync();

