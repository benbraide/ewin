#include "window_object.h"

ewin::window::object::object()
	: error_throw_policy_(error_throw_policy_type::always), auto_destroy_(true), frame_(*this){
	bind_properties_();
}

ewin::window::object::~object(){
	if (auto_destroy_){
		try{
			create_(false, nullptr);
		}
		catch (...){}
	}
}

void ewin::window::object::bind_properties_(){
	auto handler = std::bind(&object::handle_property_, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	reflect.initialize_(nullptr, handler);
	is_forbidden.initialize_(nullptr, handler);

	error_throw_policy.initialize_(&error_throw_policy_, nullptr);
	error.initialize_(nullptr, handler);

	app.initialize_(nullptr, handler);
	handle.initialize_(&handle_, nullptr);
	procedure.initialize_(&procedure_, nullptr);

	size.initialize_(nullptr, handler);
	relative_size.initialize_(nullptr, handler);

	offset.initialize_(nullptr, handler);
	relative_offset.initialize_(nullptr, handler);

	rect.initialize_(nullptr, handler);
	relative_rect.initialize_(nullptr, handler);

	tree.initialize_(&frame_, nullptr);
	view.initialize_(&frame_, nullptr);
	frame.initialize_(&frame_, nullptr);
	style.initialize_(&frame_, nullptr);
	state.initialize_(&frame_, nullptr);

	created.initialize_(nullptr, handler);
	create.initialize_(nullptr, handler);
	auto_destroy.initialize_(&auto_destroy_, nullptr);
}

void ewin::window::object::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &reflect){
		*static_cast<ptr_type *>(arg) = reflect_();
	}
	else if (prop == &is_forbidden){
		*static_cast<bool *>(arg) = is_forbidden_(arg);
	}
	else if (prop == &error){
		if (access == common::property_access::write){
			switch (static_cast<error_throw_policy_type>(error_throw_policy_)){
			case error_throw_policy_type::never://Update last error
				if (application_type::current != nullptr)
					application_type::current->last_error = application_type::error_info{ this, *static_cast<error_type *>(arg) };
				else//Error
					throw error_type::no_app;
				break;
			case error_throw_policy_type::once://Throw once
				error_throw_policy_ = error_throw_policy_type::never;
				throw *static_cast<error_type *>(arg);
				break;
			default:
				throw *static_cast<error_type *>(arg);
				break;
			}
		}
		else if (access == common::property_access::read){
			if (application_type::current != nullptr){
				if (application_type::current->last_error_owner == this)
					*static_cast<error_type *>(arg) = application_type::current->last_error_value;
				else//No error
					*static_cast<error_type *>(arg) = error_type::nil;
			}
			else//Error
				throw error_type::no_app;
		}
		
	}
	else if (prop == &app){
		if (access == common::property_access::read)
			*static_cast<application_type **>(arg) = app_;
		else if (access == common::property_access::write && handle_ == nullptr)
			app_ = *static_cast<application_type **>(arg);
	}
	else if (prop == &size){
		if (access == common::property_access::read)
			*static_cast<common::types::size *>(arg) = get_size_();
		else if (access == common::property_access::write)
			set_size_(*static_cast<common::types::size *>(arg));
	}

	/*if (prop == &instance){
		if (access == common::property_access::read)
			*static_cast<common::types::hinstance *>(arg) = info_.hInstance;
		else if (access == common::property_access::write)
			info_.hInstance = *static_cast<common::types::hinstance *>(arg);
	}
	else if (prop == &procedure){
		if (access == common::property_access::read)
			*static_cast<common::types::procedure *>(arg) = info_.lpfnWndProc;
		else if (access == common::property_access::write)
			info_.lpfnWndProc = *static_cast<common::types::procedure *>(arg);
	}
	else if (prop == &name){
		if (access == common::property_access::read)
			*static_cast<std::wstring *>(arg) = name_;
		else if (access == common::property_access::write)
			info_.lpszClassName = (name_ = *static_cast<std::wstring *>(arg)).data();
	}
	else if (prop == &menu){
		if (access == common::property_access::read)
			*static_cast<std::wstring *>(arg) = menu_;
		else if (access == common::property_access::write)
			info_.lpszMenuName = (menu_ = *static_cast<std::wstring *>(arg)).data();
	}
	else if (prop == &style){
		if (access == common::property_access::read)
			*static_cast<common::types::uint *>(arg) = info_.style;
		else if (access == common::property_access::write)
			info_.style = *static_cast<common::types::uint *>(arg);
	}
	else if (prop == &background_brush){
		if (access == common::property_access::read)
			*static_cast<common::types::hbrush *>(arg) = info_.hbrBackground;
		else if (access == common::property_access::write)
			info_.hbrBackground = *static_cast<common::types::hbrush *>(arg);
	}
	else if (prop == &small_icon){
		if (access == common::property_access::read)
			*static_cast<common::types::hicon *>(arg) = info_.hIconSm;
		else if (access == common::property_access::write)
			info_.hIconSm = *static_cast<common::types::hicon *>(arg);
	}
	else if (prop == &icon){
		if (access == common::property_access::read)
			*static_cast<common::types::hicon *>(arg) = info_.hIcon;
		else if (access == common::property_access::write)
			info_.hIcon = *static_cast<common::types::hicon *>(arg);
	}
	else if (prop == &wnd_extra){
		if (access == common::property_access::read)
			*static_cast<int *>(arg) = info_.cbWndExtra;
		else if (access == common::property_access::write)
			info_.cbWndExtra = *static_cast<int *>(arg);
	}
	else if (prop == &cls_extra){
		if (access == common::property_access::read)
			*static_cast<int *>(arg) = info_.cbClsExtra;
		else if (access == common::property_access::write)
			info_.cbClsExtra = *static_cast<int *>(arg);
	}
	else if (prop == &created){
		if (access == common::property_access::read)
			*static_cast<bool *>(arg) = (id_ != static_cast<common::types::atom>(0));
		else if (access == common::property_access::write)
			create_(*static_cast<bool *>(arg), nullptr);
	}
	else if (prop == &create){
		create_(true, static_cast<create_info *>(arg));
	}*/
}

