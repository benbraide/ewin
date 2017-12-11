#pragma once

#ifndef EWIN_APPLICATION_OBJECT_H
#define EWIN_APPLICATION_OBJECT_H

#include <thread>
#include <unordered_map>

#include "../common/type_aliases.h"
#include "../common/boolean_property.h"
#include "../common/object_property.h"
#include "../common/list_property.h"

#include "../window/window_class.h"

#define EWIN_UUID	 "{DABED3E8-D8A5-48FC-B80B-B17C167FA9B0}"
#define EWIN_WUUID	L"{DABED3E8-D8A5-48FC-B80B-B17C167FA9B0}"

namespace ewin::window{
	class object;
}

namespace ewin::application{
	class manager;
	class manager_initializer;

	class object{
	public:
		typedef common::error_type error_type;
		typedef window::object window_type;

		typedef std::function<void()> task_type;
		typedef std::reference_wrapper<window_type> window_ref_type;

		typedef std::list<window_ref_type> window_list_type;
		typedef std::unordered_map<common::types::hwnd, window_type *> window_map_type;

		typedef window_list_type::iterator window_list_iterator_type;
		typedef window_list_type::const_iterator window_list_const_iterator_type;

		~object();

		object(const object &) = delete;
		
		object &operator =(const object &) = delete;

		common::list_value_property<task_type, void *, void *, object, std::size_t, common::property_access::list_add> task;
		common::access_only_list_value_property<window_type, object, common::types::hwnd> window_handles;
		common::iterator_only_list_value_property<window_type, window_list_iterator_type, window_list_const_iterator_type, object> top_level_windows;
		common::write_only_value_property<window_type *, object> window_being_created;

	private:
		friend class manager;
		friend class manager_initializer;

		object();

		void bind_properties_();

		void handle_property_(void *prop, void *arg, common::property_access access);

		void task_(const task_type &callback);

		window_type *find_(common::types::hwnd handle);

		static common::types::result CALLBACK entry_(common::types::hwnd hwnd, common::types::uint msg, common::types::wparam wparam, common::types::lparam lparam);

		static common::types::result CALLBACK hook_(int code, common::types::wparam wparam, common::types::lparam lparam);

		std::thread::id thread_id_;
		std::shared_ptr<window_type> message_window_;
		window_list_type top_level_windows_;
		window_map_type window_handles_;
		std::pair<common::types::hwnd, window_type *> cached_window_handle_;
		window_type *window_being_created_;
		common::types::hook hook_id_;
	};
}

#endif /* !EWIN_APPLICATION_OBJECT_H */
