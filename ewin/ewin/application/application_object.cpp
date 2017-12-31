#include "../window/message_window.h"

ewin::application::object::object()
	: object(false){}

ewin::application::object::object(bool is_main)
	: window_being_created_(nullptr){
	cached_window_handle_ = std::make_pair<common::types::hwnd, window_type *>(nullptr, nullptr);
	cached_menu_handle_ = std::make_pair<common::types::hmenu, menu::container *>(nullptr, nullptr);

	if (!is_main && manager::main_ == nullptr)
		throw error_type::no_main_app;

	if (is_main && manager::main_ != nullptr)
		throw error_type::multiple_main_app;

	if (manager::current_ != nullptr)
		throw error_type::multiple_app;

	manager::current_ = this;
	manager::application_list_.push_back(*this);

	object_state_.focused = object_state_.moused = HWND_DESKTOP;
	object_state_.last_mouse_position.x = object_state_.last_mouse_position.y = -1;
	object_state_.mouse_down_position.x = object_state_.mouse_down_position.y = 0;
	object_state_.mouse_button_down = 0;
	object_state_.dragging = false;

	hook_id_ = ::SetWindowsHookExW(WH_CBT, hook_, nullptr, ::GetCurrentThreadId());
	thread_id_ = std::this_thread::get_id();

	bind_properties_();
	message_window_ = std::make_shared<window::message>();

	if (!is_main)//Create message window
		message_window_->created = true;
}

ewin::application::object::~object(){
	if (hook_id_ != nullptr){//Remove hook
		::UnhookWindowsHookEx(hook_id_);
		hook_id_ = nullptr;
	}

	if (!manager::application_list_.empty()){
		for (auto iter = manager::application_list_.begin(); iter != manager::application_list_.end(); ++iter){
			if (&iter->get() == this){
				manager::application_list_.erase(iter);
				break;
			}
		}
	}
}

void ewin::application::object::bind_properties_(){
	auto handler = EWIN_PROP_HANDLER(object);

	task.initialize_(nullptr, handler);
	async_task.initialize_(nullptr, handler);

	menu_handles.initialize_(nullptr, handler);
	window_handles.initialize_(nullptr, handler);
	top_level_handles.initialize_(nullptr, handler);
	window_being_created.initialize_(nullptr, handler);
	run.initialize_(nullptr, handler);

	drawing_factory.initialize_(nullptr, handler);
	hdc_drawer.initialize_(nullptr, handler);
	color_brush.initialize_(nullptr, handler);

	last_mouse_position.initialize_(&object_state_.last_mouse_position, nullptr);
	update_last_mouse_position.initialize_(nullptr, handler);

	hdc_drawer_.brush = &color_brush_;
	color_brush_.drawer = &hdc_drawer_;
}

