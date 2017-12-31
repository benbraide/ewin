#include "../window/window_object.h"

ewin::menu::item::item()
	: created_(false), sub_menu_(nullptr){
	cache_ = cache_info{};
	bind_properties_();
}

ewin::menu::item::~item(){
	destruct_();
}

void ewin::menu::item::bind_properties_(){
	auto handler = EWIN_PROP_HANDLER(item);

	id.initialize_(&cache_.id, handler);
	sub_menu.initialize_(nullptr, handler);

	label.initialize_(&cache_.label, handler);
	shortcut.initialize_(&cache_.shortcut, handler);

	states.initialize_(&cache_.states, handler);
	enabled.initialize_(nullptr, handler);
	is_default.initialize_(nullptr, handler);
	checked.initialize_(nullptr, handler);
	owner_drawn.initialize_(nullptr, handler);

	bitmap.initialize_(&cache_.bitmap, handler);
	checked_bitmap.initialize_(&cache_.checked_bitmap, handler);
	unchecked_bitmap.initialize_(&cache_.unchecked_bitmap, handler);
}

void ewin::menu::item::handle_property_(void *prop, void *arg, common::property_access access){
	if (is_forbidden_(property_forbidden_info{ prop, access })){
		set_error_(error_type::forbidden_property);
		return;
	}

	error_value_ = error_type::nil;
	if (prop == &app){
		if (access == common::property_access::read)
			*static_cast<application_type **>(arg) = app_;
		else if (access == common::property_access::write && !created_)
			app_ = static_cast<application_type *>(arg);
	}
	else if (prop == &sub_menu){
		if (access == common::property_access::write){
			set_sub_menu_(static_cast<popup *>(arg));
			update_sub_menu_();
		}
		else if (access == common::property_access::read)
			*static_cast<popup **>(arg) = sub_menu_;
	}
	else if (prop == &enabled){
		if (access == common::property_access::read)
			*static_cast<bool *>(arg) = !get_state_(MFS_DISABLED);
		else if (access == common::property_access::write)
			set_state_(MFS_DISABLED, !*static_cast<bool *>(arg));
	}
	else if (prop == &is_default){
		if (access == common::property_access::read)
			*static_cast<bool *>(arg) = get_state_(MFS_DEFAULT);
		else if (access == common::property_access::write)
			set_state_(MFS_DEFAULT, *static_cast<bool *>(arg));
	}
	else if (prop == &checked){
		if (access == common::property_access::read)
			*static_cast<bool *>(arg) = get_state_(MFS_CHECKED);
		else if (access == common::property_access::write)
			set_state_(MFS_CHECKED, *static_cast<bool *>(arg));
	}
	else if (prop == &label && access == common::property_access::write){
		split_label_();
		update_label_();
	}
	else if (prop == &shortcut && access == common::property_access::write)
		update_label_();
	else if (prop == &id && access == common::property_access::write)
		update_id_();
	else if (prop == &states && access == common::property_access::write)
		update_states_();
	else if (prop == &owner_drawn)
		*static_cast<bool *>(arg) = (EWIN_IS(cache_.types, MFT_OWNERDRAW) || is_owner_drawn_());
	else if (prop == &bitmap && access == common::property_access::write)
		update_bitmap_();
	else if ((prop == &checked_bitmap || prop == &unchecked_bitmap) && access == common::property_access::write)
		update_check_marks_();
	else//Forward
		object::handle_property_(prop, arg, access);
}

void ewin::menu::item::destruct_(){
	object::destruct_();
	set_sub_menu_(nullptr);
}

