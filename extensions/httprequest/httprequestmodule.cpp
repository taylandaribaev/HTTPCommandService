#include "httprequestmodule.h"
#include "httprequest.h"

#ifdef WITH_JAVASCRIPT
#include <v8pp/class.hpp>
#include <v8pp/function.hpp>
#include <v8pp/object.hpp>

#include <v8pp/class.hpp>
#include <v8pp/module.hpp>

using namespace v8;
#endif

using namespace openrc;

void HttpRequestModule::InitialiseModule()
{
	
}

void HttpRequestModule::ShutdownModule()
{}

#ifdef WITH_JAVASCRIPT
static Handle<Object> CreateObject(const FunctionCallbackInfo<Value>& args) 
{
	XMLHTTPRequest* obj = new XMLHTTPRequest(args);
	return v8pp::class_<XMLHTTPRequest>::import_external(args.GetIsolate(), obj);
}

Handle<Value> InitAll(Isolate* isolate)
{
	EscapableHandleScope scope(isolate);

	// Prepare class binding
	v8pp::class_<DBContext> DBContext_class(isolate);

	// Prototype
	DBContext_class
		.set("open", &DBContext::Open)
		.set("close", &DBContext::Open)

		.set("type", property(&DBContext::GetType, &DBContext::SetType))
		.set("address", property(&DBContext::GetAddress, &DBContext::SetAddress))
		.set("port", property(&DBContext::GetPort, &DBContext::SetPort))
		.set("dbName", property(&DBContext::GetDBName, &DBContext::SetDBName))
		.set("user", property(&DBContext::GetUser, &DBContext::SetUser))
		.set("password", property(&DBContext::GetPassword, &DBContext::SetPassword));

	v8pp::module m(isolate);
	m.set("DBContext", DBContext_class);

	static Persistent<Object> bindings_(isolate, m.new_instance());

	return scope.Escape(m.new_instance());
}

#if 0
void InitAll(Handle<Object> exports, Handle<Object> module) 
{
	DBContext::Register(nullptr);

	Isolate* isolate = Isolate::GetCurrent();
	v8pp::set_option(isolate, module, "exports", v8pp::wrap_function(isolate, "exports", &CreateObject));
}
#endif

V8PP_PLUGIN_INIT(v8::Isolate* isolate)
{
    return InitAll(isolate);
}
#endif

IModule* GetModule()
{
    return new HttpRequestModule();
}

