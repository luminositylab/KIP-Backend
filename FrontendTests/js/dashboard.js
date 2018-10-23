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
function send_left_arm(speed) {
  send_route("/arm/left/" + speed);
}
function send_right_arm(speed) {
  send_route("/arm/right/" + speed);
}


var left_shoulder_down = document.getElementById('left-shoulder-down');
var left_shoulder_up = document.getElementById('right-shoulder-up');

left_shoulder_down.onmousedown = function() {

};
left_shoulder_down.onmouseup = function() {

};


var wristServoLeft = document.getElementById('wrist-servo-left');

wristServoLeft.addEventListener("keyup", function(event) {
    if (event.key === "Enter") {
        send_servo(left_wrist_servo_id, wristServoLeft.value);
    }
});

function update_drive(keys){
  var left_arm = 0;
  var right_arm = 0;
  var left = 0;
  var right = 0;

  if(keys.includes('z')) {
    left_arm += 0.5;
  }
  if(keys.includes('x')) {
    left_arm -= 0.5;
  }
  if(keys.includes('c')) {
    right_arm += 0.5;
  }
  if(keys.includes('v')) {
    right_arm -= 0.5;
  }
  if(keys.includes('a')) {
    left += 0.5;
    right += 0.5;
  }
  if(keys.includes('d')) {
    left -= 0.5;
    right -= 0.5;
  }
  if(keys.includes('w')) {
    left -= 0.5;
    right += 0.5;
  }
  if(keys.includes('s')) {
    left += 0.5;
    right -= 0.5;
  }

  left *= 50;
  right *= 50;
  left_arm *= 50;
  right_arm *= 50;
  send_drive(left, right);
  send_left_arm(left_arm);
  send_right_arm(right_arm);
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
