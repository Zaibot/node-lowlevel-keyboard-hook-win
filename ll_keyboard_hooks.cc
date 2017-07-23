#include <nan.h>
#include <sstream>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#define _WIN32_WINNT 0x050

using namespace v8;

#define MODE_UP 0x01
#define MODE_DOWN 0x02

static Persistent<Function> persistentCallback;
HHOOK hhkLowLevelKybd;
uv_loop_t *loop;
uv_async_t async;
int32_t mode = 0x00;
std::string str;
volatile int running = 0;

void stop() {
    running = 0;
    uv_close((uv_handle_t*)&async, NULL);
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        std::ostringstream stream;
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
        if (wParam == WM_KEYDOWN && (mode & MODE_DOWN) == MODE_DOWN) {
            stream << p->vkCode;
            str = "down::" + stream.str();
            async.data = &str;
            uv_async_send(&async);
        }
        if (wParam == WM_KEYUP && (mode & MODE_UP) == MODE_UP) {
            stream << p->vkCode;
            str = "up::" + stream.str();
            async.data = &str;
            uv_async_send(&async);
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void hook() {
  hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);

  MSG msg;
  while (running && !GetMessage(&msg, NULL, NULL, NULL)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
  }

   UnhookWindowsHookEx(hhkLowLevelKybd);
}

void handleKeyEvent(uv_async_t *handle) {
    std::string &strEvent = *(static_cast<std::string*>(handle->data));

    const unsigned argc = 1;
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    Local<Value> argv[argc] = { String::NewFromUtf8(isolate, strEvent.c_str()) };

    Local<Function> f = Local<Function>::New(isolate,persistentCallback);
    f->Call(isolate->GetCurrentContext()->Global(), argc, argv);
}

uv_thread_t t_id;

void RunCallback(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();

  HandleScope scope(isolate);

  Handle<Number> hMode = Handle<Number>::Cast(args[0]);
  mode = hMode->Int32Value();

  Handle<Function> cb = Handle<Function>::Cast(args[1]);
  persistentCallback.Reset(isolate, cb);

  loop = uv_default_loop();
  running = 1;

  uv_work_t req;

  int param = 0;
  uv_thread_cb uvcb = (uv_thread_cb)hook;
  uv_async_init(loop, &async, handleKeyEvent);

  uv_thread_create(&t_id, uvcb, &param);
}

void StopCallback(const FunctionCallbackInfo<Value>& args) {
    stop();
}

void Init(Handle<Object> exports, Handle<Object> module) {
  NODE_SET_METHOD(exports, "run", RunCallback);
  NODE_SET_METHOD(exports, "stop", StopCallback);
}

NODE_MODULE(addon, Init)
