#include "button_control.h"

ewin::window::control::button::button() = default;

ewin::window::control::button::~button(){
	destruct_();
}

void ewin::window::control::button::compute_size_(){
	app_->task += [this]{
		common::types::size size;
		if (EWIN_CPP_BOOL(Button_GetIdealSize(handle_, &size))){
			cache_.size = common::types::size{
				static_cast<int>((relative_size_.width * size.cx) + padding_.cx),
				static_cast<int>((relative_size_.height * size.cy) + padding_.cy)
			};
		}
	};

	compute_additional_size_();
}
