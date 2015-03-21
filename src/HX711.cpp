#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "HX711.h"

using namespace v8;
using namespace std;

Persistent<Function> HX711::constructor;

struct HX711Exception : public std::exception {
    std::string message;
    HX711Exception (std::string msg) : message (msg) { }
    ~HX711Exception () throw () { }
    const char* what() const throw () { return message.c_str(); }
};

HX711::HX711(uint8_t data, uint8_t sck, uint8_t gain) {
    mraa_result_t error = MRAA_SUCCESS;
    
    this->m_dataPinCtx = mraa_gpio_init(data);
    if (this->m_dataPinCtx == NULL) {
        throw HX711Exception ("Couldn't initilize DATA pin.");
    }
    
    this->m_sckPinCtx = mraa_gpio_init(sck);
    if (this->m_sckPinCtx == NULL) {
        throw HX711Exception ("Couldn't initilize CLOCK pin.");
    }
    
    error = mraa_gpio_dir (this->m_dataPinCtx, MRAA_GPIO_IN);
    if (error != MRAA_SUCCESS) {
        throw HX711Exception ("Couldn't set direction for DATA pin.");
    }
    
    error = mraa_gpio_dir (this->m_sckPinCtx, MRAA_GPIO_OUT);
    if (error != MRAA_SUCCESS) {
        throw HX711Exception ("Couldn't set direction for CLOCK pin.");
    }

    this->setGain(gain);
}

HX711::~HX711() {
    mraa_result_t error = MRAA_SUCCESS;
    
    error = mraa_gpio_close (this->m_dataPinCtx);
    if (error != MRAA_SUCCESS) {
        mraa_result_print(error);
    }
    
    error = mraa_gpio_close (this->m_sckPinCtx);
    if (error != MRAA_SUCCESS) {
        mraa_result_print(error);
    }
}

