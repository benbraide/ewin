#include "message_window.h"

ewin::window::message::~message(){
	destruct_();
}

bool ewin::window::message::is_forbidden_(const property_forbidden_info &info){
	return false;
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
