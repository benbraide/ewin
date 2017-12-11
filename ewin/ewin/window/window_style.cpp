#include "window_object.h"

ewin::window::wnd_style::wnd_style(object &target)
	: wnd_style_helper(target){
	bind_properties_();
}

void ewin::window::wnd_style::bind_properties_(){
	auto handler = std::bind(&wnd_style::handle_property_, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	value.initialize_(nullptr, handler);
	extended_value.initialize_(nullptr, handler);
}

void ewin::window::wnd_style::handle_property_(void *prop, void *arg, common::property_access access){
	if (target_->is_forbidden[object::property_forbidden_info{ prop, access }]){
		target_->error = common::error_type::forbidden_property;
		return;
	}

	target_->error = common::error_type::nil;//Clear error
	if (prop == &value){
		if (access == common::property_access::write)
			reset_(*reinterpret_cast<common::types::uint *>(arg));
		else if (access == common::property_access::read)
			*reinterpret_cast<common::types::uint *>(arg) = get_();
		else if (access == common::property_access::list_add)
			set_(*reinterpret_cast<common::types::uint *>(arg));
		else if (access == common::property_access::list_remove)
			remove_(*reinterpret_cast<common::types::uint *>(arg));
	}
	else if (prop == &extended_value){
		if (access == common::property_access::write)
			reset_extended_(*reinterpret_cast<common::types::uint *>(arg));
		else if (access == common::property_access::read)
			*reinterpret_cast<common::types::uint *>(arg) = get_extended_();
		else if (access == common::property_access::list_add)
			set_extended_(*reinterpret_cast<common::types::uint *>(arg));
		else if (access == common::property_access::list_remove)
			remove_extended_(*reinterpret_cast<common::types::uint *>(arg));
	}
}
