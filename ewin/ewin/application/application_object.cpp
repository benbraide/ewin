#include "../window/window_object.h"

ewin::application::object::object(){
	bind_properties_();
	if (current_ == nullptr){
		current_ = this;
		if (main_ == nullptr)
			main_ = this;
	}
	else//Set error
		last_error_value = error_type::multiple_app;
}

void ewin::application::object::bind_properties_(){
	last_error.initialize_(&last_error_info_, nullptr);
	last_error_owner.initialize_(&last_error_info_.owner, nullptr);
	last_error_value.initialize_(&last_error_info_.value, nullptr);
}

void ewin::application::object::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &last_error){
		if (access == common::property_access::write)
			last_error_info_ = *reinterpret_cast<error_info *>(arg);
		else if (access == common::property_access::read)
			*reinterpret_cast<error_info *>(arg) = last_error_info_;
	}
	else if (prop == &last_error_value){
		if (access == common::property_access::write)
			last_error_info_.value = *reinterpret_cast<error_type *>(arg);
		else if (access == common::property_access::read)
			*reinterpret_cast<error_type *>(arg) = last_error_info_.value;
	}
	else if (prop == &last_error)
		*reinterpret_cast<window_type **>(arg) = last_error_info_.owner;
}

void ewin::application::object::handle_static_property_(void *prop, void *arg, common::property_access access){
	if (prop == &main && access == common::property_access::read)
		*reinterpret_cast<object **>(arg) = main_;
	else if (prop == &current && access == common::property_access::read)
		*reinterpret_cast<object **>(arg) = ((current_ == nullptr) ? main_ : current_);
}

ewin::common::object_value_property<ewin::application::object> ewin::application::object::main(&handle_static_property_);

ewin::common::object_value_property<ewin::application::object> ewin::application::object::current(&handle_static_property_);

ewin::application::object *ewin::application::object::main_ = nullptr;

thread_local ewin::application::object *ewin::application::object::current_ = nullptr;
