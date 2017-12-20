#include "../window/message_window.h"

ewin::application::object::object()
	: object(false){}

ewin::application::object::object(bool is_main)
	: cached_window_handle_(std::make_pair<common::types::hwnd, window_type *>(nullptr, nullptr)), window_being_created_(nullptr){
	if (!is_main && manager::main_ == nullptr)
		throw error_type::no_main_app;

	if (is_main && manager::main_ != nullptr)
		throw error_type::multiple_main_app;

	if (manager::current_ != nullptr)
		throw error_type::multiple_app;

	manager::current_ = this;
	manager::application_list_.push_back(*this);

	hook_id_ = ::SetWindowsHookExW(WH_CBT, hook_, nullptr, ::GetCurrentThreadId());
	bind_properties_();

	thread_id_ = std::this_thread::get_id();
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

	window_handles.initialize_(nullptr, handler);
	top_level_handles.initialize_(nullptr, handler);
	window_being_created.initialize_(nullptr, handler);

	run.initialize_(nullptr, handler);
	drawing_factory.initialize_(nullptr, handler);
}

void ewin::application::object::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &window_handles){
		task_([&]{//Execute in thread context
			if (access == common::property_access::list_at){
				auto &info = *reinterpret_cast<std::pair<common::types::hwnd, window_type *> *>(arg);
				info.second = find_(info.first);
			}
			else if (access == common::property_access::list_find){
				auto &info = *reinterpret_cast<std::pair<common::types::hwnd, window_type *> *>(arg);
				info.first = info.second->handle;
			}
			else if (access == common::property_access::list_size)
				*reinterpret_cast<std::size_t *>(arg) = window_handles_.size();
		});
	}
	else if (prop == &top_level_handles){
		task_([&]{//Execute in thread context
			if (access == common::property_access::list_begin)
				*reinterpret_cast<window_list_iterator_type *>(arg) = top_level_handles_.begin();
			else if (access == common::property_access::list_end)
				*reinterpret_cast<window_list_iterator_type *>(arg) = top_level_handles_.end();
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
	else if (prop == &task)
		task_(*reinterpret_cast<std::pair<void *, task_type *> *>(arg)->second);
	else if (prop == &async_task)
		async_task_(*reinterpret_cast<std::pair<void *, task_type *> *>(arg)->second);
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
		::SetWindowLongPtrW(handle, GWLP_WNDPROC, *reinterpret_cast<common::types::ptr *>(entry_));
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

	if (object_state_.focused == handle){//Reset focused
		if (object_state_.moused = object_state_.focused)
			object_state_.moused = nullptr;//Reset moused
		object_state_.focused = nullptr;
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

ewin::common::types::result ewin::application::object::app_message_(common::types::uint msg, common::types::wparam wparam, common::types::lparam lparam){
	switch (msg){
	case EWIN_WM_TASK://Execute task
		execute_task_(reinterpret_cast<task_type *>(wparam), EWIN_CPP_BOOL(lparam));
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

	if (target == current->message_window_.get() && msg > WM_APP)
		return current->app_message_(msg, wparam, lparam);

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
	default:
		break;
	}

	return ::CallNextHookEx(nullptr, code, wparam, lparam);
}

thread_local ewin::common::random_bool ewin::application::object::bool_generator;

thread_local ewin::common::random_integral_number ewin::application::object::integer_generator;

thread_local ewin::common::random_real_number ewin::application::object::real_generator;

thread_local ewin::common::random_string ewin::application::object::string_generator;
