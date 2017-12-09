#include "window_object.h"

ewin::window::wnd_view::wnd_view(object &target)
	: wnd_style_helper(target), show_type_(SW_SHOWNORMAL){
	bind_properties_();
}

void ewin::window::wnd_view::bind_properties_(){
	auto handler = std::bind(&wnd_view::handle_property_, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	visible.initialize_(nullptr, handler);
	show.initialize_(nullptr, handler);
	show_async.initialize_(nullptr, handler);

	maximized.initialize_(nullptr, handler);
	minimized.initialize_(nullptr, handler);

	transparent.initialize_(nullptr, handler);
}

void ewin::window::wnd_view::handle_property_(void *prop, void *arg, common::property_access access){
	if (target_->is_forbidden[prop]){
		target_->error = common::error_type::forbidden_property;
		return;
	}

	target_->error = common::error_type::nil;//Clear error
	if (prop == &visible){
		if (access == common::property_access::write)
			set_or_remove_(WS_VISIBLE, *reinterpret_cast<bool *>(arg));
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(get_(), WS_VISIBLE);
	}
	else if (prop == &show){
		if (target_->handle == nullptr)
			show_type_ = *reinterpret_cast<common::types::uint *>(arg);
		else//Use
			::ShowWindow(target_->handle, *reinterpret_cast<common::types::uint *>(arg));
	}
	else if (prop == &maximized){
		if (access == common::property_access::write)
			set_or_remove_(WS_MAXIMIZE, *reinterpret_cast<bool *>(arg));
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(get_(), WS_MAXIMIZE);
	}
	else if (prop == &minimized){
		if (access == common::property_access::write)
			set_or_remove_(WS_MINIMIZE, *reinterpret_cast<bool *>(arg));
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(get_(), WS_MINIMIZE);
	}
	else if (prop == &transparent){
		if (access == common::property_access::write)
			set_or_remove_extended_(WS_EX_TRANSPARENT, *reinterpret_cast<bool *>(arg));
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(get_extended_(), WS_EX_TRANSPARENT);
	}
	else if (prop == &show_async && target_->handle != nullptr)
		::ShowWindowAsync(target_->handle, *reinterpret_cast<common::types::uint *>(arg));
}
