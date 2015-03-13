var hx711 = require('./build/Release/hx711');

var scale = new hx711.HX711(3,2);

for(var i=0; i<100; i++)
{
	console.log(scale.getValue());
}
