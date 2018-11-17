var left_wrist_servo_id = 0;
var right_wrist_servo_id = 1;
var left_gripper_servo_id = 2;
var right_gripper_servo_id = 3;

var xhttp = new XMLHttpRequest();
function send_route(extention){
  xhttp.open("GET", "http://192.168.1.203:5000"+ extention, true);
  xhttp.send();
}
function send_servo(id, degree) {
}
function send_right_wrist(position) {
  send_route("/wrist/left/" + position);
  
}
function send_drive(left, right) {
  // console.log("left " + left + " right " + right);
  send_route("/drive/" + left + "/" + right);
}
function send_left_arm(speed) {
  send_route("/arm/left/" + speed);
}
function send_right_arm(speed) {
  send_route("/arm/right/" + speed);
}


var left_shoulder_down = document.getElementById('left-shoulder-down');
var left_shoulder_up = document.getElementById('left-shoulder-up');

var right_shoulder_down = document.getElementById('right-shoulder-down');
var right_shoulder_up = document.getElementById('right-shoulder-up');

var right_shoulder_down_flag = false;
right_shoulder_down.onmousedown = function() {
  right_shoulder_down_flag = true;
}
right_shoulder_down.onmouseup = function() {
  right_shoulder_down_flag = false;
  send_right_arm(0);
}

var right_shoulder_up_flag = false;
right_shoulder_up.onmousedown = function () {
  right_shoulder_up_flag = true;
}
right_shoulder_up.onmouseup = function() {
  right_shoulder_up_flag = false;
  send_right_arm(0);

}

var left_shoulder_down_flag = false;
left_shoulder_down.onmousedown = function() {
  left_shoulder_down_flag = true;
  
};
left_shoulder_down.onmouseup = function() {
  left_shoulder_down_flag = false;  
  send_left_arm(0)
};

var left_shoulder_up_flag = false;
left_shoulder_up.onmousedown = function() {
  left_shoulder_up_flag = true;
  
};
left_shoulder_up.onmouseup = function() {
  left_shoulder_up_flag = false;
  send_left_arm(0)
};

setInterval(function(){
  if(left_shoulder_down_flag){
    send_left_arm(-50);
  } else if(left_shoulder_up_flag) {
    send_left_arm(50)
  }
  if (right_shoulder_down_flag) {
    send_right_arm(-50);
  } else if (right_shoulder_up_flag) {
    send_right_arm(50);
  }

}, 100);



var wristServoLeft = document.getElementById('wrist-servo-left');
wristServoLeft.onkeypress = function(key) {
  if (key.key == 'Enter') {
    send_right_wrist(wristServoLeft.value);
  }
}


wristServoLeft.addEventListener("keyup", function(event) {
    if (event.key === "Enter") {
        send_servo(left_wrist_servo_id, wristServoLeft.value);
    }
});
function clamp(value, clamp_value) {
  if (value == 0) {
    return 0;
  }
  if(Math.abs(value) > clamp_value) {
    return value > 0 ? clamp_value : -clamp_value;
  }
  return value;
}
var start = (new Date().getTime());
var dt = (new Date()).getTime();
var updateRate = 10; // htz
var acc = 0;
function update_drive(keys, useRateLimit){
  dt = (new Date()).getTime() - start;
  
  var left = 0;
  var right = 0;

  
  if(keys.includes('w')) {
    left += 0.5;
    right += 0.5;
  }
  if(keys.includes('s')) {
    left -= 0.5;
    right -= 0.5;
  }
  if(keys.includes('a')) {
    left -= 0.5;
    right += 0.5;
  }
  if(keys.includes('d')) {
    left += 0.5;
    right -= 0.5;
  }
  
  left = clamp(left, 0.5);
  right = clamp(right, 0.5);
  left *= 50;
  right *= 50;
  
  acc += dt;
  if(useRateLimit) {
    if (acc >= (1/updateRate) * 1000){
      send_drive(left, right);
      
      acc = 0;
    }


  } else {
    send_drive(left, right);
  }
  
  start = (new Date().getTime());
  
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
  update_drive(down_keys, true);
}, false);
document.addEventListener("keyup", function(event){
  if(down_keys.includes(event.key)) {
    down_keys.splice( down_keys.indexOf(event.key), 1 );
  }
  down_run_flag = true;
  update_drive(down_keys, false);
}, false);

