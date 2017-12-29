#include "menu_separator.h"

ewin::menu::separator::separator() = default;

ewin::menu::separator::~separator() = default;

void ewin::menu::separator::create_(bool create, const create_info *info){
	if (!create && created_){//Destroy
		app_->task += [this]{
			if (::RemoveMenu(tree.parent->handle, static_cast<common::types::uint>(tree.index), MF_BYPOSITION) != FALSE){
				created_ = false;
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
	}
	else{//Failed to insert
		created_ = false;
		set_error_(::GetLastError());
	}
}
