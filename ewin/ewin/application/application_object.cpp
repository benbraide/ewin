#include "../window/message_window.h"

ewin::application::object::object()
	: cached_window_handle_(std::make_pair<common::types::hwnd, window_type *>(nullptr, nullptr)), window_being_created_(nullptr), hook_id_(nullptr){
	bind_properties_();
}

ewin::application::object::~object(){
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
	window_handles.initialize_(nullptr, handler);
	top_level_windows.initialize_(nullptr, handler);
	window_being_created.initialize_(nullptr, handler);
	run.initialize_(nullptr, handler);
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
	else if (prop == &top_level_windows){
		task_([&]{//Execute in thread context
			if (access == common::property_access::list_begin)
				*reinterpret_cast<window_list_iterator_type *>(arg) = top_level_windows_.begin();
			else if (access == common::property_access::list_end)
				*reinterpret_cast<window_list_iterator_type *>(arg) = top_level_windows_.end();
		});
	}
	else if (prop == &window_being_created){
		if (std::this_thread::get_id() != thread_id_)//Can only be accessed from same thread
			throw common::error_type::cross_thread;

		window_being_created_ = *static_cast<window_type **>(arg);
		if (window_being_created_ == nullptr && hook_id_ != nullptr){//Remove hook
			::UnhookWindowsHookEx(hook_id_);
			hook_id_ = nullptr;
		}
		else if (window_being_created_ != nullptr && hook_id_ == nullptr)//Install hook to intercept certain events
			hook_id_ = ::SetWindowsHookExW(WH_CBT, hook_, nullptr, ::GetCurrentThreadId());
	}
	else if (prop == &run){
		if (std::this_thread::get_id() == thread_id_)
			*static_cast<int *>(arg) = run_();
		else//Can only be accessed from same thread
			throw common::error_type::cross_thread;
	}
	else if (prop == &task)
		task_(*reinterpret_cast<std::pair<void *, task_type *> *>(arg)->second);
}

void ewin::application::object::task_(const task_type &callback){
	if (std::this_thread::get_id() == thread_id_)
		callback();//Same thread
	else//Execute in thread
		message_window_->send_message[window_type::message_info{ EWIN_WM_TASK, EWIN_OBJECT_WPARAM_CAST(callback) }];
}

ewin::application::object::window_type *ewin::application::object::find_(common::types::hwnd handle){
	if (handle == cached_window_handle_.first)//Use cached value
		return cached_window_handle_.second;

	auto entry = window_handles_.find(handle);
	auto value = ((entry == window_handles_.end()) ? nullptr : entry->second);

	if (value != nullptr)//Update cached value
		cached_window_handle_ = std::make_pair(handle, value);

	return value;
}

int ewin::application::object::run_(){
	common::types::msg msg;
	while (!top_level_windows_.empty()){
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
	return ((object_state_.focused == nullptr) ? false : object_state_.focused->is_dialog_message[msg]);
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

ewin::common::types::result CALLBACK ewin::application::object::entry_(common::types::hwnd hwnd, common::types::uint msg, common::types::wparam wparam, common::types::lparam lparam){
	auto current = manager::current_;
	if (hwnd == current->message_window_->handle){//Tunneled message
		switch (msg){
		case EWIN_WM_TASK://Execute task
			(*reinterpret_cast<task_type *>(wparam))();
			break;
		default:
			return ::CallWindowProcW(::DefWindowProcW, hwnd, msg, wparam, lparam);
		}

		return 0u;
	}

	auto target = current->find_(hwnd);
	if (target == nullptr)//Unknown window
		return ::CallWindowProcW(::DefWindowProcW, hwnd, msg, wparam, lparam);

	return target->dispatch_message[common::types::msg{ hwnd, msg, wparam, lparam }];
}

ewin::common::types::result CALLBACK ewin::application::object::hook_(int code, common::types::wparam wparam, common::types::lparam lparam){
	auto current = manager::current_;
	if (code == HCBT_CREATEWND){//Respond to window creation
		auto info = reinterpret_cast<common::types::hook_create_window_info *>(lparam)->lpcs;
		if (info->lpCreateParams != current->window_being_created_)
			return ::CallNextHookEx(nullptr, code, wparam, lparam);//Different window

		auto handle = reinterpret_cast<common::types::hwnd>(wparam);
		if (current->window_being_created_->attribute.is_control)//Replace procedure
			::SetWindowLongPtrW(handle, GWLP_WNDPROC, *reinterpret_cast<common::types::ptr *>(entry_));

		current->cached_window_handle_ = std::make_pair(handle, current->window_being_created_);
		current->window_handles_[handle] = current->window_being_created_;
	}

	return ::CallNextHookEx(nullptr, code, wparam, lparam);
}
