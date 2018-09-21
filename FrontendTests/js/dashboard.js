var left_wrist_servo_id = 0;
var right_wrist_servo_id = 1;
var left_gripper_servo_id = 2;
var right_gripper_servo_id = 3;
var xhttp = new XMLHttpRequest();
function send_route(extention){
  xhttp.open("GET", "http://192.168.1.106:5000"+ extention, true);
  xhttp.send();
}
function send_servo(id, degree) {
  send_route("/set/servo/" + id + "/" + degree);
}
function send_drive(left, right) {
  console.log("left " + left + " right " + right);
  //send_route("/drive/" + left + "/" + right);
}

var wristServoLeft = document.getElementById('wrist-servo-left');

wristServoLeft.addEventListener("keyup", function(event) {
    if (event.key === "Enter") {
        send_servo(left_wrist_servo_id, wristServoLeft.value);
    }
});

function update_drive(keys){
  var left = 0;
  var right = 0;
  if(keys.includes('w')) {
    left += 1;
    right += 1;
  }
  if(keys.includes('s')) {
    left -= 1;
    right -= 1;
  }
  if(keys.includes('a')) {
    left -= 0.5;
    right += 0.5;
  }
  if(keys.includes('d')) {
    left += 0.5;
    right -= 0.5;
  }
  if(!keys.includes('w')
  && !keys.includes('s')
  && !keys.includes('a')
  && !keys.includes('d')) {
    left = 0;
    right = 0;
  }
  left *= 40;
  right *= 40;
  send_drive(left, right);
}


var down_keys = [];
document.addEventListener("keydown", function(event){
  if(!down_keys.includes(event.key)) {
    down_keys.push(event.key);
  }
  update_drive(down_keys);
}, false);
document.addEventListener("keyup", function(event){
  if(down_keys.includes(event.key)) {
    down_keys.splice( down_keys.indexOf(event.key), 1 );
  }
  update_drive(down_keys);
}, false);

setInterval(function(){
  // http request runner
  //console.log(down_keys);
 }, 100);