ewin::window::object::ptr_type ewin::window::object::reflect_(){
	return shared_from_this();
}

bool ewin::window::object::is_forbidden_(void *target){
	return false;
}

void ewin::window::object::create_(bool create, const create_info *info){
	/*if (create == (id_ != static_cast<common::types::atom>(0)))
		return;//Already created or destroyed

	if (create){
		if (info != nullptr){//Copy info
			info_.hInstance = info->instance;
			info_.lpfnWndProc = info->procedure;
			info_.lpszClassName = (name_ = info->name).data();
			info_.lpszMenuName = (menu_ = info->menu).data();
			info_.style = info->style;
			info_.hbrBackground = info->background_brush;
			info_.hIconSm = info->small_icon;
			info_.hIcon = info->icon;
			info_.cbWndExtra = info->wnd_extra;
			info_.cbClsExtra = info->cls_extra;
		}

		if ((id_ = ::RegisterClassExW(&info_)) == static_cast<common::types::atom>(0))
			throw ::GetLastError();//Failed to register class
	}
	else if (::UnregisterClassW(info_.lpszClassName, info_.hInstance) == FALSE)
		throw ::GetLastError();//Failed
	else//Reset ID
		id_ = static_cast<common::types::atom>(0);*/
}

void ewin::window::object::set_rect_(const common::types::rect &value, bool relative){

}

ewin::common::types::rect ewin::window::object::get_rect_(bool relative) const{
	common::types::rect value{};
	::GetWindowRect(handle_, &value);
	if (relative/* && HAS_PARENT*/){//Convert value from screen
		::ScreenToClient(nullptr/*PARENT*/, reinterpret_cast<common::types::point *>(&value.left));
		::ScreenToClient(nullptr/*PARENT*/, reinterpret_cast<common::types::point *>(&value.right));
	}

	return value;
}

void ewin::window::object::set_point_(const common::types::point &value, bool relative){
	
}

ewin::common::types::point ewin::window::object::get_point_(bool relative) const{
	auto rect_value = get_rect_(relative);
	return common::types::point{ rect_value.left, rect_value.top };
}

void ewin::window::object::set_size_(const common::types::size &value){

}

ewin::common::types::size ewin::window::object::get_size_() const{
	auto rect_value = get_rect_(false);
	return common::types::size{ rect_value.right - rect_value.left, rect_value.bottom - rect_value.top };
}

void ewin::window::object::set_relative_size_(const sizef &value){

}

ewin::window::object::sizef ewin::window::object::get_relative_size_() const{
	return sizef{};
}