void ewin::menu::item::create_(bool create, const create_info *info){
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

bool ewin::menu::item::validate_child_remove_(object &value){
	return false;
}

bool ewin::menu::item::validate_child_add_(object &value, std::size_t index){
	return false;
}

void ewin::menu::item::parent_changed_(object *current, object *previous, std::size_t index, std::size_t previous_index){
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

void ewin::menu::item::event_listener_count_changed_(events::menu_basic &e, std::size_t count){
	if (&e == &events_.draw && !is_owner_drawn_()){
		if (count == 0u){//Remove owner draw flag
			EWIN_REMOVE(cache_.types, MFT_OWNERDRAW);
			update_types_();
		}
		else if (count == 1u){//Add owner draw flag
			EWIN_SET(cache_.types, MFT_OWNERDRAW);
			update_types_();
		}
	}
}

bool ewin::menu::item::on_init_(ewin::events::menu_message &e){
	e.stop_propagation = true;
	return !get_state_(MFS_DISABLED);
}

bool ewin::menu::item::is_owner_drawn_(){
	return false;
}

void ewin::menu::item::low_level_create_(){
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

void ewin::menu::item::low_level_create_(common::types::hmenu handle, common::types::uint index){
	if (sub_menu_ != nullptr){
		if (!sub_menu_->created){
			set_error_(error_type::parent_not_created);
			return;
		}

		if (sub_menu_->app != app_){
			set_error_(error_type::app_mismatch);
			return;
		}
	}

	if (cache_.id == 0u)//Generate random id
		cache_.id = app_->integer_generator(static_cast<common::types::word>(1), std::numeric_limits<common::types::word>::max());

	common::types::uint types = 0;
	common::types::uint mask = (MIIM_ID | MIIM_DATA);
	if (!cache_.label.empty())
		EWIN_SET(mask, MIIM_STRING);

	if (types != 0u)
		EWIN_SET(mask, MIIM_FTYPE);

	if (cache_.states != 0u)
		EWIN_SET(mask, MIIM_STATE);

	if (cache_.bitmap != nullptr)
		EWIN_SET(mask, MIIM_BITMAP);

	if (cache_.checked_bitmap != nullptr || cache_.unchecked_bitmap != nullptr)
		EWIN_SET(mask, MIIM_CHECKMARKS);

	common::types::hmenu sub = nullptr;
	if (sub_menu_ != nullptr){
		sub = sub_menu_->handle;
		EWIN_SET(mask, MIIM_SUBMENU);
	}

	std::wstring joined_label, *label;
	if (!cache_.label.empty() && !cache_.shortcut.empty()){
		joined_label = (cache_.label + L"\t" + cache_.shortcut);
		label = &joined_label;
	}
	else//No shortcut
		label = &cache_.label;

	common::types::menu_item_info info{
		sizeof(common::types::menu_item_info),					//Size
		mask,													//Flags
		types,													//Types
		cache_.states,											//States
		cache_.id,												//Id
		sub,													//Sub-menu
		cache_.checked_bitmap,									//Checked bitmap
		cache_.unchecked_bitmap,								//Unchecked bitmap
		reinterpret_cast<common::types::uptr>(this),			//Data
		label->data(),											//String
		static_cast<common::types::uint>(label->size()),		//String size
		cache_.bitmap											//Item bitmap
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

void ewin::menu::item::update_id_(){
	if (!created_)
		return;

	app_->task += [this]{
		if (cache_.id == 0u)//Generate random id
			cache_.id = app_->integer_generator(static_cast<common::types::word>(1), std::numeric_limits<common::types::word>::max());

		if (created_){
			update_(common::types::menu_item_info{
				sizeof(common::types::menu_item_info),					//Size
				MIIM_ID,												//Flags
				0,														//Types
				0,														//States
				cache_.id,												//Id
			});
		}
	};
}

void ewin::menu::item::update_label_(){
	if (!created_)
		return;

	if (cache_.label.empty() || cache_.shortcut.empty()){
		update_(common::types::menu_item_info{
			sizeof(common::types::menu_item_info),					//Size
			MIIM_STRING,											//Flags
			0,														//Types
			0,														//States
			0,														//Id
			nullptr,												//Sub-menu
			nullptr,												//Checked bitmap
			nullptr,												//Unchecked bitmap
			0,														//Data
			cache_.label.data(),									//String
			static_cast<common::types::uint>(cache_.label.size()),	//String size
		});
	}
	else{//Append shortcut
		auto label = (cache_.label + L"\t" + cache_.shortcut);
		update_(common::types::menu_item_info{
			sizeof(common::types::menu_item_info),					//Size
			MIIM_STRING,											//Flags
			0,														//Types
			0,														//States
			0,														//Id
			nullptr,												//Sub-menu
			nullptr,												//Checked bitmap
			nullptr,												//Unchecked bitmap
			0,														//Data
			label.data(),											//String
			static_cast<common::types::uint>(label.size()),			//String size
		});
	}
}

void ewin::menu::item::update_sub_menu_(){
	if (created_){
		common::types::hmenu sub = nullptr;
		if (sub_menu_ != nullptr)
			sub = sub_menu_->handle;

		update_(common::types::menu_item_info{
			sizeof(common::types::menu_item_info),					//Size
			MIIM_SUBMENU,											//Flags
			0,														//Types
			0,														//States
			0,														//Id
			sub,													//Sub-menu
		});
	}
}

void ewin::menu::item::update_states_(){
	if (created_){
		update_(common::types::menu_item_info{
			sizeof(common::types::menu_item_info),					//Size
			MIIM_STATE,												//Flags
			0,														//Types
			cache_.states,											//States
		});
	}
}

void ewin::menu::item::update_types_(){
	if (created_){
		update_(common::types::menu_item_info{
			sizeof(common::types::menu_item_info),					//Size
			MIIM_FTYPE,												//Flags
			cache_.types,											//Types
		});
	}
}

void ewin::menu::item::update_bitmap_(){
	if (created_){
		update_(common::types::menu_item_info{
			sizeof(common::types::menu_item_info),					//Size
			MIIM_BITMAP,											//Flags
			0,														//Types
			0,														//States
			0,														//Id
			nullptr,												//Sub-menu
			nullptr,												//Checked bitmap
			nullptr,												//Unchecked bitmap
			0,														//Data
			nullptr,												//String
			0,														//String size
			cache_.bitmap											//Item bitmap
		});
	}
}

void ewin::menu::item::update_check_marks_(){
	if (created_){
		update_(common::types::menu_item_info{
			sizeof(common::types::menu_item_info),					//Size
			MIIM_CHECKMARKS,										//Flags
			0,														//Types
			0,														//States
			0,														//Id
			nullptr,												//Sub-menu
			cache_.checked_bitmap,									//Checked bitmap
			cache_.unchecked_bitmap,								//Unchecked bitmap
		});
	}
}

void ewin::menu::item::update_(const common::types::menu_item_info &info){
	app_->task += [&]{
		if (!EWIN_CPP_BOOL(::SetMenuItemInfoW(tree.parent->handle, static_cast<common::types::uint>(tree.index), TRUE, &info)))
			set_error_(::GetLastError());
	};
}

void ewin::menu::item::set_state_(common::types::uint state, bool add){
	if (add){
		if (EWIN_IS(state, MFS_CHECKED) && !EWIN_IS(cache_.states, MFS_CHECKED)){
			EWIN_SET(cache_.states, state);
			common::types::msg msg{ nullptr, EWIN_WM_MENU_CHECK, 1 };
			dispatch_message_(msg, nullptr);
		}
		else//No check change
			EWIN_SET(cache_.states, state);
	}
	else{//Remove
		if (EWIN_IS(state, MFS_CHECKED) && EWIN_IS(cache_.states, MFS_CHECKED)){
			EWIN_REMOVE(cache_.states, state);
			common::types::msg msg{ nullptr, EWIN_WM_MENU_CHECK, 0 };
			dispatch_message_(msg, nullptr);
		}
		else//No check change
			EWIN_REMOVE(cache_.states, state);
	}

	update_states_();
}

bool ewin::menu::item::get_state_(common::types::uint state){
	return EWIN_IS(cache_.states, state);
}

void ewin::menu::item::set_sub_menu_(popup *value){
	if (created_ && value != nullptr){
		if (!value->created){
			set_error_(error_type::parent_not_created);
			return;
		}

		if (value->app != app_){
			set_error_(error_type::app_mismatch);
			return;
		}
	}

	if (sub_menu_ != nullptr)//Remove association
		sub_menu_->owner_ = nullptr;

	if ((sub_menu_ = value) != nullptr)//Create association
		sub_menu_->owner_ = this;
}

void ewin::menu::item::split_label_(){
	auto index = cache_.label.find(L'\t');
	if (index < cache_.label.size()){//Split
		if ((index + 1) < cache_.label.size())
			cache_.shortcut.assign((cache_.label.data() + index + 1), (cache_.label.size() - index + 1));
		else//No shortcut
			cache_.shortcut.clear();
		cache_.label.erase(index);
	}
}
