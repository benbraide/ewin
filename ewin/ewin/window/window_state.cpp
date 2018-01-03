#include "window_object.h"

ewin::window::wnd_state::wnd_state(object &target)
	: wnd_style_helper(target){
	bind_properties_();
}

void ewin::window::wnd_state::bind_properties_(){
	auto handler = EWIN_PROP_HANDLER(wnd_state);

	enabled.initialize_(nullptr, handler);
	top_most.initialize_(nullptr, handler);

	tab_stop.initialize_(nullptr, handler);
	file_drop.initialize_(nullptr, handler);
	mouse_activate.initialize_(nullptr, handler);

	clip_children.initialize_(nullptr, handler);
	clip_siblings.initialize_(nullptr, handler);
}

void ewin::window::wnd_state::handle_property_(void *prop, void *arg, common::property_access access){
	if (target_->is_forbidden[object::property_forbidden_info{ prop, access }]){
		target_->error = common::error_type::forbidden_property;
		return;
	}

	target_->error = common::error_type::nil;//Clear error
	if (prop == &enabled){
		if (target_->created){
			target_->app->task += [&]{
				if (access == common::property_access::write)
					::EnableWindow(target_->handle, EWIN_C_BOOL(*reinterpret_cast<bool *>(arg)));
				else if (access == common::property_access::read)
					*reinterpret_cast<bool *>(arg) = EWIN_CPP_BOOL(::IsWindowEnabled(target_->handle));
			};
		}
		else if (access == common::property_access::write)
			set_or_remove_(WS_DISABLED, !*reinterpret_cast<bool *>(arg));
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = !EWIN_IS(get_(), WS_DISABLED);
	}
	else if (prop == &top_most){
		if (access == common::property_access::write)
			set_or_remove_extended_(WS_EX_TOPMOST, *reinterpret_cast<bool *>(arg));
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(get_extended_(), WS_EX_TOPMOST);
	}
	else if (prop == &tab_stop){
		if (access == common::property_access::write)
			set_or_remove_extended_(WS_TABSTOP, *reinterpret_cast<bool *>(arg));
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(get_(), WS_TABSTOP);
	}
	else if (prop == &file_drop){
		if (access == common::property_access::write)
			set_or_remove_extended_(WS_EX_ACCEPTFILES, *reinterpret_cast<bool *>(arg));
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(get_extended_(), WS_EX_ACCEPTFILES);
	}
	else if (prop == &mouse_activate){
		if (access == common::property_access::write)
			set_or_remove_extended_(WS_EX_NOACTIVATE, !*reinterpret_cast<bool *>(arg));
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = !EWIN_IS(get_extended_(), WS_EX_NOACTIVATE);
	}
	else if (prop == &clip_children){
		if (access == common::property_access::write)
			set_or_remove_extended_(WS_CLIPCHILDREN, *reinterpret_cast<bool *>(arg));
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(get_(), WS_CLIPCHILDREN);
	}
	else if (prop == &clip_siblings){
		if (access == common::property_access::write)
			set_or_remove_extended_(WS_CLIPSIBLINGS, *reinterpret_cast<bool *>(arg));
		else if (access == common::property_access::read)
			*reinterpret_cast<bool *>(arg) = EWIN_IS(get_(), WS_CLIPSIBLINGS);
	}
}
