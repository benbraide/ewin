#include "../application/main_application.h"

#include "menu_container.h"

ewin::menu::container::container()
	: handle_(nullptr){}

ewin::menu::container::~container(){
	destruct_();
}

ewin::common::types::hmenu ewin::menu::container::handle_value_(){
	return handle_;
}

void ewin::menu::container::create_(bool create, const create_info *info){
	if (!create && handle_ != nullptr){//Destroy
		app_->task += [this]{
			if (EWIN_CPP_BOOL(::DestroyMenu(handle_))){
				handle_ = nullptr;
			}
			else//Failed to destroy
				set_error_(::GetLastError());
		};
	}
}

bool ewin::menu::container::validate_parent_change_(object *value){
	return (value == nullptr);
}

void ewin::menu::container::low_level_create_(bool popup){
	if (app_ == nullptr && (app_ = application::manager::main) == nullptr){//App required
		set_error_(error_type::no_app);
		return;
	}

	app_->task += [&]{
		if (popup)
			handle_ = ::CreatePopupMenu();
		else//Bar
			handle_ = ::CreateMenu();

		if (handle_ == nullptr)//Failed to create
			set_error_(::GetLastError());
	};
}
