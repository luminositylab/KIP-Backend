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
  send_route("/drive/" + left + "/" + right);
}
function send_arm(motor, direction, speed) {
  send_route("/set/motor/speed/" + motor + "/" + direction + "/" + speed);
}


var left_shoulder_down = document.getElementById('left-shoulder-down');
var left_shoulder_up = document.getElementById('right-shoulder-up');

left_shoulder_down.onmousedown = function() {
  console.log("down");
  send_arm(2, 1, 20);
};
left_shoulder_down.onmouseup = function() {
  send_arm(2, 1, 0);
};


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

  left *= 50;
  right *= 50;
  send_drive(left, right);
}

var down_run_flag = true;
var down_keys = [];
document.addEventListener("keypress", function(event){
  if(!down_keys.includes(event.key)) {
    down_keys.push(event.key);
  }
  if(down_run_flag) {

    down_run_flag = false;
  }
  update_drive(down_keys);
}, false);
document.addEventListener("keyup", function(event){
  if(down_keys.includes(event.key)) {
    down_keys.splice( down_keys.indexOf(event.key), 1 );
  }
  down_run_flag = true;
  update_drive(down_keys);
}, false);

setInterval(function(){
  // http request runner
  //console.log(down_keys);
 }, 100);
