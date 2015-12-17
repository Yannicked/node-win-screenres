#include <node.h>
//#include <iostream>
#include <vector>
#include <Windows.h>
#include <cmath>

using namespace std;

struct monitor {
	int Id;
	int Width;
	int Height;
};

namespace screenres {

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;
using v8::Integer;



BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
	vector<monitor> *v = (vector<monitor>*)dwData;
	MONITORINFO info;
	info.cbSize = sizeof(info);
	if (GetMonitorInfo(hMonitor, &info)) {
        int width = abs(info.rcMonitor.left - info.rcMonitor.right);
        int height = abs(info.rcMonitor.top  - info.rcMonitor.bottom);
		monitor m = {
			(*v).size(),
			width,
			height
		};
		(*v).push_back(m);
    }
	return TRUE;
}

vector<monitor> GetMonitors() {
    vector<monitor> monitors;
    if (EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&monitors))
        return monitors;
    return monitors;
}

void Monitors(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = args.GetIsolate();
	vector<monitor> monitors = GetMonitors();
	
	Local<Object> robj = Object::New(isolate);
	
	for(vector<monitor>::iterator it = monitors.begin(); it != monitors.end(); ++it) {
		Local<Object> wh = Object::New(isolate);
		wh->Set(String::NewFromUtf8(isolate, "width"), Integer::New(isolate, (*it).Width));
		wh->Set(String::NewFromUtf8(isolate, "height"), Integer::New(isolate, (*it).Height));
		robj->Set(Integer::New(isolate, (*it).Id), wh);
	}
	
	args.GetReturnValue().Set(robj);
}

void init(Local<Object> exports) {
	NODE_SET_METHOD(exports, "monitors", Monitors);
}

NODE_MODULE(screenres, init)

}