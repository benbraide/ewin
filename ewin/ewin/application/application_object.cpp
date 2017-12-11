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
	auto handler = std::bind(&object::handle_property_, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	task.initialize_(nullptr, handler);
	window_handles.initialize_(nullptr, handler);
	top_level_windows.initialize_(nullptr, handler);
	window_being_created.initialize_(nullptr, handler);
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
		window_being_created_ = *static_cast<window_type **>(arg);
		if (window_being_created_ == nullptr && hook_id_ != nullptr){//Remove hook
			::UnhookWindowsHookEx(hook_id_);
			hook_id_ = nullptr;
		}
		else if (window_being_created_ != nullptr && hook_id_ == nullptr)//Install hook to intercept certain events
			hook_id_ = ::SetWindowsHookExW(WH_CBT, hook_, nullptr, ::GetCurrentThreadId());
	}
	else if (prop == &task)
		task_(*static_cast<task_type *>(arg));
}

void ewin::application::object::task_(const task_type &callback){
	if (std::this_thread::get_id() == thread_id_)
		callback();//Same thread
	else//Execute in thread
		message_window_->send_message[window_type::message_info{ WM_NULL, EWIN_OBJECT_WPARAM_CAST(callback) }];
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

ewin::common::types::result CALLBACK ewin::application::object::entry_(common::types::hwnd hwnd, common::types::uint msg, common::types::wparam wparam, common::types::lparam lparam){
	auto current = manager::current_;
	if (hwnd == current->message_window_->handle){//Tunneled message
		switch (msg){
		case WM_NULL://Execute task
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

	return ::CallWindowProcW(::DefWindowProcW, hwnd, msg, wparam, lparam);
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
