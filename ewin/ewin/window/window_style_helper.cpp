#include "window_object.h"

ewin::window::wnd_style_helper::wnd_style_helper(object &target)
	: target_(&target), cached_value_(0u), cached_extended_value_(0u){
	owner.initialize_(target_, nullptr);
}

void ewin::window::wnd_style_helper::reset_(common::types::uint value){
	value = target_->filter_styles[value];
	if (value == 0u)//No changes
		return;

	if (target_->handle != nullptr){
		::SetWindowLongPtrW(target_->handle, GWL_STYLE, value);
		::SetWindowPos(target_->handle, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}
	else//Cache value
		cached_value_ = value;
}

void ewin::window::wnd_style_helper::reset_extended_(common::types::uint value){
	value = target_->filter_extended_styles[value];
	if (value == 0u)//No changes
		return;

	if (target_->handle != nullptr){
		::SetWindowLongPtrW(target_->handle, GWL_EXSTYLE, value);
		::SetWindowPos(target_->handle, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}
	else//Cache value
		cached_extended_value_ = value;
}

void ewin::window::wnd_style_helper::set_(common::types::uint value){
	value = target_->filter_styles[value];
	if (value == 0u)//No changes
		return;

	if (target_->handle != nullptr){
		::SetWindowLongPtrW(target_->handle, GWL_STYLE, (get_() | value));
		::SetWindowPos(target_->handle, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}
	else//Cache value
		EWIN_SET(cached_value_, value);
}

void ewin::window::wnd_style_helper::set_extended_(common::types::uint value){
	value = target_->filter_extended_styles[value];
	if (value == 0u)//No changes
		return;

	if (target_->handle != nullptr){
		::SetWindowLongPtrW(target_->handle, GWL_EXSTYLE, (get_extended_() | value));
		::SetWindowPos(target_->handle, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}
	else//Cache value
		EWIN_SET(cached_extended_value_, value);
}

void ewin::window::wnd_style_helper::remove_(common::types::uint value){
	value = target_->filter_styles[value];
	if (value == 0u)//No changes
		return;

	if (target_->handle != nullptr){
		::SetWindowLongPtrW(target_->handle, GWL_STYLE, EWIN_REMOVE_V(get_(), value));
		::SetWindowPos(target_->handle, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}
	else//Cache value
		EWIN_REMOVE(cached_value_, value);
}

void ewin::window::wnd_style_helper::remove_extended_(common::types::uint value){
	value = target_->filter_extended_styles[value];
	if (value == 0u)//No changes
		return;

	if (target_->handle != nullptr){
		::SetWindowLongPtrW(target_->handle, GWL_EXSTYLE, EWIN_REMOVE_V(get_extended_(), value));
		::SetWindowPos(target_->handle, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}
	else//Cache value
		EWIN_REMOVE(cached_extended_value_, value);
}

void ewin::window::wnd_style_helper::set_or_remove_(common::types::uint value, bool set){
	if (set)
		set_(value);
	else//Remove
		remove_(value);
}

void ewin::window::wnd_style_helper::set_or_remove_extended_(common::types::uint value, bool set){
	if (set)
		set_extended_(value);
	else//Remove
		remove_extended_(value);
}

ewin::common::types::uint ewin::window::wnd_style_helper::get_() const{
	if (target_->handle == nullptr)
		return cached_value_;
	return (static_cast<common::types::uint>(::GetWindowLongPtrW(target_->handle, GWL_STYLE)) | cached_value_);
}

ewin::common::types::uint ewin::window::wnd_style_helper::get_extended_() const{
	if (target_->handle == nullptr)
		return cached_extended_value_;
	return (static_cast<common::types::uint>(::GetWindowLongPtrW(target_->handle, GWL_EXSTYLE)) | cached_extended_value_);
}
