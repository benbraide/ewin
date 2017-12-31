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

void ewin::menu::container::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &handle)
		*static_cast<common::types::hmenu *>(arg) = handle_;
	else//Forward
		object::handle_property_(prop, arg, access);
}

void ewin::menu::container::create_(bool create, const create_info *info){
	if (!create && handle_ != nullptr){//Destroy
		app_->task += [this]{
			if (EWIN_CPP_BOOL(::DestroyMenu(handle_))){
				set_error_(error_type::nil);

				if (!app_->menu_handles_.empty()){//Remove from list
					auto entry = app_->menu_handles_.find(handle_);
					if (entry != app_->menu_handles_.end())
						app_->menu_handles_.erase(entry);
				}

				if (handle_ == app_->cached_menu_handle_.first){//Reset cached
					app_->cached_menu_handle_.first = nullptr;
					app_->cached_menu_handle_.second = nullptr;
				}

				handle_ = nullptr;
				common::types::msg msg{ nullptr, EWIN_WM_MENU_DESTROY };
				dispatch_message_(msg, nullptr);
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

		if (handle_ != nullptr){
			set_error_(error_type::nil);

			app_->menu_handles_[handle_] = this;
			app_->cached_menu_handle_.first = handle_;
			app_->cached_menu_handle_.second = this;

			common::types::menu_info info{
				sizeof(common::types::menu_info),
				MIM_STYLE,
				MNS_NOTIFYBYPOS
			};

			::SetMenuInfo(handle_, &info);

			common::types::msg msg{ nullptr, EWIN_WM_MENU_CREATE };
			dispatch_message_(msg, nullptr);
		}
		else//Failed to create
			set_error_(::GetLastError());
	};
}