void ewin::application::object::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &menu_handles){
		task_([&]{//Execute in thread context
			if (access == common::property_access::list_at){
				auto &info = *static_cast<std::pair<common::types::hmenu, menu::container *> *>(arg);
				info.second = find_menu_(info.first);
			}
			else if (access == common::property_access::list_find){
				auto &info = *static_cast<std::pair<common::types::hmenu, menu::container *> *>(arg);
				info.first = info.second->handle;
			}
			else if (access == common::property_access::list_size)
				*static_cast<std::size_t *>(arg) = window_handles_.size();
		});
	}
	else if (prop == &window_handles){
		task_([&]{//Execute in thread context
			if (access == common::property_access::list_at){
				auto &info = *static_cast<std::pair<common::types::hwnd, window_type *> *>(arg);
				info.second = find_(info.first);
			}
			else if (access == common::property_access::list_find){
				auto &info = *static_cast<std::pair<common::types::hwnd, window_type *> *>(arg);
				info.first = info.second->handle;
			}
			else if (access == common::property_access::list_size)
				*static_cast<std::size_t *>(arg) = window_handles_.size();
		});
	}
	else if (prop == &top_level_handles){
		task_([&]{//Execute in thread context
			if (access == common::property_access::list_begin)
				*static_cast<window_list_iterator_type *>(arg) = top_level_handles_.begin();
			else if (access == common::property_access::list_end)
				*static_cast<window_list_iterator_type *>(arg) = top_level_handles_.end();
		});
	}
	else if (prop == &window_being_created){
		if (std::this_thread::get_id() == thread_id_)
			window_being_created_ = *static_cast<window_type **>(arg);
		else//Can only be accessed from same thread
			throw common::error_type::cross_thread;
	}
	else if (prop == &run){
		if (std::this_thread::get_id() == thread_id_)
			*static_cast<int *>(arg) = run_();
		else//Can only be accessed from same thread
			throw common::error_type::cross_thread;
	}
	else if (prop == &drawing_factory){
		*static_cast<drawing::factory **>(arg) = &drawing_factory_;
		if (!drawing_factory_.created){//Create
			drawing_factory_.app_ = this;
			drawing_factory_.created = true;
		}
	}
	else if (prop == &hdc_drawer){
		*static_cast<drawing::hdc_object **>(arg) = &hdc_drawer_;
		if (!hdc_drawer_.created){//Create
			task_([this]{
				hdc_drawer_.factory = drawing_factory;
				hdc_drawer_.created = true;
			});
		}
	}
	else if (prop == &color_brush){
		if (!color_brush_.created){
			task_([this]{
				color_brush_.created = true;
			});
		}

		*static_cast<drawing::solid_color_brush **>(arg) = &color_brush_;
	}
	else if (prop == &update_last_mouse_position){
		auto position = ::GetMessagePos();
		object_state_.last_mouse_position = common::types::point{ GET_X_LPARAM(position), GET_Y_LPARAM(position) };
	}
	else if (prop == &task)
		task_(*static_cast<std::pair<void *, task_type *> *>(arg)->second);
	else if (prop == &async_task)
		async_task_(*static_cast<std::pair<void *, task_type *> *>(arg)->second);
}

void ewin::application::object::task_(const task_type &callback){
	if (std::this_thread::get_id() == thread_id_)
		callback();//Same thread
	else//Execute in thread
		message_window_->send_message[window_type::message_info{ EWIN_WM_TASK, EWIN_OBJECT_WPARAM_CAST(callback), FALSE }];
}

void ewin::application::object::async_task_(const task_type &callback){
	message_window_->post_message[window_type::message_info{ EWIN_WM_TASK, EWIN_SCALAR_WPARAM_CAST(new task_type(callback)), TRUE }];
}

void ewin::application::object::execute_task_(task_type *callback, bool is_async){
	(*callback)();
	if (is_async)//Free memory
		delete callback;
}

ewin::application::object::window_type *ewin::application::object::find_(common::types::hwnd handle){
	if (handle == cached_window_handle_.first)//Use cached value
		return cached_window_handle_.second;

	if (handle == nullptr || window_handles_.empty())
		return nullptr;

	auto entry = window_handles_.find(handle);
	auto value = ((entry == window_handles_.end()) ? nullptr : entry->second);

	if (value != nullptr)//Update cached value
		cached_window_handle_ = std::make_pair(handle, value);

	return value;
}

ewin::menu::container *ewin::application::object::find_menu_(common::types::hmenu handle){
	if (handle == cached_menu_handle_.first)//Use cached value
		return cached_menu_handle_.second;

	if (handle == nullptr || menu_handles_.empty())
		return nullptr;

	auto entry = menu_handles_.find(handle);
	auto value = ((entry == menu_handles_.end()) ? nullptr : entry->second);

	if (value != nullptr)//Update cached value
		cached_menu_handle_ = std::make_pair(handle, value);

	return value;
}

int ewin::application::object::run_(){
	common::types::msg msg;
	while (!top_level_handles_.empty()){
		if (!EWIN_CPP_BOOL(::GetMessageW(&msg, nullptr, 0u, 0u)))
			throw common::error_type::failed_to_retrieve_message;

		if (msg.message == WM_QUIT)//Quit message posted
			return static_cast<int>(msg.wParam);

		if (is_filtered_message_(msg))
			continue;//Ignore filtered message

		if (msg.hwnd == nullptr)
			dispatch_thread_message_(msg);
		else//Message to target
			dispatch_message_(msg);
	}

	return 0;
}

bool ewin::application::object::is_filtered_message_(const common::types::msg &msg) const{
	return false;
}

bool ewin::application::object::is_dialog_message_(const common::types::msg &msg){
	auto focused = find_(object_state_.focused);
	return ((focused == nullptr) ? false : focused->is_dialog_message[msg]);
}

