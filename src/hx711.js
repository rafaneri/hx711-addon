var hx711 = require('./build/Release/hx711');

var scale = new hx711.HX711(3,2);

for(var i=0; i<100; i++)
{
	scale.getValue(function(v){
		console.log(v);
	});
	delay(5000);
}

function delay(milliseconds) {
	var start = new Date().getTime();
	for (var i = 0; i < 1e7; i++) {
		if ((new Date().getTime() - start) > milliseconds){
			break;
		}
	}
}