#include "window_object.h"

ewin::window::object::object()
	: tree(*this), view(*this), frame(*this), state(*this), style(*this), attribute(*this), app_(nullptr), handle_(nullptr), procedure_(::DefWindowProcW),
	error_throw_policy_(error_throw_policy_type::always), error_value_(error_type::nil), local_error_value_(ERROR_SUCCESS), auto_destroy_(true){
	bind_properties_();
}

ewin::window::object::~object(){
	destruct_();
}

void ewin::window::object::bind_properties_(){
	auto handler = EWIN_PROP_HANDLER(object);

	reflect.initialize_(nullptr, handler);
	is_forbidden.initialize_(nullptr, handler);

	error_throw_policy.initialize_(nullptr, handler);
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

	send_message.initialize_(nullptr, handler);
	post_message.initialize_(nullptr, handler);

	filter_styles.initialize_(nullptr, handler);
	filter_extended_styles.initialize_(nullptr, handler);

	filtered_styles.initialize_(nullptr, handler);
	filtered_extended_styles.initialize_(nullptr, handler);

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
		auto info = static_cast<std::pair<property_forbidden_info *, bool> *>(arg);
		info->second = is_forbidden_(*info->first);
		return;
	}
	
	if (prop == &error){
		if (access == common::property_access::read){
			auto &info = *static_cast<common::variant_value_property_arg_info *>(arg);
			switch (info.index){
			case 0u:
				*static_cast<error_type *>(info.value) = error_value_;
				break;
			case 1u:
				*static_cast<common::types::dword *>(info.value) = local_error_value_;
				break;
			default:
				break;
			}
		}
		else if (access == common::property_access::write)
			set_error_(*static_cast<common::variant_value_property_arg_info *>(arg));

		return;
	}

	if (is_forbidden_(property_forbidden_info{ prop, access })){
		set_error_(error_type::forbidden_property);
		return;
	}

	error_value_ = error_type::nil;
	if (prop == &error_throw_policy){
		if (access == common::property_access::read)
			*static_cast<error_throw_policy_type *>(arg) = error_throw_policy_;
		else if (access == common::property_access::write)
			error_throw_policy_ = *static_cast<error_throw_policy_type *>(arg);
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
	else if (prop == &client_rect)
		*static_cast<rect_type *>(arg) = get_client_rect_();
	else if (prop == &send_message)
		send_message_(*static_cast<std::pair<message_info *, common::types::result> *>(arg));
	else if (prop == &post_message)
		post_message_(*static_cast<message_info *>(arg));
	else if (prop == &filter_styles)
		filter_styles_(*static_cast<std::pair<common::types::uint *, common::types::uint> *>(arg), false);
	else if (prop == &filter_extended_styles)
		filter_styles_(*static_cast<std::pair<common::types::uint *, common::types::uint> *>(arg), true);
	else if (prop == &filtered_styles)
		*static_cast<common::types::uint *>(arg) = filtered_styles_(false);
	else if (prop == &filtered_extended_styles)
		*static_cast<common::types::uint *>(arg) = filtered_styles_(true);
	else if (prop == &created){
		if (access == common::property_access::read)
			*static_cast<bool *>(arg) = (handle_ != nullptr);
		else if (access == common::property_access::write)
			create_(*static_cast<bool *>(arg), nullptr);
	}
	else if (prop == &create)
		create_(true, static_cast<create_info *>(arg));
}

ewin::window::object::ptr_type ewin::window::object::reflect_(){
	return shared_from_this();
}

bool ewin::window::object::is_forbidden_(const property_forbidden_info &info){
	if (info.value == &app_ && info.access == common::property_access::write)
		return (handle_ != nullptr);//Prevent app change after creation
	return false;
}

void ewin::window::object::destruct_(){
	if (auto_destroy_){
		try{
			create_(false, nullptr);
		}
		catch (...){}
	}
}

void ewin::window::object::create_(bool create, const create_info *info){
	if (!create && handle_ != nullptr){//Destroy window
		app_->task += [this]{
			if (::DestroyWindow(handle_) != FALSE){
				handle_ = nullptr;
			}
			else//Failed to destroy
				set_error_(::GetLastError());
		};
	}
}