void HX711::Init(Handle<Object> exports) {
    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(String::NewSymbol("HX711"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    // Prototype
    tpl->PrototypeTemplate()->Set(String::NewSymbol("read"),
                                  FunctionTemplate::New(Read)->GetFunction());
    tpl->PrototypeTemplate()->Set(String::NewSymbol("setGain"),
                                  FunctionTemplate::New(SetGain)->GetFunction());
    tpl->PrototypeTemplate()->Set(String::NewSymbol("readAverage"),
                                  FunctionTemplate::New(ReadAverage)->GetFunction());
    tpl->PrototypeTemplate()->Set(String::NewSymbol("getValue"),
                                  FunctionTemplate::New(GetValue)->GetFunction());
    tpl->PrototypeTemplate()->Set(String::NewSymbol("getUnits"),
                                  FunctionTemplate::New(GetUnits)->GetFunction());
    tpl->PrototypeTemplate()->Set(String::NewSymbol("tare"),
                                  FunctionTemplate::New(Tare)->GetFunction());
    tpl->PrototypeTemplate()->Set(String::NewSymbol("setScale"),
                                  FunctionTemplate::New(SetScale)->GetFunction());
    tpl->PrototypeTemplate()->Set(String::NewSymbol("setOffset"),
                                  FunctionTemplate::New(SetOffset)->GetFunction());
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

Handle<Value> HX711::Read(const Arguments& args) {
    HandleScope scope;
    
    HX711* obj = ObjectWrap::Unwrap<HX711>(args.This());
    unsigned long result = obj->read();
    
    return scope.Close(Number::New(result));
}

Handle<Value> HX711::SetGain(const Arguments& args) {
    HandleScope scope;
    
    uint8_t gain = 128;

    if (args.Length() > 0) {
        if (!args[0]->IsNumber()) {
            ThrowException(Exception::TypeError(String::New("Wrong arguments")));
            return scope.Close(Undefined());
        } else {
            gain = args[0]->NumberValue();
        }
    }

    HX711* obj = ObjectWrap::Unwrap<HX711>(args.This());
    obj->setGain(gain);
    
    return scope.Close(Undefined());
}

Handle<Value> HX711::ReadAverage(const Arguments& args) {
    HandleScope scope;

    uint8_t times = 10;

    if (args.Length() > 0) {
        if (!args[0]->IsNumber()) {
            ThrowException(Exception::TypeError(String::New("Wrong arguments")));
            return scope.Close(Undefined());
        } else {
            times = args[0]->NumberValue();
        }
    }
    
    HX711* obj = ObjectWrap::Unwrap<HX711>(args.This());
    unsigned long result = obj->readAverage(times);
    
    return scope.Close(Number::New(result));
}

Handle<Value> HX711::GetValue(const Arguments& args) {
    HandleScope scope;
    
    uint8_t times = 10;

    if (args.Length() > 0) {
        if (!args[0]->IsNumber()) {
            ThrowException(Exception::TypeError(String::New("Wrong arguments")));
            return scope.Close(Undefined());
        } else {
            times = args[0]->NumberValue();
        }
    }
    
    HX711* obj = ObjectWrap::Unwrap<HX711>(args.This());
    double result = obj->getValue(times);
    
    return scope.Close(Number::New(result));
}

Handle<Value> HX711::GetUnits(const Arguments& args) {
    HandleScope scope;
    
    uint8_t times = 1;

    if (args.Length() > 0) {
        if (!args[0]->IsNumber()) {
            ThrowException(Exception::TypeError(String::New("Wrong arguments")));
            return scope.Close(Undefined());
        } else {
            times = args[0]->NumberValue();
        }
    }
    
    HX711* obj = ObjectWrap::Unwrap<HX711>(args.This());
    float result = obj->getUnits(times);
    
    return scope.Close(Number::New(result));
}

Handle<Value> HX711::Tare(const Arguments& args) {
    HandleScope scope;
    
    uint8_t times = 10;

    if (args.Length() > 0) {
        if (!args[0]->IsNumber()) {
            ThrowException(Exception::TypeError(String::New("Wrong arguments")));
            return scope.Close(Undefined());
        } else {
            times = args[0]->NumberValue();
        }
    }
    
    HX711* obj = ObjectWrap::Unwrap<HX711>(args.This());
    obj->tare(times);

    return scope.Close(Undefined());
}

Handle<Value> HX711::SetScale(const Arguments& args) {
    HandleScope scope;
    
    float scale = 1.f;

    if (args.Length() > 0) {
        if (!args[0]->IsNumber()) {
            ThrowException(Exception::TypeError(String::New("Wrong arguments")));
            return scope.Close(Undefined());
        } else {
            scale = args[0]->NumberValue();
        }
    }
    
    HX711* obj = ObjectWrap::Unwrap<HX711>(args.This());
    obj->setScale(scale);
    
    return scope.Close(Undefined());
}

Handle<Value> HX711::SetOffset(const Arguments& args) {
    HandleScope scope;
    
    long offset = 0;

    if (args.Length() > 0) {
        if (!args[0]->IsNumber()) {
            ThrowException(Exception::TypeError(String::New("Wrong arguments")));
            return scope.Close(Undefined());
        } else {
            offset = args[0]->NumberValue();
        }
    }
    
    HX711* obj = ObjectWrap::Unwrap<HX711>(args.This());
    obj->setOffset(offset);
    
    return scope.Close(Undefined());
}

unsigned long HX711::read() {
    unsigned long Count = 0;
    
    while (mraa_gpio_read(this->m_dataPinCtx));
                                  
    for (int i=0; i<GAIN; i++)
    {
        mraa_gpio_write(this->m_sckPinCtx, 1);
        Count = Count << 1;
        mraa_gpio_write(this->m_sckPinCtx, 0);
        if(mraa_gpio_read(this->m_dataPinCtx))
        {
            Count++;
        }
    }

    mraa_gpio_write(this->m_sckPinCtx, 1);
    Count = Count ^ 0x800000;
    mraa_gpio_write(this->m_sckPinCtx, 0);

    return (Count);
}

void HX711::setGain(uint8_t gain){
    switch (gain) {
        case 128:       // channel A, gain factor 128
            GAIN = 24;
            break;
        case 64:        // channel A, gain factor 64
            GAIN = 26;
            break;
        case 32:        // channel B, gain factor 32
            GAIN = 25;
            break;
    }

    mraa_gpio_write(this->m_sckPinCtx, 0);
    read();
}

unsigned long HX711::readAverage(uint8_t times){
    unsigned long sum = 0;
    for (uint8_t i = 0; i < times; i++) {
        sum += read();
    }
    return sum / times;
}

double HX711::getValue(uint8_t times){
    return readAverage(times) - OFFSET;
}

float HX711::getUnits(uint8_t times){
    return getValue(times) / SCALE;
}

void HX711::tare(uint8_t times){
    double sum = readAverage(times);
    setOffset(sum);
}

void HX711::setScale(float scale){
    SCALE = scale;
}

void HX711::setOffset(long offset){
    OFFSET = offset;
}
