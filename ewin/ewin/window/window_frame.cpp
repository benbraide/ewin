#include "window_object.h"

ewin::window::wnd_frame::wnd_frame(object &target)
	: wnd_style_helper(target){
	bind_properties_();
}

void ewin::window::wnd_frame::bind_properties_(){
	auto handler = EWIN_PROP_HANDLER(wnd_frame);

	visible.initialize_(nullptr, handler);
	show_caption.initialize_(nullptr, handler);
	show_system_menu.initialize_(nullptr, handler);
	show_border.initialize_(nullptr, handler);

	enable_minimize_button.initialize_(nullptr, handler);
	enable_maximize_button.initialize_(nullptr, handler);
	enable_close_button.initialize_(nullptr, handler);

	thick.initialize_(nullptr, handler);
	static_edge.initialize_(nullptr, handler);
	window_edge.initialize_(nullptr, handler);
	sunken_edge.initialize_(nullptr, handler);
	dialog_modal.initialize_(nullptr, handler);
}

void ewin::window::wnd_frame::handle_property_(void *prop, void *arg, common::property_access access){
	if (target_->is_forbidden[object::property_forbidden_info{ prop, access }]){
		target_->error = common::error_type::forbidden_property;
		return;
	}

	target_->error = common::error_type::nil;//Clear error
	if (prop == &visible){
		if (access == common::property_access::write)
			set_or_remove_((WS_CAPTION | WS_SYSMENU | WS_BORDER), *reinterpret_cast<bool *>(arg));
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS_ANY(get_(), (WS_CAPTION | WS_BORDER));
	}
	else if (prop == &show_caption){
		if (access == common::property_access::write){
			if (*reinterpret_cast<bool *>(arg))
				set_(WS_CAPTION);
			else//Remove
				remove_(WS_CAPTION | WS_SYSMENU);
		}
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(get_(), WS_CAPTION);
	}
	else if (prop == &show_system_menu){
		if (access == common::property_access::write){
			if (*reinterpret_cast<bool *>(arg))
				set_(WS_CAPTION | WS_SYSMENU);
			else//Remove
				remove_(WS_SYSMENU);
		}
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(get_(), WS_SYSMENU);
	}
	else if (prop == &show_border){
		if (access == common::property_access::write)
			set_or_remove_(WS_BORDER, *reinterpret_cast<bool *>(arg));
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(get_(), WS_BORDER);
	}
	else if (prop == &enable_minimize_button){
		if (access == common::property_access::write)
			set_or_remove_(WS_MINIMIZEBOX, *reinterpret_cast<bool *>(arg));
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(get_(), WS_MINIMIZEBOX);
	}
	else if (prop == &enable_maximize_button){
		if (access == common::property_access::write)
			set_or_remove_(WS_MAXIMIZEBOX, *reinterpret_cast<bool *>(arg));
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(get_(), WS_MAXIMIZEBOX);
	}
	else if (prop == &thick){
		if (access == common::property_access::write)
			set_or_remove_(WS_THICKFRAME, *reinterpret_cast<bool *>(arg));
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(get_(), WS_THICKFRAME);
	}
	else if (prop == &static_edge){
		if (access == common::property_access::write)
			set_or_remove_extended_(WS_EX_STATICEDGE, *reinterpret_cast<bool *>(arg));
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(get_extended_(), WS_EX_STATICEDGE);
	}
	else if (prop == &window_edge){
		if (access == common::property_access::write)
			set_or_remove_extended_(WS_EX_WINDOWEDGE, *reinterpret_cast<bool *>(arg));
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(get_extended_(), WS_EX_WINDOWEDGE);
	}
	else if (prop == &sunken_edge){
		if (access == common::property_access::write)
			set_or_remove_extended_(WS_EX_CLIENTEDGE, *reinterpret_cast<bool *>(arg));
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(get_extended_(), WS_EX_CLIENTEDGE);
	}
	else if (prop == &dialog_modal){
		if (access == common::property_access::write)
			set_or_remove_extended_(WS_EX_DLGMODALFRAME, *reinterpret_cast<bool *>(arg));
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(get_extended_(), WS_EX_DLGMODALFRAME);
	}
}
