#pragma once

#ifndef EWIN_APPLICATION_OBJECT_H
#define EWIN_APPLICATION_OBJECT_H

#include <thread>
#include <unordered_map>

#include "../common/type_aliases.h"
#include "../common/boolean_property.h"
#include "../common/object_property.h"
#include "../common/list_property.h"

#include "../common/random_bool.h"
#include "../common/random_number.h"
#include "../common/random_string.h"

#include "../window/window_class.h"

#include "../drawing/drawing_factory.h"
#include "../drawing/hdc_drawing_object.h"
#include "../drawing/solid_color_drawing_brush.h"

#define EWIN_UUID		 "{DABED3E8-D8A5-48FC-B80B-B17C167FA9B0}"
#define EWIN_WUUID		L"{DABED3E8-D8A5-48FC-B80B-B17C167FA9B0}"

#define EWIN_WM_APP_FIRST	(WM_APP + 1)
#define EWIN_WM_APP_LAST	(WM_APP + 10)

#define EWIN_WM_TASK		(EWIN_WM_APP_FIRST + 0)

namespace ewin::menu{
	class container;
}

namespace ewin::window{
	class object;
}

namespace ewin::application{
	class manager;

	class object{
	public:
		typedef common::error_type error_type;
		typedef window::object window_type;

		typedef std::function<void()> task_type;
		typedef std::reference_wrapper<window_type> window_ref_type;

		typedef std::list<common::types::hwnd> hwnd_list_type;
		typedef std::unordered_map<common::types::hwnd, window_type *> window_map_type;
		typedef std::unordered_map<common::types::hmenu, menu::container *> menu_map_type;

		typedef hwnd_list_type::iterator window_list_iterator_type;
		typedef hwnd_list_type::const_iterator window_list_const_iterator_type;

		struct object_state{
			common::types::hwnd focused;
			common::types::hwnd moused;
			common::types::point last_mouse_position;
			common::types::point mouse_down_position;
			common::types::uint mouse_button_down;
			bool dragging;
		};

		object();

		~object();

		object(const object &) = delete;
		
		object &operator =(const object &) = delete;

		common::list_value_property<task_type, void *, void *, object, std::size_t, common::property_access::list_add> task;
		common::list_value_property<task_type, void *, void *, object, std::size_t, common::property_access::list_add> async_task;

		common::access_only_list_value_property<menu::container, object, common::types::hmenu> menu_handles;
		common::access_only_list_value_property<window_type, object, common::types::hwnd> window_handles;
		common::iterator_only_list_value_property<window_type, window_list_iterator_type, window_list_const_iterator_type, object> top_level_handles;
		common::write_only_value_property<window_type *, object> window_being_created;
		common::read_only_value_property<int, object> run;

		common::read_only_object_value_property<drawing::factory, object> drawing_factory;
		common::read_only_object_value_property<drawing::hdc_object, object> hdc_drawer;
		common::read_only_object_value_property<drawing::solid_color_brush, object> color_brush;

		common::read_only_point_value_property<common::types::point, object> last_mouse_position;
		common::write_only_boolean_value_property<object> update_last_mouse_position;

		static thread_local common::random_bool bool_generator;
		static thread_local common::random_integral_number integer_generator;
		static thread_local common::random_real_number real_generator;
		static thread_local common::random_string string_generator;

	protected:
		friend class manager;
		friend class menu::container;

		explicit object(bool is_main);

		void bind_properties_();

		void handle_property_(void *prop, void *arg, common::property_access access);

		void task_(const task_type &callback);

		void async_task_(const task_type &callback);

		void execute_task_(task_type *callback, bool is_async);

		window_type *find_(common::types::hwnd handle);

		menu::container *find_menu_(common::types::hmenu handle);

		int run_();

		bool is_filtered_message_(const common::types::msg &msg) const;

		bool is_dialog_message_(const common::types::msg &msg);

		void dispatch_message_(const common::types::msg &msg);

		void dispatch_thread_message_(const common::types::msg &msg);

		void translate_message_(const common::types::msg &msg);

		void create_window_(common::types::hwnd handle, common::types::hook_create_window_info &info);

		void destroy_window_(common::types::hwnd handle);

		void focus_window_(common::types::hwnd handle);

		void size_window_(window_type &window_object);

		void move_window_(window_type &window_object);

		void mouse_leave_(common::types::hwnd hwnd, common::types::uint msg);

		void mouse_move_(common::types::hwnd hwnd);

		void mouse_down_(common::types::hwnd hwnd, common::types::uint button);

		void mouse_up_(common::types::hwnd hwnd, common::types::uint button);

		void track_mouse_(common::types::hwnd hwnd, common::types::uint flags);

		common::types::result app_message_(common::types::uint msg, common::types::wparam wparam, common::types::lparam lparam);

		static common::types::result CALLBACK entry_(common::types::hwnd hwnd, common::types::uint msg, common::types::wparam wparam, common::types::lparam lparam);

		static common::types::result CALLBACK hook_(int code, common::types::wparam wparam, common::types::lparam lparam);

		std::thread::id thread_id_;
		std::shared_ptr<window_type> message_window_;

		object_state object_state_{};
		hwnd_list_type top_level_handles_;
		window_map_type window_handles_;
		menu_map_type menu_handles_;

		std::pair<common::types::hwnd, window_type *> cached_window_handle_;
		std::pair<common::types::hmenu, menu::container *> cached_menu_handle_;

		window_type *window_being_created_;
		common::types::hook hook_id_;

		drawing::factory drawing_factory_;
		drawing::hdc_object hdc_drawer_;
		drawing::solid_color_brush color_brush_;
	};
}

#endif /* !EWIN_APPLICATION_OBJECT_H */
