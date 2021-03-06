#include "window_object.h"

ewin::window::wnd_view::wnd_view(object &target)
	: wnd_style_helper(target), show_type_(SW_SHOWNORMAL){
	bind_properties_();
}

void ewin::window::wnd_view::bind_properties_(){
	auto handler = EWIN_PROP_HANDLER(wnd_view);

	visible.initialize_(nullptr, handler);
	show.initialize_(nullptr, handler);
	show_async.initialize_(nullptr, handler);

	maximized.initialize_(nullptr, handler);
	minimized.initialize_(nullptr, handler);

	transparent.initialize_(nullptr, handler);
}

void ewin::window::wnd_view::handle_property_(void *prop, void *arg, common::property_access access){
	if (target_->is_forbidden[object::property_forbidden_info{ prop, access }]){
		target_->error = common::error_type::forbidden_property;
		return;
	}

	target_->error = common::error_type::nil;//Clear error
	if (prop == &visible){
		if (target_->created){
			target_->app->task += [&]{
				if (access == common::property_access::write)
					::ShowWindow(target_->handle, (*reinterpret_cast<bool *>(arg) ? SW_NORMAL : SW_HIDE));
				else if (access == common::property_access::read)
					*reinterpret_cast<bool *>(arg) = EWIN_CPP_BOOL(::IsWindowVisible(target_->handle));
			};
		}
		else if (access == common::property_access::write)
			set_or_remove_(WS_VISIBLE, *reinterpret_cast<bool *>(arg));
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(get_(), WS_VISIBLE);
	}
	else if (prop == &show){
		if (target_->created)
			::ShowWindow(target_->handle, *reinterpret_cast<common::types::uint *>(arg));
		else
			show_type_ = *reinterpret_cast<common::types::uint *>(arg);
	}
	else if (prop == &maximized){
		if (target_->created){
			target_->app->task += [&]{
				if (access == common::property_access::write)
					::ShowWindow(target_->handle, (*reinterpret_cast<bool *>(arg) ? SW_MAXIMIZE : SW_RESTORE));
				else if (access == common::property_access::read)
					*reinterpret_cast<bool *>(arg) = EWIN_CPP_BOOL(::IsZoomed(target_->handle));
			};
		}
		else if (access == common::property_access::write)
			set_or_remove_(WS_MAXIMIZE, *reinterpret_cast<bool *>(arg));
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(get_(), WS_MAXIMIZE);
	}
	else if (prop == &minimized){
		if (target_->created){
			target_->app->task += [&]{
				if (access == common::property_access::write)
					::ShowWindow(target_->handle, (*reinterpret_cast<bool *>(arg) ? SW_MINIMIZE : SW_RESTORE));
				else if (access == common::property_access::read)
					*reinterpret_cast<bool *>(arg) = EWIN_CPP_BOOL(::IsIconic(target_->handle));
			};
		}
		else if (access == common::property_access::write)
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
	else if (prop == &show_async && target_->created)
		::ShowWindowAsync(target_->handle, *reinterpret_cast<common::types::uint *>(arg));
}
