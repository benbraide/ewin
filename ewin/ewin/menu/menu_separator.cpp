#include "menu_separator.h"

ewin::menu::separator::separator()
	: created_(false), types_(0){
	owner_drawn.initialize_(nullptr, [this](void *prop, void *arg, common::property_access access){
		*static_cast<bool *>(arg) = (EWIN_IS(types_, MFT_OWNERDRAW) || is_owner_drawn_());
	});
}

ewin::menu::separator::~separator() = default;

void ewin::menu::separator::create_(bool create, const create_info *info){
	if (!create && created_){//Destroy
		app_->task += [this]{
			if (::RemoveMenu(tree.parent->handle, static_cast<common::types::uint>(tree.index), MF_BYPOSITION) != FALSE){
				created_ = false;
				common::types::msg msg{ nullptr, EWIN_WM_MENU_DESTROY };
				dispatch_message_(msg, nullptr);
			}
			else//Failed to destroy
				set_error_(::GetLastError());
		};
	}
	else if (create && !created_)//Create
		low_level_create_();
}

bool ewin::menu::separator::validate_child_remove_(object &value){
	return false;
}

bool ewin::menu::separator::validate_child_add_(object &value, std::size_t index){
	return false;
}

void ewin::menu::separator::parent_changed_(object *current, object *previous, std::size_t index, std::size_t previous_index){
	app_ = nullptr;
	if (previous != nullptr && previous->created){//Remove item
		previous->app->task += [&]{
			::RemoveMenu(previous->handle, static_cast<common::types::uint>(previous_index), MF_BYPOSITION);
		};
	}

	if (created_ && current != nullptr){//Insert item
		(app_ = current->app)->task += [&]{
			created_ = false;
			low_level_create_(current->handle, static_cast<common::types::uint>(index));
		};
	}
	else//Reset
		created_ = false;
}

void ewin::menu::separator::event_listener_count_changed_(events::menu_basic &e, std::size_t count){
	if (events_ != nullptr  && &e == &events_->draw && !is_owner_drawn_()){
		if (count == 0u){//Remove owner draw flag
			EWIN_REMOVE(types_, MFT_OWNERDRAW);
			update_types_();
		}
		else if (count == 1u){//Add owner draw flag
			EWIN_SET(types_, MFT_OWNERDRAW);
			update_types_();
		}
	}
}

bool ewin::menu::separator::is_owner_drawn_(){
	return false;
}

void ewin::menu::separator::low_level_create_(){
	object *parent = tree.parent;
	if (parent != nullptr){//Insert into parent
		if (!parent->created){
			set_error_(error_type::parent_not_created);
			return;
		}

		(app_ = parent->app)->task += [&]{
			low_level_create_(parent->handle, static_cast<common::types::uint>(parent->tree.children[*this]));
		};
	}
	else//Error
		set_error_(error_type::parent_required);
}

void ewin::menu::separator::low_level_create_(common::types::hmenu handle, common::types::uint index){
	common::types::menu_item_info info{
		sizeof(common::types::menu_item_info),					//Size
		MIIM_FTYPE,												//Flags
		MFT_SEPARATOR											//Types
	};

	if (EWIN_CPP_BOOL(::InsertMenuItemW(handle, index, TRUE, &info))){
		created_ = true;
		set_error_(error_type::nil);

		common::types::msg msg{ nullptr, EWIN_WM_MENU_CREATE };
		dispatch_message_(msg, nullptr);
	}
	else{//Failed to insert
		created_ = false;
		set_error_(::GetLastError());
	}
}

void ewin::menu::separator::update_types_(){
	if (created_){
		update_(common::types::menu_item_info{
			sizeof(common::types::menu_item_info),					//Size
			MIIM_FTYPE,												//Flags
			(MFT_SEPARATOR | types_),								//Types
		});
	}
}

void ewin::menu::separator::update_(const common::types::menu_item_info &info){
	app_->task += [&]{
		if (!EWIN_CPP_BOOL(::SetMenuItemInfoW(tree.parent->handle, static_cast<common::types::uint>(tree.index), TRUE, &info)))
			set_error_(::GetLastError());
	};
}
