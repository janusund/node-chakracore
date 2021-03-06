// Copyright Microsoft. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#include "v8chakra.h"
#include "jsrtutils.h"

namespace v8 {

Local<Signature> Signature::New(Isolate* isolate,
                                Handle<FunctionTemplate> receiver,
                                int argc,
                                Handle<FunctionTemplate> argv[]) {
  if (argc != 0) {
    CHAKRA_UNIMPLEMENTED_("v8::Signature::New with args list");
  }

  return reinterpret_cast<Signature*>(*receiver);
}

Local<AccessorSignature> AccessorSignature::New(
    Isolate* isolate, Handle<FunctionTemplate> receiver) {
  return reinterpret_cast<AccessorSignature*>(*receiver);
}

bool InstanceOfButNotPrototypeObject(JsValueRef instance,
                                     JsValueRef constructor) {
  if (!jsrt::InstanceOf(instance, constructor)) {
    return false;
  }

  JsValueRef constructorProp;
  JsValueRef prototypeProp;
  if (jsrt::GetProperty(instance, "constructor", &constructorProp) ==
          JsNoError &&
      jsrt::GetProperty(constructorProp, "prototype", &prototypeProp) ==
          JsNoError &&
      prototypeProp == instance) {
    return false;
  }

  return true;
}

bool Utils::CheckSignature(Local<FunctionTemplate> receiver,
                           Local<Object> thisPointer,
                           Local<Object>* holder) {
  *holder = thisPointer;

  bool matched =
      InstanceOfButNotPrototypeObject(*thisPointer, *receiver->GetFunction());

  if (!matched) {
    const char txt[] = "Illegal invocation";
    JsValueRef msg, err;
    if (JsCreateString(txt, sizeof(txt) - 1, &msg) == JsNoError &&
        JsCreateTypeError(msg, &err) == JsNoError) {
      JsSetException(err);
    }
  }

  return matched;
}

}  // namespace v8
