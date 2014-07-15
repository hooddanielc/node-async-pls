#include <node.h>
#include <v8.h>
#include <iostream>

using namespace v8;

struct js_work {
  uv_work_t req;
  Persistent<Function> callback;
};

void run_work(uv_work_t* req) {
  js_work* work = static_cast<js_work*>(req->data);

  for(int i = 0; i < 2000000; ++i) {
    std::cout << "working: " << i << std::endl;
  }
}

void run_callback(uv_work_t* req, int status) {
  HandleScope scope;

  js_work *work = static_cast<js_work*>(req->data);

  const unsigned argc = 1;
  Local<Value> argv[argc] = { Local<Value>::New(String::New("hello world")) };
  work->callback->Call(Context::GetCurrent()->Global(), argc, argv);

  // properly cleanup, or death by millions of tiny leaks
  work->callback.Dispose();
  work->callback.Clear();

  delete work;
}

Handle<Value> DoAlotOfWork(const Arguments& args) {
  if(!args[0]->IsFunction()) {
    return ThrowException(Exception::TypeError(String::New("First Parameter must be a function")));
  }

  js_work *work = new js_work;
  work->req.data = work;
  work->callback = Persistent<Function>::New(args[0].As<Function>());

  uv_queue_work(uv_default_loop(), &work->req, run_work, run_callback);

  return Undefined();
}

void init(Handle<Object> exports) {
  exports->Set(String::NewSymbol("lol"),
      FunctionTemplate::New(DoAlotOfWork)->GetFunction());
}

NODE_MODULE(asyncpls, init)
