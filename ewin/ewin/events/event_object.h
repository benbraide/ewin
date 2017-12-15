#pragma once

#ifndef EWIN_EVENT_OBJECT_H
#define EWIN_EVENT_OBJECT_H

#include <memory>

#include "../common/type_aliases.h"
#include "../common/boolean_property.h"
#include "../common/object_property.h"

namespace ewin::message{
	class target;
}

namespace ewin::window{
	class object;
}

namespace ewin::events{
	class callback;

	class object{
	public:
		typedef ewin::message::target target_type;
		typedef ewin::window::object window_type;

		enum class state_type : unsigned int{
			nil						= (0 << 0x0000),
			default_called			= (1 << 0x0000),
			default_prevented		= (1 << 0x0001),
			propagation_stopped		= (1 << 0x0002),
			bubbled					= (1 << 0x0003),
		};

		explicit object(target_type *target);

		common::read_only_object_value_property<target_type, object> target;
		common::boolean_value_property<object> prevent_default;
		common::boolean_value_property<object> stop_propagation;
		common::boolean_value_property<object> bubble;

	protected:
		void bind_properties_();

		virtual void handle_property_(void *prop, void *arg, common::property_access access);

		virtual void prevent_default_();

		target_type *target_;
		state_type states_;
	};

	class message : public object{
	public:
		typedef callback callback_type;

		template <typename... args_types>
		message(common::types::msg &msg, callback_type &callback, args_types &&... args)
			: object(std::forward<args_types>(args)...), msg_(&msg), callback_(callback), result_(0){
			info.initialize_(msg_, nullptr);
			result.initialize_(nullptr, EWIN_PROP_HANDLER(message));
		}

		virtual ~message();

		common::read_only_object_value_property<common::types::msg, message> info;
		common::value_property<common::types::result, message> result;

	protected:
		virtual void handle_property_(void *prop, void *arg, common::property_access access) override;

		virtual common::types::result call_default_();

		common::types::msg *msg_;
		callback_type &callback_;
		common::types::result result_;
	};

	class mouse_activate : public message{
	public:
		template <typename... args_types>
		explicit mouse_activate(args_types &&... args)
			: message(std::forward<args_types>(args)...){
			auto handler = EWIN_PROP_HANDLER(mouse_activate);

			hit_target.initialize_(nullptr, handler);
			mouse_message.initialize_(nullptr, handler);
		}

		common::read_only_value_property<common::types::uint, mouse_activate> hit_target;
		common::read_only_value_property<common::types::uint, mouse_activate> mouse_message;

	protected:
		virtual void handle_property_(void *prop, void *arg, common::property_access access) override;
	};

	using set_cursor = mouse_activate;

	class pre_activate : public message{
	public:
		template <typename... args_types>
		explicit pre_activate(args_types &&... args)
			: message(std::forward<args_types>(args)...){
			is_activating.initialize_(nullptr, EWIN_PROP_HANDLER(pre_activate));
		}

		common::read_only_boolean_value_property<pre_activate> is_activating;

	protected:
		virtual void handle_property_(void *prop, void *arg, common::property_access access) override;
	};

	class activate : public message{
	public:
		template <typename... args_types>
		explicit activate(args_types &&... args)
			: message(std::forward<args_types>(args)...){
			auto handler = EWIN_PROP_HANDLER(activate);

			is_activated.initialize_(nullptr, handler);
			is_click_activated.initialize_(nullptr, handler);
		}

		common::read_only_boolean_value_property<activate> is_activated;
		common::read_only_boolean_value_property<activate> is_click_activated;

	protected:
		virtual void handle_property_(void *prop, void *arg, common::property_access access) override;
	};

	template <common::types::uint changing_message>
	class change : public message{
	public:
		template <typename... args_types>
		explicit change(args_types &&... args)
			: message(std::forward<args_types>(args)...){
			is_focused.initialize_(nullptr, EWIN_PROP_HANDLER(change));
		}

		common::read_only_boolean_value_property<change> is_focused;

	protected:
		virtual void handle_property_(void *prop, void *arg, common::property_access access) override{
			if (prop == &is_focused)
				*static_cast<common::types::uint *>(arg) = (msg_->message == changing_message);
			else//Forward
				message::handle_property_(prop, arg, access);
		}
	};

	using focus_change		= change<WM_SETFOCUS>;
	using position_change	= change<WM_WINDOWPOSCHANGING>;
	using size				= change<WM_SIZING>;
	using move				= change<WM_MOVING>;

	class enable : public message{
	public:
		template <typename... args_types>
		explicit enable(args_types &&... args)
			: message(std::forward<args_types>(args)...){
			is_enabled.initialize_(nullptr, EWIN_PROP_HANDLER(enable));
		}

		common::read_only_boolean_value_property<enable> is_enabled;

	protected:
		virtual void handle_property_(void *prop, void *arg, common::property_access access) override;
	};

	class hit_test : public message{
	public:
		template <typename... args_types>
		explicit hit_test(args_types &&... args)
			: message(std::forward<args_types>(args)...){
			mouse_position.initialize_(nullptr, EWIN_PROP_HANDLER(hit_test));
		}

		common::read_only_value_property<common::types::point, hit_test> mouse_position;

	protected:
		virtual void handle_property_(void *prop, void *arg, common::property_access access) override;
	};

	class style_change : public change<WM_STYLECHANGING>{
	public:
		typedef change<WM_STYLECHANGING> base_type;

		template <typename... args_types>
		explicit style_change(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){
			auto handler = EWIN_PROP_HANDLER(style_change);

			is_extended.initialize_(nullptr, handler);
			styles.initialize_(nullptr, handler);
		}

		virtual ~style_change();

		common::read_only_boolean_value_property<style_change> is_extended;
		common::read_only_value_property<common::types::style_struct *, style_change> styles;

	protected:
		virtual void handle_property_(void *prop, void *arg, common::property_access access) override;
	};

	EWIN_MAKE_OPERATORS(object::state_type);
}

#endif /* !EWIN_EVENT_OBJECT_H */
