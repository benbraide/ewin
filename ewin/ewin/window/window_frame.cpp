#include "window_object.h"

ewin::window::wnd_frame::wnd_frame(object &target)
	: target_(&target){
	bind_properties_();
}

void ewin::window::wnd_frame::bind_properties_(){
	auto handler = std::bind(&wnd_frame::handle_property_, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	owner.initialize_(&target_, nullptr);
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
	if (target_->is_forbidden[prop]){
		target_->error = common::error_type::forbidden_property;
		return;
	}

	target_->error = common::error_type::nil;//Clear error
	if (prop == &visible){
		if (access == common::property_access::write){
			if (*reinterpret_cast<bool *>(arg))
				::SetWindowLongPtrW(target_->handle, GWL_STYLE, (styles_() | WS_CAPTION | WS_SYSMENU | WS_BORDER));
			else//Remove
				::SetWindowLongPtrW(target_->handle, GWL_STYLE, EWIN_REMOVE_V(styles_(), (WS_CAPTION | WS_SYSMENU | WS_BORDER)));
		}
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS_ANY(styles_(), (WS_CAPTION | WS_BORDER));
	}
	else if (prop == &show_caption){
		if (access == common::property_access::write){
			if (*reinterpret_cast<bool *>(arg))
				::SetWindowLongPtrW(target_->handle, GWL_STYLE, (styles_() | WS_CAPTION));
			else//Remove
				::SetWindowLongPtrW(target_->handle, GWL_STYLE, EWIN_REMOVE_V(styles_(), (WS_CAPTION | WS_SYSMENU)));
		}
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(styles_(), WS_CAPTION);
	}
	else if (prop == &show_system_menu){
		if (access == common::property_access::write){
			if (*reinterpret_cast<bool *>(arg))
				::SetWindowLongPtrW(target_->handle, GWL_STYLE, (styles_() | WS_CAPTION | WS_SYSMENU));
			else//Remove
				::SetWindowLongPtrW(target_->handle, GWL_STYLE, EWIN_REMOVE_V(styles_(), WS_SYSMENU));
		}
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(styles_(), WS_SYSMENU);
	}
	else if (prop == &show_border){
		if (access == common::property_access::write){
			if (*reinterpret_cast<bool *>(arg))
				::SetWindowLongPtrW(target_->handle, GWL_STYLE, (styles_() | WS_BORDER));
			else//Remove
				::SetWindowLongPtrW(target_->handle, GWL_STYLE, EWIN_REMOVE_V(styles_(), WS_BORDER));
		}
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(styles_(), WS_BORDER);
	}
	else if (prop == &enable_minimize_button){
		if (access == common::property_access::write){
			if (*reinterpret_cast<bool *>(arg))
				::SetWindowLongPtrW(target_->handle, GWL_STYLE, (styles_() | WS_MINIMIZEBOX));
			else//Remove
				::SetWindowLongPtrW(target_->handle, GWL_STYLE, EWIN_REMOVE_V(styles_(), WS_MINIMIZEBOX));
		}
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(styles_(), WS_MINIMIZEBOX);
	}
	else if (prop == &enable_maximize_button){
		if (access == common::property_access::write){
			if (*reinterpret_cast<bool *>(arg))
				::SetWindowLongPtrW(target_->handle, GWL_STYLE, (styles_() | WS_MAXIMIZEBOX));
			else//Remove
				::SetWindowLongPtrW(target_->handle, GWL_STYLE, EWIN_REMOVE_V(styles_(), WS_MAXIMIZEBOX));
		}
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(styles_(), WS_MAXIMIZEBOX);
	}
	else if (prop == &thick){
		if (access == common::property_access::write){
			if (*reinterpret_cast<bool *>(arg))
				::SetWindowLongPtrW(target_->handle, GWL_STYLE, (styles_() | WS_THICKFRAME));
			else//Remove
				::SetWindowLongPtrW(target_->handle, GWL_STYLE, EWIN_REMOVE_V(styles_(), WS_THICKFRAME));
		}
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(styles_(), WS_THICKFRAME);
	}
	else if (prop == &static_edge){
		if (access == common::property_access::write){
			if (*reinterpret_cast<bool *>(arg))
				::SetWindowLongPtrW(target_->handle, GWL_STYLE, (extended_styles_() | WS_EX_STATICEDGE));
			else//Remove
				::SetWindowLongPtrW(target_->handle, GWL_STYLE, EWIN_REMOVE_V(extended_styles_(), WS_EX_STATICEDGE));
		}
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(extended_styles_(), WS_EX_STATICEDGE);
	}
	else if (prop == &window_edge){
		if (access == common::property_access::write){
			if (*reinterpret_cast<bool *>(arg))
				::SetWindowLongPtrW(target_->handle, GWL_STYLE, (extended_styles_() | WS_EX_WINDOWEDGE));
			else//Remove
				::SetWindowLongPtrW(target_->handle, GWL_STYLE, EWIN_REMOVE_V(extended_styles_(), WS_EX_WINDOWEDGE));
		}
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(extended_styles_(), WS_EX_WINDOWEDGE);
	}
	else if (prop == &sunken_edge){
		if (access == common::property_access::write){
			if (*reinterpret_cast<bool *>(arg))
				::SetWindowLongPtrW(target_->handle, GWL_STYLE, (extended_styles_() | WS_EX_CLIENTEDGE));
			else//Remove
				::SetWindowLongPtrW(target_->handle, GWL_STYLE, EWIN_REMOVE_V(extended_styles_(), WS_EX_CLIENTEDGE));
		}
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(extended_styles_(), WS_EX_CLIENTEDGE);
	}
	else if (prop == &dialog_modal){
		if (access == common::property_access::write){
			if (*reinterpret_cast<bool *>(arg))
				::SetWindowLongPtrW(target_->handle, GWL_STYLE, (extended_styles_() | WS_EX_DLGMODALFRAME));
			else//Remove
				::SetWindowLongPtrW(target_->handle, GWL_STYLE, EWIN_REMOVE_V(extended_styles_(), WS_EX_DLGMODALFRAME));
		}
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(extended_styles_(), WS_EX_DLGMODALFRAME);
	}
}

ewin::common::types::ptr ewin::window::wnd_frame::styles_() const{
	return ::GetWindowLongPtrW(target_->handle, GWL_STYLE);
}

ewin::common::types::ptr ewin::window::wnd_frame::extended_styles_() const{
	return ::GetWindowLongPtrW(target_->handle, GWL_EXSTYLE);
}