void ewin::application::object::dispatch_message_(const common::types::msg &msg){
	if (!is_dialog_message_(msg)){//Not dialog message --> dispatch
		translate_message_(msg);
		::DispatchMessageW(&msg);
	}
}

void ewin::application::object::dispatch_thread_message_(const common::types::msg &msg){
	translate_message_(msg);
	::DispatchMessageW(&msg);
}

void ewin::application::object::translate_message_(const common::types::msg &msg){
	::TranslateMessage(&msg);
}

void ewin::application::object::create_window_(common::types::hwnd handle, common::types::hook_create_window_info &info){
	if (window_being_created_ == nullptr || info.lpcs->lpCreateParams != window_being_created_)
		return;//Different window

	if (window_being_created_->attribute.is_control)//Replace procedure
		::SetWindowLongPtrW(handle, GWLP_WNDPROC, reinterpret_cast<common::types::ptr>(entry_));
	else if (!window_being_created_->attribute.is_message_only && window_being_created_->tree.parent == nullptr)
		top_level_handles_.push_back(handle);//Add to top-level list

	cached_window_handle_ = std::make_pair(handle, window_being_created_);
	window_handles_[handle] = window_being_created_;
	window_being_created_ = nullptr;
}

void ewin::application::object::destroy_window_(common::types::hwnd handle){
	if (!top_level_handles_.empty()){//Remove from top-level list, if applicable
		auto entry = std::find(top_level_handles_.begin(), top_level_handles_.end(), handle);
		if (entry != top_level_handles_.end())
			top_level_handles_.erase(entry);
	}

	if (!window_handles_.empty())//Remove from map
		window_handles_.erase(handle);

	if (object_state_.focused == handle)//Reset focused
		object_state_.focused = nullptr;

	if (object_state_.moused = handle){//Reset moused
		object_state_.moused = nullptr;
		object_state_.mouse_button_down = 0;
		object_state_.dragging = false;
	}

	if (cached_window_handle_.first == handle)
		cached_window_handle_ = {};//Reset cache
}

void ewin::application::object::focus_window_(common::types::hwnd handle){
	object_state_.focused = handle;
}

void ewin::application::object::size_window_(window_type &window_object){
	//#TODO: Update drawing objects; Alert size watchers
}

void ewin::application::object::move_window_(window_type &window_object){
	//#TODO: Alert position watchers
}

void ewin::application::object::mouse_leave_(common::types::hwnd hwnd, common::types::uint msg){
	auto position = ::GetMessagePos();
	switch (::SendMessageW(hwnd, WM_NCHITTEST, 0, position)){
	case HTCLIENT://Inside client --> Possibly inside child
		if (msg == WM_NCMOUSELEAVE)//Moved from non-client to client
			track_mouse_(hwnd, 0);
		return;
	case HTNOWHERE://Outside window
		break;
	default://Moved from client to non-client
		track_mouse_(hwnd, TME_NONCLIENT);
		return;
	}

	if (object_state_.dragging){//End drag
		::SendMessageW(hwnd, EWIN_WM_MOUSEDRAGEND, 0, 0);
		object_state_.mouse_button_down = 0;
		object_state_.dragging = false;
	}

	::SendMessageW(hwnd, EWIN_WM_MOUSELEAVE, 0, 0);//Alert
	for (hwnd = ::GetParent(hwnd); hwnd != HWND_DESKTOP && ::SendMessageW(hwnd, WM_NCHITTEST, 0, position) == HTNOWHERE; hwnd = ::GetParent(hwnd))
		::SendMessageW(hwnd, EWIN_WM_MOUSELEAVE, 0, 0);//Alert

	object_state_.moused = hwnd;//Update
}

