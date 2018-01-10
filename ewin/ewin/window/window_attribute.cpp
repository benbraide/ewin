#include "dialog_window.h"
#include "message_window.h"
#include "control/control_object.h"

ewin::window::wnd_attribute::wnd_attribute(object &target)
	: target_(&target){
	bind_properties_();
}

void ewin::window::wnd_attribute::bind_properties_(){
	auto handler = EWIN_PROP_HANDLER(wnd_attribute);

	owner.initialize_(target_, nullptr);

	is_windowless.initialize_(nullptr, handler);
	is_group.initialize_(nullptr, handler);

	is_dialog.initialize_(nullptr, handler);
	is_modal.initialize_(nullptr, handler);

	is_message_only.initialize_(nullptr, handler);
	is_control.initialize_(nullptr, handler);
}

void ewin::window::wnd_attribute::handle_property_(void *prop, void *arg, common::property_access access){
	if (target_->is_forbidden[object::property_forbidden_info{ prop, access }]){
		target_->error = common::error_type::forbidden_property;
		return;
	}

	target_->error = common::error_type::nil;//Clear error
	if (prop == &is_dialog)
		*static_cast<bool *>(arg) = (dynamic_cast<dialog *>(target_) != nullptr);
	else if (prop == &is_message_only)
		*static_cast<bool *>(arg) = (dynamic_cast<message *>(target_) != nullptr);
	else if (prop == &is_control)
		*static_cast<bool *>(arg) = (dynamic_cast<control::object *>(target_) != nullptr);
}
