#include "application_manager.h"

ewin::common::read_only_object_value_property<ewin::application::object, ewin::application::manager> ewin::application::manager::main;

ewin::common::read_only_object_value_property<ewin::application::object, ewin::application::manager> ewin::application::manager::current;

ewin::application::object *ewin::application::manager::main_ = nullptr;

thread_local std::shared_ptr<ewin::application::object> ewin::application::manager::current_;

void ewin::application::manager::bind_properties_(){
	main.initialize_(main_, nullptr);
	current.initialize_(nullptr, &handle_property_);
}

void ewin::application::manager::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &current){
		if (current_ == nullptr)
			current_.reset(new object);
		*reinterpret_cast<object **>(arg) = current_.get();
	}
}

ewin::application::manager_initializer::manager_initializer(){
	manager::current_.reset(new object);
	manager::main_ = manager::current_.get();
}
