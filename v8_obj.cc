#include <map>
#include <nan.h>

class V8_Obj : public Nan::ObjectWrap {
public:
  static void Init(Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE exports, v8::Local<v8::Value> module);

private:
  std::map<std::string, std::string> contents;
  static NAN_METHOD(New);
  static NAN_PROPERTY_SETTER(Setter);
  static NAN_PROPERTY_GETTER(Getter);
};

NAN_PROPERTY_SETTER(V8_Obj::Setter) {
  v8::String::Utf8Value data(value);
  v8::String::Utf8Value prop(property);
  auto self = Nan::ObjectWrap::Unwrap<V8_Obj>(info.This());
  self->contents.insert({ *prop, *data });
}

NAN_PROPERTY_GETTER(V8_Obj::Getter) {
  if (property->IsSymbol()) {
    if (Nan::Equals(property, v8::Symbol::GetIterator(info.GetIsolate())).FromJust())
      Nan::ThrowError("Does not support for-of");
    return;
  }
  v8::String::Utf8Value prop(property);
  auto self = Nan::ObjectWrap::Unwrap<V8_Obj>(info.This());
  auto pair = self->contents.find(*prop);
  if (pair == self->contents.end())
    return;
  info.GetReturnValue().Set(Nan::New<v8::String>(pair->second.c_str()).ToLocalChecked());
}

NAN_METHOD(V8_Obj::New) {
  V8_Obj *obj = new V8_Obj();
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

void V8_Obj::Init(Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE exports, v8::Local<v8::Value> module) {
  v8::Local<v8::FunctionTemplate> tmpl = Nan::New<v8::FunctionTemplate>(New);
  tmpl->InstanceTemplate()->SetInternalFieldCount(1);
  Nan::SetNamedPropertyHandler(tmpl->InstanceTemplate(), Getter, Setter);
  Nan::Set(module.As<v8::Object>(), Nan::New("exports").ToLocalChecked(),
           Nan::GetFunction(tmpl).ToLocalChecked());
}

NODE_MODULE(v8_obj, V8_Obj::Init)
