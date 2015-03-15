var hx711 = require('./build/Release/hx711');

var scale = new hx711.HX711(3,2);

setInterval(function(){
	console.log(scale.getValue());
}, 500);