#include "../window/window_object.h"

ewin::menu::bar::bar()
	: owner_(nullptr), event_id_(0){
	bind_properties_();
}

ewin::menu::bar::bar(const create_info &info)
	: owner_(nullptr), event_id_(0){
	bind_properties_();
	create_(true, &info);
}

ewin::menu::bar::~bar(){
	destruct_();
}

void ewin::menu::bar::bind_properties_(){
	owner.initialize_(nullptr, [this](void *prop, void *arg, common::property_access access){
		if (is_forbidden_(property_forbidden_info{ prop, access })){
			set_error_(error_type::forbidden_property);
			return;
		}

		error_value_ = error_type::nil;
		if (access == common::property_access::write)
			set_owner_(static_cast<window::object *>(arg));
		else if (access == common::property_access::read)
			*static_cast<window::object **>(arg) = owner_;
	});
}

void ewin::menu::bar::create_(bool create, const create_info *info){
	if (create && handle_ == nullptr){//Create
		low_level_create_(false);
		if (handle_ != nullptr && owner_ != nullptr && owner_->created){
			if (owner_->app != app_){
				set_error_(error_type::app_mismatch);
				return;
			}

			::SetMenu(owner_->handle, handle_);
			::DrawMenuBar(owner_->handle);
		}
	}
	else if (!create && owner_ != nullptr && event_id_ != 0u)
		owner_->events->create -= event_id_;

	container::create_(create, info);//Forward
}

void ewin::menu::bar::set_owner_(window::object *value){
	if (value == owner_)
		return;//No changes

	if (handle_ != nullptr && value != nullptr && value->created && value->app != app_){
		set_error_(error_type::app_mismatch);
		return;
	}

	set_error_(error_type::nil);
	if (owner_ != nullptr && event_id_ != 0u)//Remove previous event
		owner_->events->create -= event_id_;

	if ((owner_ = value) != nullptr){
		if (handle_ != nullptr && owner_->created){
			::SetMenu(owner_->handle, handle_);
			::DrawMenuBar(owner_->handle);
		}

		event_id_ = owner_->events->create += [this]{
			if (handle_ != nullptr){//Bind menu
				if (owner_->app == app_){
					error_value_ = error_type::nil;
					::SetMenu(owner_->handle, handle_);
					::DrawMenuBar(owner_->handle);
				}
				else//Error
					error_value_ = error_type::app_mismatch;
			}
		};
	}
	else//Reset event id
		event_id_ = 0;
}

void ewin::menu::bar::child_removed_(object &value, std::size_t index){
	container::child_removed_(value, index);
	if (handle_ != nullptr && owner_ != nullptr && owner_->created)
		::DrawMenuBar(owner_->handle);
}

void ewin::menu::bar::child_added_(object &value, std::size_t index){
	container::child_added_(value, index);
	if (handle_ != nullptr && owner_ != nullptr && owner_->created)
		::DrawMenuBar(owner_->handle);
}
