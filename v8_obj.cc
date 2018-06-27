#include <map>
#include <nan.h>

class My_Obj : public Nan::ObjectWrap {
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

// To handle Node 10's deprecated non-isolate v8::String::Utf8Value version
#if NODE_MODULE_VERSION >= NODE_9_0_MODULE_VERSION
#define UTF8VALUE(value) (info.GetIsolate(), value)
#else
#define UTF8VALUE(value) (value)
#endif

NAN_PROPERTY_SETTER(My_Obj::Setter) {
  v8::String::Utf8Value data UTF8VALUE(value);
  v8::String::Utf8Value prop UTF8VALUE(property);
  auto self = Nan::ObjectWrap::Unwrap<My_Obj>(info.This());
  self->contents.insert({ *prop, *data });
}

NAN_METHOD(My_Obj::next) {
  auto self = Nan::ObjectWrap::Unwrap<My_Obj>(info.Data().As<v8::Object>());
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

NAN_PROPERTY_GETTER(My_Obj::Getter) {
  auto self = Nan::ObjectWrap::Unwrap<My_Obj>(info.This());
  if (property->IsSymbol()) {
    if (Nan::Equals(property, v8::Symbol::GetIterator(info.GetIsolate())).FromJust()) {
      self->iter = self->contents.begin();
      auto iter_template = Nan::New<v8::FunctionTemplate>();
      Nan::SetCallHandler(iter_template, [](const Nan::FunctionCallbackInfo<v8::Value> &info) {
          auto next_template = Nan::New<v8::FunctionTemplate>();
          Nan::SetCallHandler(next_template, My_Obj::next, info.Data());
          auto obj = Nan::New<v8::Object>();
          Nan::Set(obj, Nan::New<v8::String>("next").ToLocalChecked(),
                   next_template->GetFunction());
          info.GetReturnValue().Set(obj);
        }, info.This());
      info.GetReturnValue().Set(iter_template->GetFunction());
    }
    return;
  }
  v8::String::Utf8Value prop UTF8VALUE(property);
  auto pair = self->contents.find(*prop);
  if (pair == self->contents.end())
    return;
  info.GetReturnValue().Set(Nan::New<v8::String>(pair->second.c_str()).ToLocalChecked());
}

NAN_METHOD(My_Obj::New) {
  if (!info.IsConstructCall()) {
    Nan::ThrowError("New must be called as a constructor.");
    return;
  }

  My_Obj *obj = new My_Obj();
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

void My_Obj::Init(Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE exports, v8::Local<v8::Value> module) {
  v8::Local<v8::FunctionTemplate> tmpl = Nan::New<v8::FunctionTemplate>(New);
  tmpl->InstanceTemplate()->SetInternalFieldCount(1);
  Nan::SetNamedPropertyHandler(tmpl->InstanceTemplate(), Getter, Setter);
  Nan::Set(module.As<v8::Object>(), Nan::New("exports").ToLocalChecked(),
           Nan::GetFunction(tmpl).ToLocalChecked());
}

NODE_MODULE(v8_obj, My_Obj::Init)
