#include "main_application.h"

void ewin::application::manager::bind_properties_(){
	main.initialize_(main_, nullptr);
	current.initialize_(nullptr, handle_property_);
	application_list.initialize_(nullptr, handle_property_);
}

void ewin::application::manager::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &application_list){
		if (access == common::property_access::list_begin)
			*static_cast<object_list_iterator_type *>(arg) = (temp_application_list_ = application_list_).begin();
		else if (access == common::property_access::list_end)
			*static_cast<object_list_iterator_type *>(arg) = temp_application_list_.end();
	}
	else if (prop == &current)
		*static_cast<object **>(arg) = current_;
}

ewin::common::read_only_object_value_property<ewin::application::main_app, ewin::application::manager> ewin::application::manager::main;

ewin::common::read_only_object_value_property<ewin::application::object, ewin::application::manager> ewin::application::manager::current;

ewin::common::iterator_only_list_value_property<ewin::application::object, ewin::application::manager::object_list_iterator_type,
	ewin::application::manager::object_list_const_iterator_type, ewin::application::manager> ewin::application::manager::application_list;

ewin::application::main_app *ewin::application::manager::main_ = nullptr;

thread_local ewin::application::object *ewin::application::manager::current_ = nullptr;

ewin::application::manager::object_list_type ewin::application::manager::application_list_;

ewin::application::manager::object_list_type ewin::application::manager::temp_application_list_;
