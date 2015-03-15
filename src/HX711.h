// HX711.h
#ifndef HX711_H
#define HX711_H

#include <node.h>
#include <mraa/gpio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define SAMPLES_LOG2 8
#define SAMPLES 4

using namespace v8;

class HX711 : public node::ObjectWrap {
	public:
		static void Init(v8::Handle<v8::Object> exports);

	private:
		explicit HX711(uint8_t data = 3, uint8_t sck = 2);
		~HX711();

		static v8::Handle<v8::Value> New(const v8::Arguments& args);
		static v8::Handle<v8::Value> GetValue(const v8::Arguments& args);
		static v8::Persistent<v8::Function> constructor;
		mraa_gpio_context m_sckPinCtx;
		mraa_gpio_context m_dataPinCtx;
		unsigned long m_calibration;
		unsigned long adcRead();
};

#endif /* HX711_H */
