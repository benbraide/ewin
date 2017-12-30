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

#include "../message/message_target.h"

#include "../menu/menu_collection.h"

#include "../drawing/hwnd_drawing_object.h"

#include "window_class.h"
#include "window_frame.h"
#include "window_tree.h"
#include "window_view.h"
#include "window_state.h"
#include "window_style.h"
#include "window_attribute.h"
#include "window_event.h"

namespace ewin::window{
	class object : public std::enable_shared_from_this<object>, public ewin::message::target{
	public:
		typedef ewin::message::target message_target_type;
		typedef application::object application_type;

		typedef common::error_type error_type;
		typedef common::error_throw_policy_type error_throw_policy_type;

		typedef std::shared_ptr<object> ptr_type;

		enum class attribute_option_type : unsigned int{
			nil					= (0 << 0x0000),
			client_size			= (1 << 0x0000),
			absolute_position	= (1 << 0x0001),
		};
		
		struct property_forbidden_info{
			void *value;
			common::property_access access;
		};

		struct message_info{
			common::types::uint value;
			common::types::wparam wparam;
			common::types::lparam lparam;
		};

		struct create_info{};

		struct cache_info{
			common::types::rect rect;
			common::types::rect relative_rect;
			common::types::rect client_rect;
			common::types::point position;
			common::types::point relative_position;
			common::types::size size;
			common::types::size client_size;
		};

		struct parent_change_info{
			object *old;
			object *current;
		};

		struct child_change_info{
			object *value;
			std::size_t index;
			bool removed;
		};

		enum class dimension_type{
			rect,
			relative_rect,
			client_rect,
			position,
			relative_position,
			size,
			client_size,
		};

		object();

		virtual ~object();

		common::read_only_value_property<ptr_type, object> reflect;
		common::transformation_property<property_forbidden_info, bool, object> is_forbidden;

		common::value_property<error_throw_policy_type, object> error_throw_policy;
		common::variant_value_property<object, common::property_access::nil, error_type, common::types::dword> error;

		common::object_value_property<application_type, object> app;
		common::read_only_value_property<common::types::hwnd, object> handle;
		common::read_only_value_property<common::types::procedure, object> procedure;

		common::size_value_property<common::types::size, object> size;
		common::size_value_property<common::types::size, object> client_size;

		common::point_value_property<common::types::point, object> position;
		common::point_value_property<common::types::point, object> relative_position;

		common::rect_value_property<common::types::rect, object> rect;
		common::rect_value_property<common::types::rect, object> relative_rect;
		common::read_only_rect_value_property<common::types::rect, object> client_rect;

		common::transformation_property<message_info, common::types::result, object> send_message;
		common::transformation_property<message_info, void, object> post_message;

		common::transformation_property<common::types::uint, common::types::uint, object> filter_styles;
		common::transformation_property<common::types::uint, common::types::uint, object> filter_extended_styles;

		common::read_only_state_value_property<common::types::uint, object> filtered_styles;
		common::read_only_state_value_property<common::types::uint, object> filtered_extended_styles;

		common::transformation_property<common::types::msg, bool, object> is_dialog_message;

		wnd_tree<object> tree;
		wnd_view view;
		wnd_frame frame;
		wnd_state state;
		wnd_style style;
		wnd_attribute attribute;
		common::read_only_object_value_property<wnd_event, object> events;

		common::boolean_value_property<object> created;
		common::transformation_property<create_info, void, object> create;
		common::boolean_value_property<object> auto_destroy;

		common::read_only_object_value_property<ewin::menu::bar_collection, object> menu;
		common::read_only_object_value_property<ewin::menu::external_collection, object> system_menu;

		common::read_only_object_value_property<drawing::hwnd_object, object> drawer;
		common::read_only_object_value_property<drawing::solid_color_brush, object> color_brush;

		common::write_only_variant_value_property<object, parent_change_info, child_change_info> changed;

	protected:
		friend class application::object;
		friend class wnd_tree<object>;

		template <class, class>
		friend class events::typed_basic;

		virtual ewin::message::target *parent_() const override;

		void bind_properties_();

		virtual void handle_property_(void *prop, void *arg, common::property_access access);

		virtual ptr_type reflect_();

		virtual bool is_forbidden_(const property_forbidden_info &info);

		virtual void destruct_();

		virtual void create_(bool create, const create_info *info);

		virtual void low_level_create_(const common::types::create_struct &info, object *parent, application_type *app, attribute_option_type options);

		virtual void set_error_(common::variant_value_property_arg_info &info);

		virtual void set_error_(error_type value);

		virtual void set_error_(common::types::dword value);

		virtual void update_dimension_(dimension_type type);

		virtual void cache_dimensions_();

		virtual void filter_styles_(std::pair<common::types::uint *, common::types::uint> &info, bool is_extended) const;

		virtual common::types::uint filter_styles_(common::types::uint value, bool is_extended) const;

		virtual common::types::uint filtered_styles_(bool is_extended) const;

		virtual common::types::uint white_listed_styles_(bool is_extended) const;

		virtual common::types::uint black_listed_styles_(bool is_extended) const;

		virtual common::types::uint persistent_styles_(bool is_extended) const;

		virtual void send_message_(std::pair<message_info *, common::types::result> &info);

		virtual void post_message_(message_info &info);

		virtual bool is_dialog_message_(const common::types::msg &msg) const;

		virtual void screen_to_client_(common::types::point &point) const;

		virtual void screen_to_client_(common::types::rect &rect) const;

		virtual void client_to_screen_(common::types::point &point) const;

		virtual void client_to_screen_(common::types::rect &rect) const;

		virtual bool validate_parent_change_(object *value);

		virtual bool validate_child_remove_(object &value);

		virtual bool validate_child_add_(object &value, std::size_t index);

		virtual void child_removed_(object &value, std::size_t index);

		virtual void child_added_(object &value, std::size_t index);

		virtual void parent_changed_(object *current, object *previous, std::size_t index, std::size_t previous_index);

		virtual void event_listener_count_changed_(events::basic &e, std::size_t count);

		application_type *app_;
		common::types::hwnd handle_;
		wnd_event events_;
		cache_info cache_;

		bool auto_destroy_;
		error_throw_policy_type error_throw_policy_;

		error_type error_value_;
		common::types::dword local_error_value_;
		
		std::shared_ptr<ewin::menu::bar_collection> menu_;
		std::shared_ptr<ewin::menu::external_collection> system_menu_;

		drawing::hwnd_object drawer_;
		drawing::solid_color_brush color_brush_;
	};

	EWIN_MAKE_OPERATORS(object::attribute_option_type);
}

#endif /* !EWIN_WINDOW_OBJECT_H */