void ewin::application::object::mouse_move_(common::types::hwnd hwnd){
	if (hwnd == object_state_.moused){//Check for drag or continue drag
		if (!object_state_.dragging && object_state_.mouse_button_down != 0u){
			auto &mouse_down_position = object_state_.mouse_down_position;
			common::types::point position{ GET_X_LPARAM(::GetMessagePos()), GET_Y_LPARAM(::GetMessagePos()) };

			common::types::size delta{//Absolute values
				((position.x < mouse_down_position.x) ? (mouse_down_position.x - position.x) : (position.x - mouse_down_position.x)),
				((position.y < mouse_down_position.y) ? (mouse_down_position.y - position.y) : (position.y - mouse_down_position.y))
			};

			if ((delta.cx >= ::GetSystemMetrics(SM_CXDRAG) || delta.cy >= ::GetSystemMetrics(SM_CYDRAG))){
				object_state_.dragging = true;//Drag begin

				auto old_mouse_position = object_state_.last_mouse_position;
				object_state_.last_mouse_position = object_state_.mouse_down_position;
				::SendMessageW(hwnd, EWIN_WM_MOUSEDRAG, object_state_.mouse_button_down, 0);
				object_state_.last_mouse_position = old_mouse_position;
			}
		}
		else if (object_state_.dragging)//Continue drag
			::SendMessageW(hwnd, EWIN_WM_MOUSEDRAG, object_state_.mouse_button_down, 0);
	}
	else{//New target
		switch (::SendMessageW(hwnd, WM_NCHITTEST, 0, ::GetMessagePos())){
		case HTCLIENT://Inside client area
			track_mouse_(hwnd, 0);
			break;
		default://Inside non-client area
			track_mouse_(hwnd, TME_NONCLIENT);
			break;
		}

		::SendMessageW(hwnd, EWIN_WM_MOUSEENTER, 0, 0);//Alert
	}
}

void ewin::application::object::mouse_down_(common::types::hwnd hwnd, common::types::uint button){
	if (object_state_.mouse_button_down == 0u)//First button down
		object_state_.mouse_down_position = common::types::point{ GET_X_LPARAM(::GetMessagePos()), GET_Y_LPARAM(::GetMessagePos()) };
	object_state_.mouse_button_down = button;
}

void ewin::application::object::mouse_up_(common::types::hwnd hwnd, common::types::uint button){
	EWIN_REMOVE(object_state_.mouse_button_down, button);
	if (object_state_.mouse_button_down == 0u && object_state_.dragging){//End drag
		::SendMessageW(hwnd, EWIN_WM_MOUSEDRAGEND, 0, 0);
		object_state_.dragging = false;
	}
}

void ewin::application::object::track_mouse_(common::types::hwnd hwnd, common::types::uint flags){
	common::types::track_mouse_event track_info{ sizeof(common::types::track_mouse_event), (TME_LEAVE | flags), hwnd, 0 };
	::TrackMouseEvent(&track_info);//Notify when mouse leaves window or client area
}

ewin::common::types::result ewin::application::object::menu_init_(window_type &window_object, common::types::wparam wparam, common::types::lparam lparam){
	auto target = find_menu_(reinterpret_cast<common::types::hmenu>(wparam));
	if (target == nullptr)//Menu not found
		return ::CallWindowProcW(window_object.procedure_, window_object.handle_, WM_INITMENUPOPUP, wparam, lparam);

	menu::item *menu_item;
	common::types::uint index = 0;

	common::types::hmenu target_handle = target->handle;
	common::types::msg info{ nullptr, EWIN_WM_MENU_INIT };

	//Allow system and control menus to carry out default initialization
	::CallWindowProcW(window_object.procedure_, window_object.handle_, WM_INITMENUPOPUP, wparam, lparam);

	for (auto item : target->tree.children){//Query enabling of items
		if ((menu_item = dynamic_cast<menu::item *>(item)) != nullptr){
			common::types::menu_item_info item_info{
				sizeof(common::types::menu_item_info),					//Size
				MIIM_STATE												//Flags
			};

			if (EWIN_CPP_BOOL(::GetMenuItemInfoW(target_handle, index, TRUE, &item_info)))
				menu_item->states = item_info.fState;//Refresh states

			menu_item->enabled = (item->dispatch_message[info] == 1u);
		}

		++index;
	}

	return 0;
}

ewin::common::types::result ewin::application::object::menu_rbutton_up_(window_type &window_object, common::types::wparam wparam, common::types::lparam lparam){
	return ::CallWindowProcW(window_object.procedure_, window_object.handle_, WM_MENURBUTTONUP, wparam, lparam);
}

