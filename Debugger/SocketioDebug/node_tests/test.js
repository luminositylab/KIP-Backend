var socket = require('socket.io-client')('http://localhost:5000');

socket.on('connect', function(){
	console.log('connected!');
	socket.emit('update');
});

socket.on('update', function(data){
	console.log('updated!');
});


socket.on('disconnect', function(){
	console.log('disconnected!');
});
