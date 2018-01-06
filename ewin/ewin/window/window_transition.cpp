#include "window_object.h"

ewin::window::wnd_transition::wnd_transition(object &owner)
	: owner_(owner){
	options_ = options{
		ewin::animation::easing_type::back,
		ewin::animation::easing_mode::out,
		0.2f,
		60
	};

	ids_ = ids{};
	bind_properties_();
}

ewin::window::wnd_transition::~wnd_transition() = default;

void ewin::window::wnd_transition::bind_properties_(){
	auto handler = EWIN_PROP_HANDLER(wnd_transition);

	size.initialize_(nullptr, handler);
	client_size.initialize_(nullptr, handler);

	position.initialize_(nullptr, handler);
	relative_position.initialize_(nullptr, handler);
}

void ewin::window::wnd_transition::handle_property_(void *prop, void *arg, common::property_access access){
	if (!owner_.created)
		return;//Not applicable

	if (prop == &size){
		if (access == common::property_access::write){
			auto value = static_cast<common::types::size *>(arg);
			if (value->cx == owner_.size.width)//Transition height
				single_<application::object>(&ids_.size, owner_.app, &owner_.size.height, value->cy);
			else if (value->cy == owner_.size.height)//Transition width
				single_<application::object>(&ids_.size, owner_.app, &owner_.size.width, value->cx);
			else//Transition all
				multiple_<application::object>(&ids_.size, owner_.app, &owner_.size, *value);
		}
		else//Read
			*static_cast<common::types::size *>(arg) = owner_.size;
	}
	else if (prop == &client_size){
		if (access == common::property_access::write){
			auto value = static_cast<common::types::size *>(arg);
			if (value->cx == owner_.client_size.width)//Transition height
				single_<application::object>(&ids_.size, owner_.app, &owner_.client_size.height, value->cy);
			else if (value->cy == owner_.client_size.height)//Transition width
				single_<application::object>(&ids_.size, owner_.app, &owner_.client_size.width, value->cx);
			else//Transition all
				multiple_<application::object>(&ids_.size, owner_.app, &owner_.client_size, *value);
		}
		else//Read
			*static_cast<common::types::size *>(arg) = owner_.client_size;
	}
	else if (prop == &position){
		if (access == common::property_access::write){
			auto value = static_cast<common::types::point *>(arg);
			if (value->x == owner_.position.x)//Transition y
				single_<application::object>(&ids_.position, owner_.app, &owner_.position.y, value->y);
			else if (value->y == owner_.position.y)//Transition x
				single_<application::object>(&ids_.position, owner_.app, &owner_.position.x, value->x);
			else//Transition all
				multiple_<application::object>(&ids_.position, owner_.app, &owner_.position, *value);
		}
		else//Read
			*static_cast<common::types::point *>(arg) = owner_.position;
	}
	else if (prop == &relative_position){
		if (access == common::property_access::write){
			auto value = static_cast<common::types::point *>(arg);
			if (value->x == owner_.relative_position.x)//Transition y
				single_<application::object>(&ids_.position, owner_.app, &owner_.relative_position.y, value->y);
			else if (value->y == owner_.relative_position.y)//Transition x
				single_<application::object>(&ids_.position, owner_.app, &owner_.relative_position.x, value->x);
			else//Transition all
				multiple_<application::object>(&ids_.position, owner_.app, &owner_.relative_position, *value);
		}
		else//Read
			*static_cast<common::types::point *>(arg) = owner_.relative_position;
	}
}
