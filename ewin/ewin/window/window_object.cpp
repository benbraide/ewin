#include "window_object.h"

ewin::window::object::object()
	: tree(*this), view(*this), frame(*this), error_throw_policy_(error_throw_policy_type::always), error_value_(error_type::nil), auto_destroy_(true){
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
	client_size.initialize_(nullptr, handler);

	position.initialize_(nullptr, handler);
	relative_position.initialize_(nullptr, handler);

	rect.initialize_(nullptr, handler);
	relative_rect.initialize_(nullptr, handler);
	client_rect.initialize_(nullptr, handler);

	/*tree.initialize_(&tree_, nullptr);
	view.initialize_(&view_, nullptr);
	frame.initialize_(&frame_, nullptr);
	style.initialize_(&frame_, nullptr);
	state.initialize_(&frame_, nullptr);*/

	created.initialize_(nullptr, handler);
	create.initialize_(nullptr, handler);
	auto_destroy.initialize_(&auto_destroy_, nullptr);
}

void ewin::window::object::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &reflect){
		*static_cast<ptr_type *>(arg) = reflect_();
		return;
	}

	if (prop == &is_forbidden){
		*static_cast<bool *>(arg) = is_forbidden_(*reinterpret_cast<property_forbidden_info *>(arg));
		return;
	}

	if (is_forbidden_(property_forbidden_info{ prop, access })){
		set_error_(error_type::forbidden_property);
		return;
	}

	if (prop == &error){
		if (access == common::property_access::write)
			set_error_(*static_cast<error_type *>(arg));
		else if (access == common::property_access::read)
			*static_cast<error_type *>(arg) = error_value_;
	}
	else if (prop == &app){
		if (access == common::property_access::read)
			*static_cast<application_type **>(arg) = app_;
		else if (access == common::property_access::write && handle_ == nullptr)
			app_ = *static_cast<application_type **>(arg);
	}
	else if (prop == &size){
		if (access == common::property_access::read)
			*static_cast<size_type *>(arg) = get_size_(false);
		else if (access == common::property_access::write)
			set_size_(*static_cast<size_type *>(arg), false);
	}
	else if (prop == &client_size){
		if (access == common::property_access::read)
			*static_cast<size_type *>(arg) = get_size_(true);
		else if (access == common::property_access::write)
			set_size_(*static_cast<size_type *>(arg), true);
	}
	else if (prop == &position){
		if (access == common::property_access::read)
			*static_cast<point_type *>(arg) = get_position_(false);
		else if (access == common::property_access::write)
			set_position_(*static_cast<point_type *>(arg), false);
	}
	else if (prop == &relative_position){
		if (access == common::property_access::read)
			*static_cast<point_type *>(arg) = get_position_(true);
		else if (access == common::property_access::write)
			set_position_(*static_cast<point_type *>(arg), true);
	}
	else if (prop == &rect){
		if (access == common::property_access::read)
			*static_cast<rect_type *>(arg) = get_rect_(false);
		else if (access == common::property_access::write)
			set_rect_(*static_cast<rect_type *>(arg), false);
	}
	else if (prop == &relative_rect){
		if (access == common::property_access::read)
			*static_cast<rect_type *>(arg) = get_rect_(true);
		else if (access == common::property_access::write)
			set_rect_(*static_cast<rect_type *>(arg), true);
	}
	else if (prop == &client_rect){
		*static_cast<rect_type *>(arg) = get_client_rect_();
	}
}

ewin::window::object::ptr_type ewin::window::object::reflect_(){
	return shared_from_this();
}

bool ewin::window::object::is_forbidden_(const property_forbidden_info &info){
	if (info.value == &app_ && info.access == common::property_access::write)
		return (handle_ != nullptr);//Prevent app change after creation
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

void ewin::window::object::set_error_(error_type value){
	switch (static_cast<error_throw_policy_type>(error_throw_policy_)){
	case error_throw_policy_type::never://Update last error
		error_value_ = value;
		break;
	case error_throw_policy_type::once://Throw once
		error_throw_policy_ = error_throw_policy_type::never;
		if (value != error_type::nil)
			throw value;
		break;
	default:
		if (value != error_type::nil)
			throw value;
		break;
	}
}

void ewin::window::object::set_rect_(const rect_type &value, bool relative){
	if (!relative && tree.parent != nullptr){
		common::types::size size{ (value.right - value.left), (value.bottom - value.top) };
		common::types::point relative_pos{ value.left, value.top };

		::ScreenToClient(tree.parent->handle_, &relative_pos);
		::SetWindowPos(handle_, nullptr, relative_pos.x, relative_pos.y, (relative_pos.x + size.cx), (relative_pos.y + size.cy), SWP_NOZORDER | SWP_NOACTIVATE);
	}
	else//No conversion
		::SetWindowPos(handle_, nullptr, value.left, value.top, (value.right - value.left), (value.bottom - value.top), SWP_NOZORDER | SWP_NOACTIVATE);
}

ewin::window::object::rect_type ewin::window::object::get_rect_(bool relative) const{
	common::types::rect value{};
	::GetWindowRect(handle_, &value);
	if (relative && tree.parent != nullptr){//Convert value from screen
		common::types::size size{ (value.right - value.left), (value.bottom - value.top) };
		common::types::point relative_pos{ value.left, value.top };

		::ScreenToClient(tree.parent->handle_, &relative_pos);
		value = common::types::rect{ relative_pos.x, relative_pos.y, (relative_pos.x + size.cx), (relative_pos.y + size.cy) };
	}

	return rect_type{ value.left, value.top, value.right, value.bottom };
}

ewin::window::object::rect_type ewin::window::object::get_client_rect_() const{
	common::types::rect value{};
	::GetClientRect(handle_, &value);
	return rect_type{ value.left, value.top, value.right, value.bottom };
}

void ewin::window::object::set_position_(const point_type &value, bool relative){
	auto size = get_size_(false);
	set_rect_(rect_type{ value.x, value.y, (value.x + size.width), (value.y + size.height) }, relative);
}

ewin::window::object::point_type ewin::window::object::get_position_(bool relative) const{
	auto rect_value = get_rect_(relative);
	return point_type{ rect_value.left, rect_value.top };
}

void ewin::window::object::set_size_(const size_type &value, bool client){
	auto position = get_position_(true);
	if (client){//Convert
		auto rect = get_rect_(false), relative_rect = get_rect_(true);
		auto padding = rect_type{
			(relative_rect.left - rect.left),
			(relative_rect.top - rect.top),
			(rect.right - relative_rect.right),
			(rect.bottom - relative_rect.bottom)
		};

		set_rect_(
			rect_type{ position.x, position.y, (position.x + value.width + padding.left + padding.right), (position.y + value.height + padding.top + padding.bottom) },
			true
		);
	}
	else//No conversion
		set_rect_(rect_type{ position.x, position.y, (position.x + value.width), (position.y + value.height) }, true);
}

ewin::window::object::size_type ewin::window::object::get_size_(bool client) const{
	if (client){//Client size
		auto rect_value = get_client_rect_();
		return size_type{ rect_value.right - rect_value.left, rect_value.bottom - rect_value.top };
	}

	auto rect_value = get_rect_(false);
	return size_type{ rect_value.right - rect_value.left, rect_value.bottom - rect_value.top };
}
