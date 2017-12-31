#include "../application/main_application.h"

#include "external_menu.h"

ewin::menu::external_item::external_item(object &parent, std::size_t index){
	common::types::menu_item_info info{
		sizeof(common::types::menu_item_info),
		(MIIM_ID | MIIM_STRING | MIIM_FTYPE | MIIM_STATE | MIIM_BITMAP | MIIM_CHECKMARKS | MIIM_SUBMENU)
	};

	if (EWIN_CPP_BOOL(::GetMenuItemInfoW(parent.handle, static_cast<common::types::uint>(index), TRUE, &info))){
		auto_destroy_ = false;//Prevent destruction
		parent.tree.children += *this;
		tree.index = index;

		created_ = true;
		app_ = parent.app;

		if (info.wID == 0u && !EWIN_IS(info.fType, MFT_SEPARATOR))//Generate random id
			cache_.id = app_->integer_generator(static_cast<common::types::word>(1), std::numeric_limits<common::types::word>::max());
		else//Use retrieved ID
			cache_.id = static_cast<common::types::word>(info.wID);

		if (info.cch > 0u){//Cache text
			cache_.label.resize(info.cch);
			info.fMask = MIIM_STRING;
			info.dwTypeData = cache_.label.data();
			info.cch += 1;
			if (EWIN_CPP_BOOL(::GetMenuItemInfoW(parent.handle, static_cast<common::types::uint>(index), TRUE, &info)))
				split_label_();
		}

		cache_.types = info.fType;
		cache_.states = info.fState;
		cache_.bitmap = info.hbmpItem;
		cache_.checked_bitmap = info.hbmpChecked;
		cache_.unchecked_bitmap = info.hbmpUnchecked;

		if (info.hSubMenu != nullptr){//Item is a link
			auto sub_menu = dynamic_cast<popup *>(app_->menu_handles[info.hSubMenu]);
			if (sub_menu == nullptr)//Create
				sub_menu_ = (sub_menu_ptr_ = std::make_shared<external_popup>(info.hSubMenu, *app_)).get();
			else//Use existing
				sub_menu_ = sub_menu;

			if (sub_menu_->owner == nullptr)
				sub_menu_->owner = this;
		}
	}
	else//Error
		created_ = false;
}

ewin::menu::external_item::~external_item(){
	destruct_();
}
