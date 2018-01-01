#include "control_object.h"

ewin::window::control::object::object(){
	bind_properties_();
}

ewin::window::control::object::~object(){
	destruct_();
}

void ewin::window::control::object::bind_properties_(){
	auto handler = EWIN_PROP_HANDLER(object);

	id.initialize_(&id_, nullptr);
	font.initialize_(&font_, nullptr);

	relative_size.initialize_(&relative_size_, handler);
	padding.initialize_(&padding_, handler);
}

void ewin::window::control::object::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop != &relative_size && prop != &padding){
		if (prop == &size || prop == &client_size || prop == &rect || prop == &relative_rect || prop == &client_rect)
			relative_size_.width = relative_size_.height = 0.0f;//Clear relative size
		base_type::handle_property_(prop, arg, access);
	}
	else//Resize if size is relative
		resize_();
}

void ewin::window::control::object::resize_(){
	if (handle_ != nullptr && (relative_size_.width > 0.0f || relative_size_.height > 0.0f)){//Update size
		compute_size_();
		::SetWindowPos(handle_, nullptr, 0, 0, cache_.size.cx, cache_.size.cy, (SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE));
	}
}

void ewin::window::control::object::compute_size_(){

}
