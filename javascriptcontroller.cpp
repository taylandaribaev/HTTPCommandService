#include "javascriptcontroller.h"
#include "javascriptmanager.h"

#ifdef WITH_JAVASCRIPT

using namespace v8;
using namespace std;


class JSValue
{
public:
	static std::string GetString(Handle<Object>& obj, const std::string& name)
	{
		// name
		Handle<String> meta_name_name = String::NewFromUtf8(obj->GetIsolate(), name.c_str());
		Handle<Value> meta_name_val = obj->Get(meta_name_name);

		if (meta_name_val->IsString())
		{
			v8::String::Utf8Value str(meta_name_val);
			return *str;
		}

		return "";
	}

	static Handle<Object> GetObject(Handle<Object>& obj, const std::string& name)
	{
		// name
		Handle<String> meta_name_name = String::NewFromUtf8(obj->GetIsolate(), name.c_str());
		Handle<Value> meta_name_val = obj->Get(meta_name_name);

		if (meta_name_val->IsObject())
		{
			return Handle<Object>::Cast(meta_name_val);
		}

		return Handle<Object>();
	}
};

namespace http {

bool JavaScriptControllerMethod::Validate(class IController* obj,
                        const SessionId& sessionId,
                        const ControllerArguments& arguments)
{
	JavascriptController* class_ = static_cast<JavascriptController*>(obj);
    return class_->ValidateExecute(validator_, sessionId, arguments);
}

void JavaScriptControllerMethod::Execute(class IController* obj,
                        const SessionId& sessionId,
                        const ControllerArguments& arguments,
                        ControllerOutput& outContent)
{
    JavascriptController* class_ = static_cast<JavascriptController*>(obj);
	class_->ActionExecute(function_, sessionId, arguments, outContent);

#if 0
	map<string, string> options;
	map<string, string> argumentsMap;
	map<string, string> output;

	_argumentsMap = arguments.ToArgumentMap();
	int32 result = JavascriptManager::Instance().Execute(class_->GetName() + ".js", options, arguments, output);

	outContent.append( "Result: " + std::to_string(result) + "\n" );

	for(auto& pair : output)
	{
		outContent.append( "\t" + pair.first + ": " + pair.second + "\n" );
	}
#endif
}

bool JavascriptController::Construct()
{
	isolate_ = Isolate::New();

	{
		Isolate::Scope isolate_scope(isolate_);
		HandleScope scope(isolate_);
		Handle<String> source = ReadFile(isolate_, _filename);

		if (source.IsEmpty()) 
		{
			fprintf(stderr, "Error reading '%s'.\n", _filename.c_str());
			//return 1;
		}


		script_ = source;
		//_processor = new JsHttpRequestProcessor(_isolate, source);
		if (!this->Initialize(&_options, &_outputs)) 
		{
			fprintf(stderr, "Error initializing processor.\n");
			//return 1;
		}


		HandleScope handle_scope(GetIsolate());
		v8::Local<v8::Context> context =
			v8::Local<v8::Context>::New(GetIsolate(), context_);

		// Enter this processor's context so all the remaining operations
		// take place there
		Context::Scope context_scope(context);

		Handle<String> meta_name = String::NewFromUtf8(GetIsolate(), "meta");
		Handle<Value> meta_val = context->Global()->Get(meta_name);

		if (!meta_val->IsObject())
			return false;

		Handle<Object> meta_object = Handle<Object>::Cast(meta_val);

		std::string name = JSValue::GetString(meta_object, "name");
		if(!name.empty())
		{
			_name = name;
		}

		std::string description = JSValue::GetString(meta_object, "description");
		if(!description.empty())
		{
			_description = description;
		}

		std::string validator = JSValue::GetString(meta_object, "validator");
		if(!validator.empty())
		{
			//_validatorName = validator;

			// The script compiled and ran correctly.  Now we fetch out the
			// Process function from the global object.
			Handle<String> validator_name = String::NewFromUtf8(GetIsolate(), validator.c_str());
			Handle<Value> validator_val = context->Global()->Get(validator_name);

			// If there is no Process function, or if it is not a function,
			// bail out
			if (validator_val->IsFunction()) 
			{
				// It is a function; cast it to a Function
				Handle<Function> validator_fun = Handle<Function>::Cast(validator_val);

				// Store the function in a Persistent handle, since we also want
				// that to remain after this call returns
				validator_.Reset(GetIsolate(), validator_fun);
			}
			else
			{
				fprintf(stderr, "Error not found validator.\n");
			}

		}

		Handle<Object> actions_object = JSValue::GetObject(meta_object, "actions");

		Handle<Array> properties_names = actions_object->GetPropertyNames();
		for(int i=0;i<properties_names->Length(); ++i)
		{
			Handle<Value> name_val = properties_names->Get(i);
			Handle<Value> action_val = actions_object->Get(name_val);
		
			if (action_val->IsObject())
			{
				Handle<Object> action_object = Handle<Object>::Cast(action_val);

				String::Utf8Value str(name_val);
				std::string function_name = *str;

				std::string action_name = JSValue::GetString(action_object, "name");
				std::string action_description = JSValue::GetString(action_object, "description");
				std::string action_validator = JSValue::GetString(action_object, "validator");


				
				Handle<Function> action_fun;
				Handle<Function> validator_fun;

				// Find
				Handle<String> function_string = String::NewFromUtf8(GetIsolate(), function_name.c_str());
				Handle<Value> function_val = context->Global()->Get(function_string);

				if (function_val->IsFunction()) 
				{
					action_fun = Handle<Function>::Cast(function_val);

					//v8::Persistent<v8::Function> function;
					//function.Reset(GetIsolate(), function_fun);
					//_actions.insert(std::make_pair(function_name, function));
				}
				else
				{
					fprintf(stderr, "Error not found action.\n");
				}

				// If exist validator.
				if(!action_validator.empty())
				{
					Handle<String> validator_name = String::NewFromUtf8(GetIsolate(), validator.c_str());
					Handle<Value> validator_val = context->Global()->Get(validator_name);

					if (validator_val->IsFunction()) 
					{
						validator_fun = Handle<Function>::Cast(validator_val);

						//v8::Persistent<v8::Function> validator;
						//validator.Reset(GetIsolate(), validator_fun);
						//_validators.insert(std::make_pair(action_validator, validator));
					}
					else
					{
						fprintf(stderr, "Error not found validator.\n");
					}
				}
				
				JSRegisterMethod(action_name, action_description, GetIsolate(), action_fun, validator_fun);
			}
		}
	}

    return true;
}

bool JavascriptController::Validate(const SessionId& sessionId, const ControllerArguments& arguments) 
{
	Isolate::Scope isolate_scope(isolate_);
	HandleScope scope(isolate_);

	_argumentsMap = arguments.ToArgumentMap();

	// Create a handle scope to keep the temporary object references.
	HandleScope handle_scope(GetIsolate());

	v8::Local<v8::Context> context =
		v8::Local<v8::Context>::New(GetIsolate(), context_);

	// Enter this processor's context so all the remaining operations
	// take place there
	Context::Scope context_scope(context);

	// Wrap the C++ request object in a JavaScript wrapper
	Handle<Object> request_obj = WrapMap(&_argumentsMap);

	// Set up an exception handler before calling the Process function
	TryCatch try_catch;

	// Invoke the process function, giving the global object as 'this'
	// and one argument, the request.
	const int argc = 1;
	Handle<Value> argv[argc] = { request_obj };
	v8::Local<v8::Function> validator =
		v8::Local<v8::Function>::New(GetIsolate(), validator_);

	Handle<Value> result = validator->Call(context->Global(), argc, argv);
	if (result.IsEmpty()) 
	{
		String::Utf8Value error(try_catch.Exception());
		Log(*error);
		return false;
	} 
	else 
	{
		return true;
	}

	return true;
}

bool JavascriptController::ValidateExecute(
	Persistent<Function>& validate_per,
	const SessionId& sessionId,
	const ControllerArguments& arguments)
{
	Isolate::Scope isolate_scope(isolate_);
	HandleScope scope(isolate_);

	_argumentsMap = arguments.ToArgumentMap();

	// Create a handle scope to keep the temporary object references.
	HandleScope handle_scope(GetIsolate());

	v8::Local<v8::Context> context =
		v8::Local<v8::Context>::New(GetIsolate(), context_);

	// Enter this processor's context so all the remaining operations
	// take place there
	Context::Scope context_scope(context);

	// Wrap the C++ request object in a JavaScript wrapper
	Handle<Object> request_obj = WrapMap(&_argumentsMap);

	// Set up an exception handler before calling the Process function
	TryCatch try_catch;

	// Invoke the process function, giving the global object as 'this'
	// and one argument, the request.
	const int argc = 1;
	Handle<Value> argv[argc] = { request_obj };
	v8::Local<v8::Function> validator =
		v8::Local<v8::Function>::New(GetIsolate(), validate_per);

	Handle<Value> result = validator->Call(context->Global(), argc, argv);
	if (result.IsEmpty()) 
	{
		String::Utf8Value error(try_catch.Exception());
		Log(*error);
		return false;
	} 
	else 
	{
		return true;
	}

	return true;
}

bool JavascriptController::ActionExecute(
	Persistent<Function>& action_per,
	const SessionId& sessionId,
	const ControllerArguments& arguments,
	ControllerOutput& outContent)
{
	Isolate::Scope isolate_scope(isolate_);
	HandleScope scope(isolate_);

	_argumentsMap = arguments.ToArgumentMap();

	// Create a handle scope to keep the temporary object references.
	HandleScope handle_scope(GetIsolate());

	v8::Local<v8::Context> context =
		v8::Local<v8::Context>::New(GetIsolate(), context_);

	// Enter this processor's context so all the remaining operations
	// take place there
	Context::Scope context_scope(context);

	// Wrap the C++ request object in a JavaScript wrapper
	Handle<Object> request_obj = WrapMap(&_argumentsMap);

	// Set up an exception handler before calling the Process function
	TryCatch try_catch;

	// Invoke the process function, giving the global object as 'this'
	// and one argument, the request.
	const int argc = 1;
	Handle<Value> argv[argc] = { request_obj };
	v8::Local<v8::Function> validator =
		v8::Local<v8::Function>::New(GetIsolate(), action_per);

	Handle<Value> result = validator->Call(context->Global(), argc, argv);
	if (result.IsEmpty()) 
	{
		String::Utf8Value error(try_catch.Exception());
		Log(*error);
		return false;
	} 
	else 
	{
		return true;
	}

#if 0
	Isolate::Scope isolate_scope(isolate_);
	HandleScope scope(isolate_);

	_argumentsMap = arguments.ToArgumentMap();
	_outputs.clear();
	//int32 result = JavascriptManager::Instance().Execute(_filename, options, arguments, output);

	if (!this->Process(&_argumentsMap))
	{
		outContent.append( "JS Error.\n" );
		return false;
	}
#endif
	outContent.append( "Result: OK\n" );

	for(auto& pair : _outputs)
	{
		outContent.append( "\t" + pair.first + ": " + pair.second + "\n" );
	}


	return true;
}

Handle<String> JavascriptController::ReadFile(Isolate* isolate, const string& name) 
{
	FILE* file = fopen(name.c_str(), "rb");
	if (file == NULL) 
		return Handle<String>();

	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	rewind(file);

	char* chars = new char[size + 1];
	chars[size] = '\0';
	for (int i = 0; i < size;) 
	{
		int read = static_cast<int>(fread(&chars[i], 1, size - i, file));
		i += read;
	}
	fclose(file);

	Handle<String> result =
		String::NewFromUtf8(isolate, chars, String::kNormalString, size);

	delete[] chars;
	return result;
}

} // End http.

#endif
