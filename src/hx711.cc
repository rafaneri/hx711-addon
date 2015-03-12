#include <node.h>
#include "HX711.h"

using namespace v8;

void InitAll(Handle<Object> exports) {
  HX711::Init(exports);
}

NODE_MODULE(hx711, InitAll)
