var eyes = document.getElementById("eyes");

var all_eyes = [
    "./Assets/eyes-scout.gif",
    "./Assets/eyes-love.gif",
    "./Assets/eyes-cry.gif"
]
var i = 0;
setInterval(function(){
    i++;
    if(i>=3){
        i = 0;
    } 
    eyes.src = all_eyes[i];
}, 6000 * 2);