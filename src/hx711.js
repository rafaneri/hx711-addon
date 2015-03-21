var hx711 = require('./build/Release/hx711');

var scale = new hx711.HX711(3,2);

setTimeout(function(){
	scale.setScale(2837);
	scale.tare();
	setTimeout(function(){
		setInterval(function(){
			console.log(scale.getUnits());
		}, 2000);
	}, 2000);
}, 2000);