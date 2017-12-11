#include "window_object.h"

ewin::window::wnd_attribute::wnd_attribute(object &target) : target_(&target){
	bind_properties_();
}

void ewin::window::wnd_attribute::bind_properties_(){
	auto handler = std::bind(&wnd_attribute::handle_property_, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	owner.initialize_(target_, nullptr);

	is_windowless.initialize_(nullptr, handler);
	is_group.initialize_(nullptr, handler);

	is_dialog.initialize_(nullptr, handler);
	is_modal.initialize_(nullptr, handler);

	is_message_only.initialize_(nullptr, handler);
	is_control.initialize_(nullptr, handler);
}

void ewin::window::wnd_attribute::handle_property_(void *prop, void *arg, common::property_access access){
	if (target_->is_forbidden[prop]){
		target_->error = common::error_type::forbidden_property;
		return;
	}

	target_->error = common::error_type::nil;//Clear error
}