ewin::common::types::result ewin::application::object::menu_highlight_(window_type &window_object, common::types::wparam wparam, common::types::lparam lparam){
	if (!EWIN_IS(HIWORD(wparam), MF_HILITE)) 
		return ::CallWindowProcW(window_object.procedure_, window_object.handle_, WM_MENUSELECT, wparam, lparam);

	menu::container *target;
	if (EWIN_IS(HIWORD(wparam), MF_SYSMENU))//System menu
		target = find_menu_(::GetSystemMenu(window_object.handle_, FALSE));
	else//Normal menu
		target = find_menu_(reinterpret_cast<common::types::hmenu>(lparam));

	if (target == nullptr)//Menu not found
		return ::CallWindowProcW(window_object.procedure_, window_object.handle_, WM_MENUSELECT, wparam, lparam);

	menu::item *item = nullptr;
	if (!EWIN_IS(HIWORD(wparam), MF_POPUP)){//Search by ID
		menu::item *menu_child;
		for (auto child : target->tree.children){
			if ((menu_child = dynamic_cast<menu::item *>(child)) != nullptr && menu_child->id == LOWORD(wparam)){//Found
				item = menu_child;
				break;
			}
		}
	}
	else//Search by position
		item = dynamic_cast<menu::item *>(target->tree.children[LOWORD(wparam)]);

	if (item == nullptr)//Item not found
		return ::CallWindowProcW(window_object.procedure_, window_object.handle_, WM_MENUSELECT, wparam, lparam);

	return item->dispatch_message[common::types::msg{ nullptr, EWIN_WM_MENU_HIGHLIGHT }];
}

ewin::common::types::result ewin::application::object::menu_command_(window_type &window_object, common::types::wparam wparam, common::types::lparam lparam){
	auto target = find_menu_(reinterpret_cast<common::types::hmenu>(lparam));
	if (target == nullptr)//Menu not found
		return ::CallWindowProcW(window_object.procedure_, window_object.handle_, WM_MENUCOMMAND, wparam, lparam);

	auto item = dynamic_cast<menu::item *>(target->tree.children[wparam]);
	if (item == nullptr)//Item not found
		return ::CallWindowProcW(window_object.procedure_, window_object.handle_, WM_MENUCOMMAND, wparam, lparam);

	return item->dispatch_message[common::types::msg{ nullptr, EWIN_WM_MENU_SELECT }];
}

ewin::common::types::result ewin::application::object::command_(window_type &window_object, common::types::wparam wparam, common::types::lparam lparam){
	return ::CallWindowProcW(window_object.procedure_, window_object.handle_, WM_COMMAND, wparam, lparam);
}

ewin::common::types::result ewin::application::object::system_command_(window_type &window_object, common::types::wparam wparam, common::types::lparam lparam){
	if (window_object.system_menu_ == nullptr)
		return ::CallWindowProcW(window_object.procedure_, window_object.handle_, WM_SYSCOMMAND, wparam, lparam);

	menu::item *item = nullptr, *menu_child;
	for (auto child : window_object.system_menu_->tree.children){//Search by ID
		if ((menu_child = dynamic_cast<menu::item *>(child)) != nullptr && menu_child->id == LOWORD(wparam)){//Found
			item = menu_child;
			break;
		}
	}

	if (item != nullptr)//Item found
		item->dispatch_message[common::types::msg{ nullptr, EWIN_WM_MENU_SELECT }];
		
	return ::CallWindowProcW(window_object.procedure_, window_object.handle_, WM_SYSCOMMAND, wparam, lparam);
}

ewin::common::types::result ewin::application::object::app_message_(common::types::uint msg, common::types::wparam wparam, common::types::lparam lparam){
	switch (msg){
	case EWIN_WM_TASK://Execute task
		execute_task_(reinterpret_cast<task_type *>(wparam), EWIN_CPP_BOOL(lparam));
		break;
	case EWIN_WM_MENU_MSG:
		reinterpret_cast<ewin::message::menu_target *>(wparam)->dispatch_message[*reinterpret_cast<common::types::msg *>(lparam)];
		break;
	default:
		break;
	}

	return 0u;
}

