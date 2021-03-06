#include "../application/main_application.h"

#include "dialog_window.h"

ewin::window::object::object()
	: tree(*this), view(*this), frame(*this), state(*this), style(*this),
	attribute(*this), app_(nullptr), handle_(nullptr), auto_destroy_(true){
	cache_ = cache_info{};
	bind_properties_();

	drawer_.target = *this;
	drawer_.lock_target = true;
	drawer_.brush = &color_brush_;

	color_brush_.drawer = &drawer_;
}

ewin::window::object::~object(){
	destruct_();
}

ewin::message::target *ewin::window::object::parent_() const{
	return tree.parent_;
}

ewin::window::wnd_event *ewin::window::object::get_events_(){
	if (events_ == nullptr)
		events_ = std::make_shared<wnd_event>(*this);
	return events_.get();
}

bool ewin::window::object::has_events_() const{
	return (events_ != nullptr);
}

void ewin::window::object::bind_properties_(){
	auto handler = EWIN_PROP_HANDLER(object);

	reflect.initialize_(nullptr, handler);
	is_forbidden.initialize_(nullptr, handler);

	app.initialize_(nullptr, handler);
	handle.initialize_(&handle_, nullptr);
	procedure.initialize_(&procedure_, nullptr);

	size.initialize_(&cache_.size, handler);
	client_size.initialize_(&cache_.client_size, handler);

	position.initialize_(&cache_.position, handler);
	relative_position.initialize_(&cache_.relative_position, handler);

	rect.initialize_(&cache_.rect, handler);
	relative_rect.initialize_(&cache_.relative_rect, handler);
	client_rect.initialize_(&cache_.client_rect, nullptr);

	send_message.initialize_(nullptr, handler);
	post_message.initialize_(nullptr, handler);

	filter_styles.initialize_(nullptr, handler);
	filter_extended_styles.initialize_(nullptr, handler);

	filtered_styles.initialize_(nullptr, handler);
	filtered_extended_styles.initialize_(nullptr, handler);

	is_dialog_message.initialize_(nullptr, handler);
	transition.initialize_(nullptr, handler);
	events.initialize_(nullptr, handler);

	created.initialize_(nullptr, handler);
	create.initialize_(nullptr, handler);
	auto_destroy.initialize_(&auto_destroy_, nullptr);

	menu.initialize_(nullptr, handler);
	system_menu.initialize_(nullptr, handler);

	drawer.initialize_(nullptr, handler);
	color_brush.initialize_(nullptr, handler);
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
	
	if (is_forbidden_(property_forbidden_info{ prop, access })){
		set_error_(error_type::forbidden_property);
		return;
	}

	error_value_ = error_type::nil;
	if (prop == &app){
		if (access == common::property_access::read)
			*static_cast<application_type **>(arg) = app_;
		else if (access == common::property_access::write && handle_ == nullptr)
			app_ = static_cast<application_type *>(arg);
	}
	else if (prop == &is_dialog_message){
		auto info = static_cast<std::pair<common::types::msg *, bool> *>(arg);
		info->second = is_dialog_message_(*info->first);
		return;
	}
	else if (prop == &drawer){
		if (!drawer_.created && handle_ != nullptr){
			app_->task += [this]{
				drawer_.created = true;
			};
		}

		*static_cast<drawing::hwnd_object **>(arg) = &drawer_;
	}
	else if (prop == &color_brush){
		if (!color_brush_.created && handle_ != nullptr){
			app_->task += [this]{
				color_brush_.created = true;
			};
		}

		*static_cast<drawing::solid_color_brush **>(arg) = &color_brush_;
	}
	else if (prop == &menu){
		if (tree.parent_ != nullptr || attribute.is_control || attribute.is_message_only){
			set_error_(error_type::forbidden_property);
			return;
		}

		if (menu_ == nullptr){//Create
			menu_ = std::make_shared<ewin::menu::bar_collection>();
			menu_->owner = this;
		}

		*static_cast<ewin::menu::bar_collection **>(arg) = menu_.get();
	}
	else if (prop == &system_menu){
		if (handle_ == nullptr){
			set_error_(error_type::forbidden_property);
			return;
		}

		if (system_menu_ == nullptr)//Create
			system_menu_ = std::make_shared<ewin::menu::external_collection>(::GetSystemMenu(handle_, FALSE), *app_);

		*static_cast<ewin::menu::external_collection **>(arg) = system_menu_.get();
	}
	else if (prop == &size && access == common::property_access::write)
		update_dimension_(dimension_type::size);
	else if (prop == &client_size && access == common::property_access::write)
		update_dimension_(dimension_type::client_size);
	else if (prop == &position && access == common::property_access::write)
		update_dimension_(dimension_type::position);
	else if (prop == &relative_position && access == common::property_access::write)
		update_dimension_(dimension_type::relative_position);
	else if (prop == &rect && access == common::property_access::write)
		update_dimension_(dimension_type::rect);
	else if (prop == &relative_rect && access == common::property_access::write)
		update_dimension_(dimension_type::relative_rect);
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
	else if (prop == &events)
		*static_cast<wnd_event **>(arg) = get_events_();
	else if (prop == &transition)
		*static_cast<wnd_transition **>(arg) = get_transition_();
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

	if (!tree.children_.empty()){
		auto children = tree.children_;//Get copy of list
		for (auto child : children)//Remove association
			child->tree.set_parent_(nullptr, 0);
	}

	tree.set_parent_(nullptr, 0);//Remove parent, if any
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

void ewin::window::object::low_level_create_(const common::types::create_struct &info, object *parent, application_type *app, attribute_option_type options){
	common::types::uint additional_styles = 0;
	common::types::hwnd parent_handle = nullptr;

	if (app == nullptr)//Use internal app
		app = app_;

	if (parent == nullptr)
		parent = tree.parent_;

	common::types::point offset{};
	if (parent != nullptr){//Add child style and use parent's app
		if (!parent->created){
			set_error_(error_type::parent_not_created);
			return;
		}

		EWIN_SET(additional_styles, WS_CHILD);
		app = parent->app_;
		parent_handle = parent->handle;
	}
	else if (app == nullptr)//Use main app
		app = application::manager::main;

	if (app == nullptr){//App required
		set_error_(error_type::no_app);
		return;
	}

	if (EWIN_IS(options, attribute_option_type::absolute_position)){
		offset = common::types::point{ info.x, info.y };
		if (parent != nullptr)//Convert absolute value to parent relative
			parent->screen_to_client_(offset);
	}

	if (parent == nullptr && info.hwndParent != HWND_MESSAGE){
		if (info.hwndParent != nullptr){//Resolve parent
			if ((tree.parent_ = app->window_handles[info.hwndParent]) == nullptr){
				set_error_(error_type::parent_not_found);
				return;
			}
		}
		else//No parent
			tree.parent_ = nullptr;
	}
	else//Use parent
		tree.parent_ = parent;

	(app_ = app)->task += [&]{//Create window inside app thread context
		auto styles = (info.style | persistent_styles_(false) | view.cached_value_ | frame.cached_value_ | state.cached_value_ | style.cached_value_);
		auto extended_styles = (info.dwExStyle | persistent_styles_(true) | view.cached_extended_value_ | frame.cached_extended_value_ |
			state.cached_extended_value_ | style.cached_extended_value_);

		common::types::size size{};
		common::types::rect rect{ 0, 0, info.cx, info.cy };
		if (handle_ != nullptr && EWIN_IS(options, attribute_option_type::client_size)){//Adjust size
			::AdjustWindowRectEx(&rect, styles, EWIN_C_BOOL(info.hMenu != nullptr), extended_styles);
			size = common::types::size{ (rect.right - rect.left), (rect.bottom - rect.top) };
		}

		app->window_being_created = this;
		handle_ = ::CreateWindowExW(
			extended_styles,
			class_name_(),
			info.lpszName,
			(styles | additional_styles),
			(EWIN_IS(options, attribute_option_type::absolute_position) ? offset.x : info.x),
			(EWIN_IS(options, attribute_option_type::absolute_position) ? offset.y : info.y),
			(EWIN_IS(options, attribute_option_type::client_size) ? size.cx : info.cx),
			(EWIN_IS(options, attribute_option_type::client_size) ? size.cy : info.cy),
			((parent_handle == nullptr) ? info.hwndParent : parent_handle),
			info.hMenu,
			(info.hInstance == nullptr) ? ::GetModuleHandleW(nullptr) : info.hInstance,
			this
		);

		if (handle_ != nullptr){//Reset cached styles
			view.cached_value_ = frame.cached_value_ = state.cached_value_ = style.cached_value_ = 0;
			view.cached_extended_value_ = frame.cached_extended_value_ = state.cached_extended_value_ = style.cached_extended_value_ = 0;
		}
		else//Failed to create window
			set_error_(::GetLastError());
	};
}

const wchar_t *ewin::window::object::class_name_(){
	return (attribute.is_dialog ? application::manager::main->dialog_window_class.raw_name : application::manager::main->general_window_class.raw_name);
}

void ewin::window::object::update_dimension_(dimension_type type){
	switch (type){
	case dimension_type::rect:
		if (handle_ != nullptr){
			cache_.position.x = cache_.relative_rect.left = cache_.rect.left;
			cache_.position.y = cache_.relative_rect.top = cache_.rect.top;

			cache_.size.cx = (cache_.rect.right - cache_.rect.left);
			cache_.size.cy = (cache_.rect.bottom - cache_.rect.top);

			if (tree.parent_ != nullptr)//Convert from screen
				tree.parent_->screen_to_client_(cache_.relative_rect);
			else//No conversion necessary
				cache_.relative_rect = cache_.rect;

			::SetWindowPos(handle_, nullptr, cache_.relative_rect.left, cache_.relative_rect.top, cache_.size.cx, cache_.size.cy, (SWP_NOZORDER | SWP_NOACTIVATE));
			::GetClientRect(handle_, &cache_.client_rect);

			cache_.relative_position.x = cache_.relative_rect.left;
			cache_.relative_position.y = cache_.relative_rect.top;

			cache_.client_size.cx = (cache_.client_rect.right - cache_.client_rect.left);
			cache_.client_size.cy = (cache_.client_rect.bottom - cache_.client_rect.top);
		}
		else{//Invalidate others
			cache_.relative_rect = cache_.client_rect = common::types::rect{};
			cache_.position.x = cache_.rect.left;
			cache_.position.y = cache_.rect.top;
		}
		break;
	case dimension_type::relative_rect:
		if (handle_ != nullptr){
			cache_.relative_position.x = cache_.rect.left = cache_.relative_rect.left;
			cache_.relative_position.y = cache_.rect.top = cache_.relative_rect.top;

			cache_.size.cx = (cache_.relative_rect.right - cache_.relative_rect.left);
			cache_.size.cy = (cache_.relative_rect.bottom - cache_.relative_rect.top);

			if (tree.parent_ != nullptr)//Convert from screen
				tree.parent_->client_to_screen_(cache_.rect);
			else//No conversion necessary
				cache_.rect = cache_.relative_rect;

			cache_.position.x = cache_.rect.left;
			cache_.position.y = cache_.rect.top;

			::SetWindowPos(handle_, nullptr, cache_.relative_rect.left, cache_.relative_rect.top, cache_.size.cx, cache_.size.cy, (SWP_NOZORDER | SWP_NOACTIVATE));
			::GetClientRect(handle_, &cache_.client_rect);

			cache_.client_size.cx = (cache_.client_rect.right - cache_.client_rect.left);
			cache_.client_size.cy = (cache_.client_rect.bottom - cache_.client_rect.top);
		}
		else{//Invalidate others
			cache_.rect = cache_.client_rect = common::types::rect{};
			cache_.position = common::types::point{};
			cache_.relative_position.x = cache_.relative_rect.left;
			cache_.relative_position.y = cache_.relative_rect.top;
		}
		break;
	case dimension_type::position:
		cache_.rect.right = ((cache_.rect.left = cache_.position.x) + cache_.size.cx);
		cache_.rect.bottom = ((cache_.rect.top = cache_.position.y) + cache_.size.cy);
		update_dimension_(dimension_type::rect);
		break;
	case dimension_type::relative_position:
		cache_.relative_rect.right = ((cache_.relative_rect.left = cache_.relative_position.x) + cache_.size.cx);
		cache_.relative_rect.bottom = ((cache_.relative_rect.top = cache_.relative_position.y) + cache_.size.cy);
		update_dimension_(dimension_type::relative_rect);
		break;
	case dimension_type::size:
		cache_.rect.right = (cache_.rect.left + cache_.size.cx);
		cache_.rect.bottom = (cache_.rect.top + cache_.size.cy);
		update_dimension_(dimension_type::rect);
		break;
	case dimension_type::client_size:
	{
		if (handle_ != nullptr){
			common::types::rect client_rect = cache_.client_rect;
			client_to_screen_(client_rect);//Convert to screen

			cache_.rect.right = (cache_.rect.left + cache_.client_size.cx + (client_rect.left - cache_.rect.left) + (cache_.rect.right - client_rect.right));
			cache_.rect.bottom = (cache_.rect.top + cache_.client_size.cy + (client_rect.top - cache_.rect.top) + (cache_.rect.bottom - client_rect.bottom));

			update_dimension_(dimension_type::rect);
		}
		else//Invalidate rects
			cache_.rect = cache_.relative_rect = cache_.client_rect = common::types::rect{};
		break;
	}
	default:
		break;
	}
}

void ewin::window::object::cache_dimensions_(){
	if (handle_ == nullptr)
		return;//Ignore

	::GetWindowRect(handle_, &cache_.rect);
	::GetClientRect(handle_, &cache_.client_rect);

	cache_.position.x = cache_.relative_rect.left = cache_.rect.left;
	cache_.position.y = cache_.relative_rect.top = cache_.rect.top;

	cache_.size.cx = (cache_.rect.right - cache_.rect.left);
	cache_.size.cy = (cache_.rect.bottom - cache_.rect.top);

	if (tree.parent_ != nullptr)//Convert from screen
		tree.parent_->screen_to_client_(cache_.relative_rect);
	else//No conversion necessary
		cache_.relative_rect = cache_.rect;

	cache_.relative_position.x = cache_.relative_rect.left;
	cache_.relative_position.y = cache_.relative_rect.top;

	cache_.client_size.cx = (cache_.client_rect.right - cache_.client_rect.left);
	cache_.client_size.cy = (cache_.client_rect.bottom - cache_.client_rect.top);
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

void ewin::window::object::screen_to_client_(common::types::point &point) const{
	if (handle_ != nullptr)
		::ScreenToClient(handle_, &point);
}

void ewin::window::object::screen_to_client_(common::types::rect &rect) const{
	if (handle_ != nullptr)
		::MapWindowPoints(HWND_DESKTOP, handle_, reinterpret_cast<common::types::point *>(&rect), 2);
}

void ewin::window::object::client_to_screen_(common::types::point &point) const{
	if (handle_ != nullptr)
		::ClientToScreen(handle_, &point);
}

void ewin::window::object::client_to_screen_(common::types::rect &rect) const{
	if (handle_ != nullptr)
		::MapWindowPoints(handle_, HWND_DESKTOP, reinterpret_cast<common::types::point *>(&rect), 2);
}

bool ewin::window::object::validate_parent_change_(object *value){
	if (attribute.is_message_only){//Cannot change parent
		set_error_(common::error_type::parent_change_forbidden);
		return false;
	}

	if (value != nullptr && created && !value->created){//Parent window has not been created
		set_error_(common::error_type::parent_not_created);
		return false;
	}

	return true;
}

bool ewin::window::object::validate_child_remove_(object &value){
	return true;
}

bool ewin::window::object::validate_child_add_(object &value, std::size_t index){
	return true;
}

void ewin::window::object::child_removed_(object &value, std::size_t index){}

void ewin::window::object::child_added_(object &value, std::size_t index){}

void ewin::window::object::parent_changed_(object *current, object *previous, std::size_t index, std::size_t previous_index){
	if (handle_ != nullptr && current != previous){//Update
		::SetParent(handle_, ((current == nullptr) ? nullptr : current->handle_));
		if (previous == nullptr && current != nullptr)
			style.value += WS_CHILD;//Set child style
		else if (previous != nullptr && current == nullptr)
			style.value -= WS_CHILD;//Remove child style
	}
}

void ewin::window::object::event_listener_count_changed_(events::basic &e, std::size_t count){}

ewin::window::wnd_transition *ewin::window::object::get_transition_(){
	if (transition_ == nullptr)
		transition_ = std::make_shared<wnd_transition>(*this);
	return transition_.get();
}
