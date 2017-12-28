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
	label.initialize_(&cache_.label, handler);
	sub_menu.initialize_(nullptr, handler);

	states.initialize_(&cache_.states, handler);
	enabled.initialize_(nullptr, handler);
	is_default.initialize_(nullptr, handler);
	checked.initialize_(nullptr, handler);

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
	else if (prop == &id && access == common::property_access::write)
		update_id_();
	else if (prop == &label && access == common::property_access::write)
		update_label_();
	else if (prop == &states && access == common::property_access::write)
		update_states_();
	else if (prop == &bitmap && access == common::property_access::write)
		update_bitmap_();
	else if ((prop == &checked_bitmap || prop == &unchecked_bitmap) && access == common::property_access::write)
		update_check_marks_();
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
	app_->task += [&]{
		created_ = false;
		if (previous != nullptr)//Remove item
			::RemoveMenu(previous->handle, static_cast<common::types::uint>(previous_index), MF_BYPOSITION);

		if (current != nullptr)//Insert into new parent
			low_level_create_(current->handle, static_cast<common::types::uint>(index));
	};
}

void ewin::menu::item::low_level_create_(){
	object *parent = tree.parent;
	if (parent != nullptr){//Insert into parent
		(app_ = parent->app)->task += [&]{
			low_level_create_(parent->handle, static_cast<common::types::uint>(parent->tree.children[*this]));
		};
	}
	else//Error
		set_error_(error_type::parent_required);
}

void ewin::menu::item::low_level_create_(common::types::hmenu handle, common::types::uint index){
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
		cache_.label.data(),									//String
		static_cast<common::types::uint>(cache_.label.size()),	//String size
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

void ewin::menu::item::update_id_(common::types::word *value){
	if (value != nullptr){
		cache_.id = *value;
		return;
	}

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

void ewin::menu::item::update_label_(common::types::uptr *value, std::size_t *size){
	if (value != nullptr){
		if (*value != reinterpret_cast<common::types::uptr>(cache_.label.data()))
			cache_.label.assign(reinterpret_cast<const wchar_t *>(*value), *size);
		return;
	}

	if (created_){
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
}

void ewin::menu::item::update_sub_menu_(popup *value){
	if (value != nullptr){
		set_sub_menu_(value);
		return;
	}

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

void ewin::menu::item::update_states_(common::types::uint *value){
	if (value != nullptr){
		cache_.states = *value;
		return;
	}

	if (created_){
		update_(common::types::menu_item_info{
			sizeof(common::types::menu_item_info),					//Size
			MIIM_STATE,												//Flags
			0,														//Types
			cache_.states,											//States
		});
	}
}

void ewin::menu::item::update_types_(common::types::uint *value){
	if (value != nullptr){
		cache_.types = *value;
		return;
	}

	if (created_){
		update_(common::types::menu_item_info{
			sizeof(common::types::menu_item_info),					//Size
			MIIM_FTYPE,												//Flags
			cache_.types,											//Types
		});
	}
}

void ewin::menu::item::update_bitmap_(common::types::hbitmap *value){
	if (value != nullptr){
		cache_.bitmap = *value;
		return;
	}

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

void ewin::menu::item::update_check_marks_(common::types::hbitmap *checked, common::types::hbitmap *unchecked){
	if (checked != nullptr){
		cache_.checked_bitmap = *checked;
		cache_.unchecked_bitmap = *unchecked;
		return;
	}

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
	if (add)
		EWIN_SET(cache_.states, state);
	else//Remove
		EWIN_REMOVE(cache_.states, state);
}

bool ewin::menu::item::get_state_(common::types::uint state){
	return EWIN_IS(cache_.states, state);
}

void ewin::menu::item::set_sub_menu_(popup *value){
	if (sub_menu_ != nullptr)//Remove association
		sub_menu_->owner_ = nullptr;

	if ((sub_menu_ = value) != nullptr)//Create association
		sub_menu_->owner_ = this;
}