#include "../window/window_object.h"

ewin::menu::popup_tracker::popup_tracker(popup &owner)
	: owner_(owner), flags_(::GetSystemMetrics(SM_MENUDROPALIGNMENT)){
	flags.initialize_(&flags_, nullptr);
	position.initialize_(&position_, nullptr);

	target.initialize_(nullptr, [this](void *prop, void *arg, common::property_access access){
		if (access == common::property_access::read)
			*static_cast<window::object **>(arg) = target_;
		else if (access == common::property_access::write)
			target_ = static_cast<window::object *>(arg);
	});

	activate.initialize_(nullptr, [this](void *prop, void *arg, common::property_access access){
		if (!*static_cast<bool *>(arg))
			return;

		if (!owner_.created){
			owner_.error = common::error_type::menu_not_created;
			return;
		}

		if (target_ == nullptr){
			owner_.error = common::error_type::bad_track_target;
			return;
		}

		if (!EWIN_CPP_BOOL(::TrackPopupMenuEx(owner_.handle, flags_, position_.x, position_.y, target_->handle, nullptr)))
			owner_.error = ::GetLastError();
		else//Clear error
			owner_.error = common::error_type::nil;
	});
}

ewin::menu::popup::popup()
	: owner_(nullptr), tracker(*this){
	bind_properties_();
}

ewin::menu::popup::popup(const create_info &info)
	: owner_(nullptr), tracker(*this){
	bind_properties_();
	create_(true, &info);
}

ewin::menu::popup::~popup(){
	destruct_();
}

void ewin::menu::popup::bind_properties_(){
	owner.initialize_(nullptr, [this](void *prop, void *arg, common::property_access access){
		if (access == common::property_access::write){
			if (owner_ != nullptr)//Remove association
				owner_->sub_menu = nullptr;

			if ((owner_ = static_cast<item *>(arg)) != nullptr)//Create association
				owner_->sub_menu = this;
		}
		else if (access == common::property_access::read)
			*static_cast<item **>(arg) = owner_;
	});
}

void ewin::menu::popup::destruct_(){
	container::destruct_();
	if (owner_ != nullptr)//Remove association
		owner_->sub_menu = nullptr;
}

void ewin::menu::popup::create_(bool create, const create_info *info){
	if (create && handle_ == nullptr)//Create
		low_level_create_(false);
	container::create_(create, info);//Forward
}
