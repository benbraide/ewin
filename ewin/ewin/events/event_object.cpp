#include "event_callback.h"

ewin::events::object::object(window_type *target)
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

void ewin::events::message::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &result){
		if (access == common::property_access::read){
			if (EWIN_IS(states_, state_type::default_prevented))
				*static_cast<common::types::result *>(arg) = result_;
			else//Call default
				*static_cast<common::types::result *>(arg) = call_default_();
		}
		else if (access == common::property_access::write){
			result_ = *static_cast<common::types::result *>(arg);
			EWIN_SET(states_, state_type::default_prevented);
		}
	}
}

ewin::common::types::result ewin::events::message::call_default_(){
	if (EWIN_IS_ANY(states_, state_type::default_prevented | state_type::bubbled))
		return result_;//Return previous result
	return (result_ = callback_(*this));
}

void ewin::events::mouse_activate::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &hit_target)
		*static_cast<common::types::uint *>(arg) = LOWORD(msg_->lParam);
	else if (prop == &mouse_message)
		*static_cast<common::types::uint *>(arg) = HIWORD(msg_->lParam);
	else//Forward
		message::handle_property_(prop, arg, access);
}
