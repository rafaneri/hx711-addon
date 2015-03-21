// HX711.h
#ifndef HX711_H
#define HX711_H

#include <node.h>
#include <mraa/gpio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

using namespace v8;

class HX711 : public node::ObjectWrap {
	public:
		static void Init(v8::Handle<v8::Object> exports);

	private:
		// define clock and data pin, channel, and gain factor
		// channel selection is made by passing the appropriate gain: 128 or 64 for channel A, 32 for channel B
		// gain: 128 or 64 for channel A; channel B works with 32 gain factor only
		explicit HX711(uint8_t data = 3, uint8_t sck = 2, uint8_t gain = 128);
		~HX711();

		// node.js wrapper methods
		static v8::Handle<v8::Value> New(const v8::Arguments& args);
		static v8::Handle<v8::Value> Read(const v8::Arguments& args);
		static v8::Handle<v8::Value> SetGain(const v8::Arguments& args);
		static v8::Handle<v8::Value> ReadAverage(const v8::Arguments& args);
		static v8::Handle<v8::Value> GetValue(const v8::Arguments& args);
		static v8::Handle<v8::Value> GetUnits(const v8::Arguments& args);
		static v8::Handle<v8::Value> Tare(const v8::Arguments& args);
		static v8::Handle<v8::Value> SetScale(const v8::Arguments& args);
		static v8::Handle<v8::Value> SetOffset(const v8::Arguments& args);
		static v8::Persistent<v8::Function> constructor;
		
		mraa_gpio_context m_sckPinCtx; // Power Down and Serial Clock Input Pin
		mraa_gpio_context m_dataPinCtx; // Serial Data Output Pin
		
		uint8_t GAIN; // amplification factor
		unsigned long OFFSET; // used for tare weight
		float SCALE; // used to return weight in grams, kg, ounces, whatever

		// waits for the chip to be ready and returns a reading
		unsigned long read();

		// set the gain factor; takes effect only after a call to read()
		// channel A can be set for a 128 or 64 gain; channel B has a fixed 32 gain
		// depending on the parameter, the channel is also set to either A or B
		void setGain(uint8_t gain = 128);

		// returns an average reading; times = how many times to read
		unsigned long readAverage(uint8_t times = 10);

		// returns (readAverage() - OFFSET), that is the current value without the tare weight; times = how many readings to do
		double getValue(uint8_t times = 10);

		// returns getValue() divided by SCALE, that is the raw value divided by a value obtained via calibration
		// times = how many readings to do
		float getUnits(uint8_t times = 1);

		// set the OFFSET value for tare weight; times = how many times to read the tare value
		void tare(uint8_t times = 10);

		// set the SCALE value; this value is used to convert the raw data to "human readable" data (measure units)
		void setScale(float scale = 1.f);

		// set OFFSET, the value that's subtracted from the actual reading (tare weight)
		void setOffset(long offset = 0);
};

#endif /* HX711_H */
