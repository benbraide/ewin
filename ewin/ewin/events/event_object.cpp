#include "../window/window_object.h"

ewin::events::object::object(target_type *target)
	: target_(target), states_(state_type::nil){
	bind_properties_();
}

void ewin::events::object::bind_properties_(){
	auto handler = EWIN_PROP_HANDLER(object);

	target.initialize_(target_, nullptr);
	prevent_default.initialize_(nullptr, handler);
	stop_propagation.initialize_(nullptr, handler);
	bubble.initialize_(nullptr, handler);
}

void ewin::events::object::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &prevent_default){
		if (access == common::property_access::read)
			*static_cast<bool *>(arg) = EWIN_IS(states_, state_type::default_prevented);
		else if (access == common::property_access::write && *static_cast<bool *>(arg))
			prevent_default_();
	}
	else if (prop == &stop_propagation){
		if (access == common::property_access::read)
			*static_cast<bool *>(arg) = EWIN_IS(states_, state_type::propagation_stopped);
		else if (access == common::property_access::write && *static_cast<bool *>(arg))
			EWIN_SET(states_, state_type::propagation_stopped);
	}
	else if (prop == &bubble){
		if (access == common::property_access::read)
			*static_cast<bool *>(arg) = EWIN_IS(states_, state_type::bubbled);
		else if (access == common::property_access::write && *static_cast<bool *>(arg))
			EWIN_SET(states_, state_type::bubbled);
	}
}

void ewin::events::object::prevent_default_(){
	EWIN_SET(states_, state_type::default_prevented);
}

ewin::events::message::~message() = default;

void ewin::events::message::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &result){
		if (access == common::property_access::write){
			result_ = *static_cast<common::types::result *>(arg);
			EWIN_SET(states_, state_type::default_called);
		}
		else if (access == common::property_access::read)
			*static_cast<common::types::result *>(arg) = call_default_();
	}
	else//Forward
		object::handle_property_(prop, arg, access);
}

ewin::common::types::result ewin::events::message::call_default_(){
	if (!EWIN_IS_ANY(states_, state_type::default_called | state_type::default_prevented | state_type::bubbled))
		result_ = callback_(*this);//Call
	EWIN_SET(states_, state_type::default_called);
	return result_;
}

void ewin::events::message::remove_bubble_(){
	EWIN_SET(states_, state_type::bubbled);
}

void ewin::events::mouse_activate::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &hit_target)
		*static_cast<common::types::uint *>(arg) = LOWORD(msg_->lParam);
	else if (prop == &mouse_message)
		*static_cast<common::types::uint *>(arg) = HIWORD(msg_->lParam);
	else//Forward
		message::handle_property_(prop, arg, access);
}

void ewin::events::pre_activate::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &is_activating)
		*static_cast<bool *>(arg) = EWIN_CPP_BOOL(msg_->wParam);
	else//Forward
		message::handle_property_(prop, arg, access);
}

void ewin::events::activate::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &is_activated)
		*static_cast<common::types::uint *>(arg) = (msg_->wParam != WA_INACTIVE);
	else if (prop == &is_click_activated)
		*static_cast<common::types::uint *>(arg) = (msg_->wParam == WA_CLICKACTIVE);
	else//Forward
		message::handle_property_(prop, arg, access);
}

void ewin::events::enable::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &is_enabled)
		*static_cast<bool *>(arg) = EWIN_CPP_BOOL(msg_->wParam);
	else//Forward
		message::handle_property_(prop, arg, access);
}

void ewin::events::hit_test::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &mouse_position)
		*static_cast<common::types::point *>(arg) = common::types::point{ GET_X_LPARAM(msg_->lParam), GET_Y_LPARAM(msg_->lParam) };
	else//Forward
		message::handle_property_(prop, arg, access);
}

ewin::events::style_change::~style_change(){
	auto info = reinterpret_cast<common::types::style_struct *>(msg_->lParam);
	if (info->styleNew != info->styleOld){//Filter styles
		auto window_target = dynamic_cast<window_type *>(target_);
		if (msg_->wParam == GWL_EXSTYLE){
			auto styles_removed = window_target->filter_extended_styles[EWIN_REMOVE_V(info->styleOld, info->styleNew)];
			auto styles_added = window_target->filter_extended_styles[EWIN_REMOVE_V(info->styleNew, info->styleOld)];
			info->styleNew = EWIN_REMOVE_V(EWIN_SET_V(info->styleOld, styles_added), styles_removed);
		}
		else{//Normal styles
			auto styles_removed = window_target->filter_styles[EWIN_REMOVE_V(info->styleOld, info->styleNew)];
			auto styles_added = window_target->filter_styles[EWIN_REMOVE_V(info->styleNew, info->styleOld)];
			info->styleNew = EWIN_REMOVE_V(EWIN_SET_V(info->styleOld, styles_added), styles_removed);
		}
	}
}

void ewin::events::style_change::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &is_extended)
		*static_cast<common::types::uint *>(arg) = (msg_->wParam == GWL_EXSTYLE);
	else if (prop == &styles)
		*static_cast<common::types::style_struct **>(arg) = reinterpret_cast<common::types::style_struct *>(msg_->lParam);
	else//Forward
		base_type::handle_property_(prop, arg, access);
}
