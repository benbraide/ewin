#pragma once

#ifndef EWIN_WINDOW_OBJECT_H
#define EWIN_WINDOW_OBJECT_H

#include "../common/string_property.h"
#include "../common/state_property.h"
#include "../common/size_property.h"
#include "../common/point_property.h"
#include "../common/rect_property.h"
#include "../common/transformation_property.h"
#include "../common/variant_property.h"

#include "../application/application_object.h"

#include "window_class.h"
#include "window_frame.h"
#include "window_tree.h"
#include "window_view.h"
#include "window_state.h"
#include "window_style.h"

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
		
		struct property_forbidden_info{
			void *value;
			common::property_access access;
		};

		struct create_info{};

		struct parent_change_info{
			object *old;
			object *current;
		};

		struct child_change_info{
			object *value;
			std::size_t index;
			bool removed;
		};

		object();

		virtual ~object();

		common::read_only_value_property<ptr_type, object> reflect;
		common::transformation_property<bool, void *, object> is_forbidden;

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

		common::transformation_property<common::types::uint, common::types::uint, object> filter_styles;
		common::transformation_property<common::types::uint, common::types::uint, object> filter_extended_styles;

		common::read_only_size_value_property<common::types::uint, object> filtered_styles;
		common::read_only_size_value_property<common::types::uint, object> filtered_extended_styles;

		wnd_tree tree;
		wnd_view view;
		wnd_frame frame;
		wnd_state state;
		wnd_style style;

		common::boolean_value_property<object> created;
		common::transformation_property<create_info, void, object> create;
		common::boolean_value_property<object> auto_destroy;

		common::write_only_variant_value_property<object, parent_change_info, child_change_info> changed;

	private:
		void bind_properties_();

		virtual void handle_property_(void *prop, void *arg, common::property_access access);

		virtual ptr_type reflect_();

		virtual bool is_forbidden_(const property_forbidden_info &info);

		void create_(bool create, const create_info *info);

		virtual void set_error_(error_type value);

		virtual void set_rect_(const rect_type &value, bool relative);

		virtual rect_type get_rect_(bool relative) const;

		virtual rect_type get_client_rect_() const;

		virtual void set_position_(const point_type &value, bool relative);

		virtual point_type get_position_(bool relative) const;

		virtual void set_size_(const size_type &value, bool client);

		virtual size_type get_size_(bool client) const;

		virtual void filter_styles_(std::pair<common::types::uint *, common::types::uint> &info, bool is_extended) const;

		virtual common::types::uint filter_styles_(common::types::uint value, bool is_extended) const;

		virtual common::types::uint filtered_styles_(bool is_extended) const;

		virtual common::types::uint white_listed_styles_(bool is_extended) const;

		virtual common::types::uint black_listed_styles_(bool is_extended) const;

		application_type *app_;
		common::types::hwnd handle_;
		common::types::procedure procedure_;

		error_throw_policy_type error_throw_policy_;
		error_type error_value_;
		bool auto_destroy_;
	};
}

#endif /* !EWIN_WINDOW_OBJECT_H */
