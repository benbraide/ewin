#pragma once

#ifndef EWIN_APPLICATION_OBJECT_H
#define EWIN_APPLICATION_OBJECT_H

#include <thread>

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

		common::list_value_property<task_type, void *, void *, object, common::property_access::list_add> task;

	private:
		friend class manager;
		friend class manager_initializer;

		object();

		void bind_properties_();

		void handle_property_(void *prop, void *arg, common::property_access access);

		void task_(task_type &callback);

		static common::types::result CALLBACK entry_(common::types::hwnd hwnd, common::types::uint msg, common::types::wparam wparam, common::types::lparam lparam);

		window_type &get_message_window_();

		std::thread::id thread_id_;
		std::shared_ptr<window_type> message_window_;
	};
}

#endif /* !EWIN_APPLICATION_OBJECT_H */
