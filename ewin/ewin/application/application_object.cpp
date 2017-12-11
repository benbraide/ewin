#include "../window/window_object.h"

ewin::application::object::object(){
	manager::application_list_.push_back(*this);
	bind_properties_();
}

ewin::application::object::~object(){
	for (auto iter = manager::application_list_.begin(); iter != manager::application_list_.end(); ++iter){
		if (&iter->get() == this){
			manager::application_list_.erase(iter);
			break;
		}
	}
}

void ewin::application::object::bind_properties_(){
	auto handler = std::bind(&object::handle_property_, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	task.initialize_(nullptr, handler);
	window_handles.initialize_(nullptr, handler);
	top_level_windows.initialize_(nullptr, handler);
}

void ewin::application::object::handle_property_(void *prop, void *arg, common::property_access access){
	if (prop == &window_handles){
		task_([&]{//Execute in thread context
			if (access == common::property_access::list_at){
				auto &info = *reinterpret_cast<std::pair<common::types::hwnd, window_type *> *>(arg);
				auto entry = window_handles_.find(info.first);
				info.second = ((entry == window_handles_.end()) ? nullptr : &entry->second.get());
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
	else if (prop == &task)
		task_(*static_cast<task_type *>(arg));
}

void ewin::application::object::task_(const task_type &callback){
	if (std::this_thread::get_id() == thread_id_)
		callback();//Same thread
	else//Execute in thread
		get_message_window_().send_message[window_type::message_info{ WM_NULL, EWIN_OBJECT_WPARAM_CAST(callback) }];
}

ewin::application::object::window_type &ewin::application::object::get_message_window_(){
	return *message_window_;
}

ewin::common::types::result CALLBACK ewin::application::object::entry_(common::types::hwnd hwnd, common::types::uint msg, common::types::wparam wparam, common::types::lparam lparam){
	return 0;
}
