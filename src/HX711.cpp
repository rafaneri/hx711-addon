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

HX711::HX711(uint8_t data, uint8_t sck) {
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
    unsigned long result = 0;
    
    do
    {
        result = obj->adcRead();
    }while(!(result ^ 0xffffff));
    
    return scope.Close(Number::New(result));
}

unsigned long HX711::adcRead() {
    unsigned long Count = 0;
    
    while (mraa_gpio_read(this->m_dataPinCtx));
                                  
    for (int i=0; i<24; i++)
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