void ewin::window::object::low_level_create_(const common::types::create_struct &info){
	common::types::uint additional_styles = 0;
	common::types::hwnd parent_handle = nullptr;

	auto app = app_;
	if (tree.parent_ != nullptr){//Add child style and use parent's app
		if (!tree.parent_->created){
			set_error_(error_type::parent_not_created);
			return;
		}

		EWIN_SET(additional_styles, WS_CHILD);
		app = tree.parent_->app_;
		parent_handle = tree.parent_->handle_;
	}
	else if (app == nullptr)//Use main app
		app = application::manager::main;

	if (app == nullptr){//App required
		set_error_(error_type::no_app);
		return;
	}

	(app_ = app)->task += [&]{//Create window inside app thread context
		try{
			app->window_being_created = this;
			handle_ = ::CreateWindowExW(
				(info.dwExStyle | persistent_styles_(true)),
				((dynamic_cast<object *>(this) != nullptr) ? application::manager::general_window_class.raw_name : application::manager::dialog_window_class.raw_name),
				info.lpszName,
				(info.style | persistent_styles_(false)),
				info.x,
				info.y,
				info.cx,
				info.cy,
				((parent_handle == nullptr) ? info.hwndParent : parent_handle),
				info.hMenu,
				(info.hInstance == nullptr) ? ::GetModuleHandleW(nullptr) : info.hInstance,
				this
			);

			if (handle_ == nullptr)//Failed to create window
				set_error_(::GetLastError());
		}
		catch (...){
			app->window_being_created = nullptr;
			throw;//Forward exception
		}

		app->window_being_created = nullptr;
	};
}

void ewin::window::object::set_error_(common::variant_value_property_arg_info &info){
	switch (info.index){
	case 0u:
		set_error_(*static_cast<error_type *>(info.value));
		break;
	case 1u:
		set_error_(*static_cast<common::types::dword *>(info.value));
		break;
	default:
		break;
	}
}

void ewin::window::object::set_error_(error_type value){
	if (value == error_type::nil){//Clear error
		error_value_ = value;
		return;
	}

	switch (static_cast<error_throw_policy_type>(error_throw_policy_)){
	case error_throw_policy_type::never://Update last error
		error_value_ = value;
		break;
	case error_throw_policy_type::once://Throw once
		error_throw_policy_ = error_throw_policy_type::never;
		throw value;
		break;
	default:
		throw value;
		break;
	}
}

void ewin::window::object::set_error_(common::types::dword value){
	if ((local_error_value_ = value) == ERROR_SUCCESS)
		error_value_ = error_type::nil;
	else//Signify local error
		set_error_(error_type::local_error);
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

void ewin::window::object::filter_styles_(std::pair<common::types::uint *, common::types::uint> &info, bool is_extended) const{
	info.second = filter_styles_(*info.first, is_extended);
}

ewin::common::types::uint ewin::window::object::filter_styles_(common::types::uint value, bool is_extended) const{
	return EWIN_REMOVE_V(value, filtered_styles_(is_extended));
}

ewin::common::types::uint ewin::window::object::filtered_styles_(bool is_extended) const{
	return EWIN_REMOVE_V(black_listed_styles_(is_extended), white_listed_styles_(is_extended));
}

ewin::common::types::uint ewin::window::object::white_listed_styles_(bool is_extended) const{
	return (is_extended ? 0u : (WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_TABSTOP | WS_DISABLED | WS_VISIBLE));
}

ewin::common::types::uint ewin::window::object::black_listed_styles_(bool is_extended) const{
	return (is_extended ? WS_EX_LEFTSCROLLBAR : (WS_HSCROLL | WS_VSCROLL));
}

ewin::common::types::uint ewin::window::object::persistent_styles_(bool is_extended) const{
	return 0u;
}

void ewin::window::object::send_message_(std::pair<message_info *, common::types::result> &info){
	if (handle_ == nullptr)
		error = error_type::window_not_created;
	else//Send
		info.second = ::SendMessageW(handle_, info.first->value, info.first->wparam, info.first->lparam);
}

void ewin::window::object::post_message_(message_info &info){
	if (handle_ == nullptr)
		error = error_type::window_not_created;
	else//Post
		::PostMessageW(handle_, info.value, info.wparam, info.lparam);
}

bool ewin::window::object::is_dialog_message_(const common::types::msg &msg) const{
	return EWIN_CPP_BOOL(::IsDialogMessageW(handle_, &const_cast<common::types::msg &>(msg)));
}

ewin::common::types::result ewin::window::object::dispatch_message_(const common::types::msg &msg){
	return 0u;
}
