#include <nan.h>
#include <sstream>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#define _WIN32_WINNT 0x050

using namespace v8;

static Persistent<Function> persistentCallback;
HHOOK hhkLowLevelKybd;
uv_loop_t *loop;
uv_async_t async;
std::string str;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    BOOL fEatKeystroke = FALSE;

    if (nCode == HC_ACTION)
    {
        switch (wParam)
        {
        case WM_KEYDOWN:
            PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
            std::ostringstream stream;
            stream << p->vkCode;
            str = stream.str();
            async.data = &str;
            uv_async_send(&async);
            break;
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void hook() {
  hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);

  MSG msg;
  while (!GetMessage(&msg, NULL, NULL, NULL)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
  }

   UnhookWindowsHookEx(hhkLowLevelKybd);
}

void handleKeyEvent(uv_async_t *handle) {
    std::string &keyCodeString = *(static_cast<std::string*>(handle->data));

    const unsigned argc = 1;
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    Local<Value> argv[argc] = { String::NewFromUtf8(isolate, keyCodeString.c_str()) };

    Local<Function> f = Local<Function>::New(isolate,persistentCallback);
    f->Call(isolate->GetCurrentContext()->Global(), argc, argv);
}

void RunCallback(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();

  HandleScope scope(isolate);

  Handle<Function> cb = Handle<Function>::Cast(args[0]);
  persistentCallback.Reset(isolate, cb);

  loop = uv_default_loop();

  uv_work_t req;

  int param = 0;
  uv_thread_t t_id;
  uv_thread_cb uvcb = (uv_thread_cb)hook;
  uv_async_init(loop, &async, handleKeyEvent);

  uv_thread_create(&t_id, uvcb, &param);
}

void Init(Handle<Object> exports, Handle<Object> module) {
  NODE_SET_METHOD(module, "exports", RunCallback);
}

NODE_MODULE(addon, Init)
