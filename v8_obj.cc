#include <map>
#include <nan.h>

class V8_Obj : public Nan::ObjectWrap {
public:
  static void Init(Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE exports, v8::Local<v8::Value> module);

private:
  std::map<std::string, std::string> contents;
  std::map<std::string, std::string>::iterator iter;
  static NAN_METHOD(New);
  static NAN_PROPERTY_SETTER(Setter);
  static NAN_PROPERTY_GETTER(Getter);
  static NAN_METHOD(next);
};

NAN_PROPERTY_SETTER(V8_Obj::Setter) {
  v8::String::Utf8Value data(value);
  v8::String::Utf8Value prop(property);
  auto self = Nan::ObjectWrap::Unwrap<V8_Obj>(info.This());
  self->contents.insert({ *prop, *data });
}

NAN_METHOD(V8_Obj::next) {
  auto self = static_cast<V8_Obj*>(info.Data().As<v8::External>()->Value());
  bool done = self->iter == self->contents.end();
  auto obj = Nan::New<v8::Object>();
  Nan::Set(obj, Nan::New<v8::String>("done").ToLocalChecked(),
           Nan::New<v8::Boolean>(done));
  if (!done) {
    Nan::Set(obj, Nan::New<v8::String>("value").ToLocalChecked(),
             Nan::New<v8::String>(self->iter->first.c_str()).ToLocalChecked());
  }
  self->iter++;
  info.GetReturnValue().Set(obj);
}

NAN_PROPERTY_GETTER(V8_Obj::Getter) {
  auto self = Nan::ObjectWrap::Unwrap<V8_Obj>(info.This());
  if (property->IsSymbol()) {
    if (Nan::Equals(property, v8::Symbol::GetIterator(info.GetIsolate())).FromJust()) {
      self->iter = self->contents.begin();
      auto iter = Nan::New<v8::FunctionTemplate>();
      Nan::SetCallHandler(iter, [](const Nan::FunctionCallbackInfo<v8::Value> &info) {
          auto next_template = Nan::New<v8::FunctionTemplate>();
          Nan::SetCallHandler(next_template, V8_Obj::next, info.Data());
          auto obj = Nan::New<v8::Object>();
          Nan::Set(obj, Nan::New<v8::String>("next").ToLocalChecked(),
                   next_template->GetFunction());
          info.GetReturnValue().Set(obj);
        }, Nan::New<v8::External>(self));
      info.GetReturnValue().Set(iter->GetFunction());
    }
    return;
  }
  v8::String::Utf8Value prop(property);
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
