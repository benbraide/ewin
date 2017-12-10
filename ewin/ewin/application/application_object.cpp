#include "../window/window_object.h"

ewin::application::object::object(){
	bind_properties_();
}

void ewin::application::object::bind_properties_(){}

void ewin::application::object::handle_property_(void *prop, void *arg, common::property_access access){}

void ewin::application::object::task_(task_type &callback){
	if (std::this_thread::get_id() == thread_id_)
		callback();//Same thread
	else//Execute in thread
		get_message_window_().send_message[window_type::message_info{ WM_NULL, *reinterpret_cast<common::types::wparam *>(&callback) }];
}

ewin::application::object::window_type &ewin::application::object::get_message_window_(){
	return *message_window_;
}
