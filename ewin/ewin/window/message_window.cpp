#include "message_window.h"

ewin::window::message::~message(){
	destruct_();
}

bool ewin::window::message::is_forbidden_(const property_forbidden_info &info){
	return (
		(info.value != &error_throw_policy) &&
		(info.value != &error) &&
		(info.value != &app) &&
		(info.value != &handle) &&
		(info.value != &procedure) &&
		(info.value != &send_message) &&
		(info.value != &post_message) &&
		(info.value != &filter_styles) &&
		(info.value != &filter_extended_styles) &&
		(info.value != &filtered_styles) &&
		(info.value != &filtered_extended_styles) &&
		(info.value != &created) &&
		(info.value != &auto_destroy) &&
		(info.value != &changed) &&
		(info.value != &tree.owner) &&
		(info.value != &view.owner) &&
		(info.value != &frame.owner) &&
		(info.value != &state.owner) &&
		(info.value != &style.owner) &&
		(info.value != &attribute.owner) &&
		(info.value != &attribute.is_windowless) &&
		(info.value != &attribute.is_group) &&
		(info.value != &attribute.is_dialog) &&
		(info.value != &attribute.is_modal) &&
		(info.value != &attribute.is_message_only) &&
		(info.value != &attribute.is_control)
	);
}

void ewin::window::message::create_(bool create, const create_info *info){
	if (create && handle_ == nullptr){//Create
		low_level_create_(common::types::create_struct{
			nullptr,
			nullptr,
			nullptr,
			HWND_MESSAGE
		}, nullptr, nullptr, attribute_option_type::nil);
	}
	else//Pass to base type
		object::create_(create, info);
}

ewin::common::types::uint ewin::window::message::white_listed_styles_(bool is_extended) const{
	return 0u;
}

ewin::common::types::uint ewin::window::message::black_listed_styles_(bool is_extended) const{
	return ~0u;//Blacklist all styles
}