ewin::common::types::result CALLBACK ewin::application::object::entry_(common::types::hwnd hwnd, common::types::uint msg, common::types::wparam wparam, common::types::lparam lparam){
	auto current = manager::current_;
	auto target = current->find_(hwnd);

	if (target == nullptr)//Unknown window
		return ::CallWindowProcW(::DefWindowProcW, hwnd, msg, wparam, lparam);

	if (target == current->message_window_.get() && msg >= EWIN_WM_APP_FIRST && msg <= EWIN_WM_APP_LAST)
		return current->app_message_(msg, wparam, lparam);

	switch (msg){
	case WM_NCMOUSELEAVE:
	case WM_MOUSELEAVE:
		current->mouse_leave_(hwnd, msg);
		return ::CallWindowProcW(::DefWindowProcW, hwnd, msg, wparam, lparam);
	case WM_NCMOUSEMOVE:
	case WM_MOUSEMOVE:
		current->mouse_move_(hwnd);
		break;
	case WM_LBUTTONDOWN:
		current->mouse_down_(hwnd, MK_LBUTTON);
		break;
	case WM_MBUTTONDOWN:
		current->mouse_down_(hwnd, MK_MBUTTON);
		break;
	case WM_RBUTTONDOWN:
		current->mouse_down_(hwnd, MK_RBUTTON);
		break;
	case WM_NCLBUTTONUP:
	case WM_LBUTTONUP:
		current->mouse_up_(hwnd, MK_LBUTTON);
		break;
	case WM_NCMBUTTONUP:
	case WM_MBUTTONUP:
		current->mouse_up_(hwnd, MK_MBUTTON);
		break;
	case WM_NCRBUTTONUP:
	case WM_RBUTTONUP:
		current->mouse_up_(hwnd, MK_RBUTTON);
		break;
	case WM_INITMENUPOPUP:
		return current->menu_init_(*target, wparam, lparam);
	case WM_MENURBUTTONUP:
		return current->menu_rbutton_up_(*target, wparam, lparam);
	case WM_MENUSELECT:
		return current->menu_highlight_(*target, wparam, lparam);
	case WM_MENUCOMMAND:
		return current->menu_command_(*target, wparam, lparam);
	case WM_COMMAND:
		return current->command_(*target, wparam, lparam);
	case WM_SYSCOMMAND:
		return current->system_command_(*target, wparam, lparam);
	default:
		break;
	}

	auto result = target->dispatch_message[common::types::msg{ hwnd, msg, wparam, lparam }];
	switch (msg){
	case WM_CREATE:
		if (result == 0u){
			target->handle_ = hwnd;
			target->cache_dimensions_();
		}
		break;
	case WM_NCDESTROY:
		current->destroy_window_(hwnd);
		break;
	case WM_ACTIVATEAPP:
		if (!EWIN_CPP_BOOL(wparam)){//Another app activated
			for (; current->object_state_.moused != HWND_DESKTOP; current->object_state_.moused = ::GetParent(current->object_state_.moused))
				::SendMessageW(current->object_state_.moused, WM_NCMOUSELEAVE, 0, 0);//Alert
		}
		break;
	case WM_WINDOWPOSCHANGED:
	{//Check for size and position changes
		auto info = reinterpret_cast<common::types::wnd_position *>(lparam);
		if (!EWIN_IS(info->flags, (SWP_NOSIZE | SWP_NOMOVE)))
			target->cache_dimensions_();

		if (!EWIN_IS(info->flags, SWP_NOSIZE))
			current->size_window_(*target);

		if (!EWIN_IS(info->flags, SWP_NOMOVE))
			current->move_window_(*target);

		break;
	}
	case EWIN_WM_POSITION_CHANGE:
		target->cache_dimensions_();
		break;
	case WM_SETFOCUS:
		current->focus_window_(hwnd);
		break;
	default:
		break;
	}

	return result;
}

ewin::common::types::result CALLBACK ewin::application::object::hook_(int code, common::types::wparam wparam, common::types::lparam lparam){
	switch (code){
	case HCBT_CREATEWND:
		manager::current_->create_window_(reinterpret_cast<common::types::hwnd>(wparam), *reinterpret_cast<common::types::hook_create_window_info *>(lparam));
		break;
	case HCBT_DESTROYWND:
		::SetMenu(reinterpret_cast<common::types::hwnd>(wparam), nullptr);//Prevent menu, if any, destruction
		break;
	default:
		break;
	}

	return ::CallNextHookEx(nullptr, code, wparam, lparam);
}

thread_local ewin::common::random_bool ewin::application::object::bool_generator;

thread_local ewin::common::random_integral_number ewin::application::object::integer_generator;

thread_local ewin::common::random_real_number ewin::application::object::real_generator;

thread_local ewin::common::random_string ewin::application::object::string_generator;
