#pragma once

#ifndef EWIN_MENU_OBJECT_H
#define EWIN_MENU_OBJECT_H

#include "../common/string_property.h"
#include "../common/state_property.h"
#include "../common/size_property.h"
#include "../common/point_property.h"
#include "../common/rect_property.h"
#include "../common/transformation_property.h"
#include "../common/variant_property.h"

#include "../message/menu_message_target.h"

#include "../application/application_manager.h"

#include "../window/window_tree.h"

#include "menu_event.h"

namespace ewin::menu{
	class object : public std::enable_shared_from_this<object>, public ewin::message::menu_target{
	public:
		typedef ewin::message::menu_target message_target_type;
		typedef application::object application_type;

		typedef common::error_type error_type;
		typedef common::error_throw_policy_type error_throw_policy_type;

		typedef std::shared_ptr<object> ptr_type;

		struct property_forbidden_info{
			void *value;
			common::property_access access;
		};

		struct create_info{};

		struct cache_info{
			common::types::rect rect;
			common::types::point position;
			common::types::size size;
		};

		object();

		virtual ~object();

		common::read_only_value_property<ptr_type, object> reflect;
		common::transformation_property<property_forbidden_info, bool, object> is_forbidden;

		common::value_property<error_throw_policy_type, object> error_throw_policy;
		common::variant_value_property<object, common::property_access::nil, error_type, common::types::dword> error;

		common::object_value_property<application_type, object> app;
		common::read_only_value_property<common::types::hmenu, object> handle;

		common::size_value_property<common::types::size, object> size;
		common::point_value_property<common::types::point, object> position;
		common::rect_value_property<common::types::rect, object> rect;

		common::boolean_value_property<object> created;
		common::transformation_property<create_info, void, object> create;
		common::boolean_value_property<object> auto_destroy;

		window::wnd_tree<object> tree;
		common::read_only_object_value_property<menu_event, object> events;

	protected:
		friend class window::wnd_tree<object>;

		template <class, class>
		friend class events::typed_basic;

		virtual ewin::message::menu_target *parent_() const override;

		void bind_properties_();

		virtual void handle_property_(void *prop, void *arg, common::property_access access);

		virtual ptr_type reflect_();

		virtual common::types::hmenu handle_value_();

		virtual bool is_forbidden_(const property_forbidden_info &info);

		virtual void destruct_();

		virtual void create_(bool create, const create_info *info);

		virtual void set_error_(common::variant_value_property_arg_info &info);

		virtual void set_error_(error_type value);

		virtual void set_error_(common::types::dword value);

		virtual bool validate_parent_change_(object *value);

		virtual bool validate_child_remove_(object &value);

		virtual bool validate_child_add_(object &value, std::size_t index);

		virtual void child_removed_(object &value, std::size_t index);

		virtual void child_added_(object &value, std::size_t index);

		virtual void parent_changed_(object *current, object *previous, std::size_t index, std::size_t previous_index);

		virtual void event_listener_count_changed_(events::menu_basic &e, std::size_t count);

		application_type *app_;
		cache_info cache_;

		error_throw_policy_type error_throw_policy_;
		error_type error_value_;
		common::types::dword local_error_value_;

		bool auto_destroy_;
		menu_event events_;
	};
}

#endif /* !EWIN_MENU_OBJECT_H */
