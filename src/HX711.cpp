#include <iostream>
#include "HX711.h"

using namespace v8;

Persistent<Function> HX711::constructor;

HX711::HX711(uint8_t data, uint8_t sck) {    
    mraa_init();
    DATA = mraa_gpio_init(data);
    SCK = mraa_gpio_init(sck);
    
    mraa_gpio_dir(DATA, MRAA_GPIO_IN);
    mraa_gpio_dir(SCK, MRAA_GPIO_OUT);

    CALIBRATION = preciseReading();
}

HX711::~HX711() {
    mraa_gpio_close(DATA);
    mraa_gpio_close(SCK);
}

void HX711::Init(Handle<Object> exports) {
    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(String::NewSymbol("HX711"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    // Prototype
    tpl->PrototypeTemplate()->Set(String::NewSymbol("getValue"),
                                  FunctionTemplate::New(GetValue)->GetFunction());
    constructor = Persistent<Function>::New(tpl->GetFunction());
    exports->Set(String::NewSymbol("HX711"), constructor);
}

Handle<Value> HX711::New(const Arguments& args) {
    HandleScope scope;
    
    if (args.IsConstructCall()) {
        // Invoked as constructor: `new HX711(...)`
        uint8_t data = args[0]->IsUndefined() ? 3 : args[0]->NumberValue();
        uint8_t sck = args[1]->IsUndefined() ? 2 : args[1]->NumberValue();
        HX711* obj = new HX711(data, sck);
        obj->Wrap(args.This());
        return args.This();
    } else {
        // Invoked as plain function `HX711(...)`, turn into construct call.
        const int argc = 2;
        Local<Value> argv[argc] = { args[0], args[1] };
        return scope.Close(constructor->NewInstance(argc, argv));
    }
}

Handle<Value> HX711::GetValue(const Arguments& args) {
    HandleScope scope;
    
    HX711* obj = ObjectWrap::Unwrap<HX711>(args.This());
    
    int samples = args[0]->IsUndefined() ? 10 : args[0]->NumberValue();
    
    return scope.Close(Number::New(obj->preciseReading(samples)));
}
                              
long HX711::read() {
    long v = 0;
                                  
    while (mraa_gpio_read(DATA) == 1);
                                  
    for (int i=0; i<24; i++)
    {
        mraa_gpio_write(SCK, 1);
        v <<= 1;
        v |= (mraa_gpio_read(DATA) == 1) ? 1 : 0;
        mraa_gpio_write(SCK, 0);
    }
                                  
    mraa_gpio_write(SCK, 1);
    mraa_gpio_write(SCK, 0);
                                  
    v |= (v & 0x00800000) ? 0xff000000 : 0x00000000;
                                  
    return v;
}
                              
long HX711::preciseReading(int samples) {
    long v = 0;
    for (int i=0; i<samples; i++)
    {
        v += read();
    }
    return (v >> SAMPLES_LOG2);
}
