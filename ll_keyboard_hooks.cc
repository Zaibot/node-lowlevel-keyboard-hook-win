#include <node.h>
#include <nan.h>
#include <sstream>
#include <string>
#include <iostream>
#include <thread>
#include <random>
#include <stdlib.h>
#include <Windows.h>
#define _WIN32_WINNT 0x050

using namespace v8;

Local<Function> cb;
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

void fake_download(uv_work_t *req) {
  hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);

  MSG msg;
  while (!GetMessage(&msg, NULL, NULL, NULL)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  UnhookWindowsHookEx(hhkLowLevelKybd);
}

void print_progress(uv_async_t *handle) {
    std::string &keyCodeString = *(static_cast<std::string*>(handle->data));

    printf("%s", keyCodeString);
    printf("%s", "\n");

    const unsigned argc = 1;
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    Local<Value> argv[argc] = { String::NewFromUtf8(isolate, "Testing123") };
    // Uncommenting this breaks everything...  With no error...
    cb->Call(isolate->GetCurrentContext()->Global(), argc, argv);
}

void RunCallback(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();

  HandleScope scope(isolate);

  cb = Local<Function>::Cast(args[0]);

  loop = uv_default_loop();

  uv_work_t req;
  int size = 10240;
  req.data = (void*) &size;

  int tracklen = 10;
  uv_thread_t hare_id;
  uv_thread_cb uvcb = (uv_thread_cb)hook;
  uv_async_init(loop, &async, print_progress);

  uv_thread_create(&hare_id, uvcb, &tracklen);
}

void Init(Handle<Object> exports, Handle<Object> module) {
  NODE_SET_METHOD(module, "exports", RunCallback);
}

NODE_MODULE(addon, Init)
