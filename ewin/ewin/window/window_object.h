#pragma once

#ifndef EWIN_WINDOW_OBJECT_H
#define EWIN_WINDOW_OBJECT_H

#include <list>
#include <memory>

#include "../common/type_aliases.h"
#include "../common/boolean_property.h"
#include "../common/string_property.h"
#include "../common/state_property.h"
#include "../common/list_property.h"
#include "../common/extended_value_property.h"
#include "../common/size_property.h"
#include "../common/point_property.h"
#include "../common/rect_property.h"
#include "../common/object_property.h"
#include "../common/validation_property.h"

#include "../application/application_object.h"

#include "window_class.h"
#include "window_frame.h"

namespace ewin::window{
	class object : public std::enable_shared_from_this<object>{
	public:
		typedef application::object application_type;

		typedef common::error_type error_type;
		typedef common::error_throw_policy_type error_throw_policy_type;

		typedef common::point_value_property_backend<int> point_type;
		typedef common::size_value_property_backend<int> size_type;
		typedef common::rect_value_property_backend<int> rect_type;

		typedef std::shared_ptr<object> ptr_type;
		typedef std::list<ptr_type> object_list_type;

		typedef object_list_type::iterator object_list_iterator_type;
		typedef object_list_type::const_iterator object_list_const_iterator_type;

		struct create_info{};

		object();

		virtual ~object();

		common::read_only_value_property<ptr_type, object> reflect;
		common::validation_property<bool, void *, object> is_forbidden;

		common::value_property<error_throw_policy_type, object> error_throw_policy;
		common::value_property<error_type, object> error;

		common::extended_value_property<application_type, application_type *, object> app;
		common::read_only_value_property<common::types::hwnd, object> handle;
		common::read_only_value_property<common::types::procedure, object> procedure;

		common::size_value_property<int, object> size;
		common::size_value_property<int, object> client_size;

		common::point_value_property<int, object> position;
		common::point_value_property<int, object> relative_position;

		common::rect_value_property<int, object> rect;
		common::rect_value_property<int, object> relative_rect;
		common::read_only_rect_value_property<int, object> client_rect;

		/*common::extended_value_property<object, object *, object> parent;

		common::extended_value_property<object, object *, object> previous_sibling;
		common::extended_value_property<object, object *, object> next_sibling;

		common::list_value_property<object, object_list_iterator_type, object_list_const_iterator_type, object> children;
		common::list_value_property<object, object_list_iterator_type, object_list_const_iterator_type, object> siblings;
		common::list_value_property<object, object_list_iterator_type, object_list_const_iterator_type, object> ancestors;*/

		common::object_value_property<wnd_frame, object> tree;
		common::object_value_property<wnd_frame, object> view;
		common::object_value_property<wnd_frame, object> frame;
		common::object_value_property<wnd_frame, object> style;
		common::object_value_property<wnd_frame, object> state;

		/*common::boolean_value_property<object> visible;
		common::boolean_value_property<object> enabled;

		common::boolean_value_property<object> maximized;
		common::boolean_value_property<object> minimized;

		common::boolean_value_property<object> show_caption;
		common::boolean_value_property<object> show_system_menu;

		common::boolean_value_property<object> clip_children;
		common::boolean_value_property<object> clip_siblings;

		common::boolean_value_property<object> tab_stop;
		common::boolean_value_property<object> file_drop;
		common::boolean_value_property<object> mouse_activate;

		common::boolean_value_property<object> right_align;
		common::boolean_value_property<object> rtl_reading;
		common::boolean_value_property<object> rtl_layout;

		common::boolean_value_property<object> top_most;
		common::boolean_value_property<object> transparent;*/

		common::boolean_value_property<object> created;
		common::write_only_value_property<create_info, object> create;
		common::boolean_value_property<object> auto_destroy;

	private:
		void bind_properties_();

		virtual void handle_property_(void *prop, void *arg, common::property_access access);

		virtual ptr_type reflect_();

		virtual bool is_forbidden_(void *target);

		void create_(bool create, const create_info *info);

		virtual void set_rect_(const rect_type &value, bool relative);

		virtual rect_type get_rect_(bool relative) const;

		virtual rect_type get_client_rect_() const;

		virtual void set_position_(const point_type &value, bool relative);

		virtual point_type get_position_(bool relative) const;

		virtual void set_size_(const size_type &value, bool client);

		virtual size_type get_size_(bool client) const;

		application_type *app_;
		common::types::hwnd handle_;
		common::types::procedure procedure_;

		error_throw_policy_type error_throw_policy_;
		bool auto_destroy_;

		wnd_frame frame_;
	};
}

#endif /* !EWIN_WINDOW_OBJECT_H */
