#pragma once

#ifndef EWIN_EVENT_OBJECT_H
#define EWIN_EVENT_OBJECT_H

#include <memory>

#include "../common/type_aliases.h"
#include "../common/boolean_property.h"
#include "../common/object_property.h"

namespace ewin::window{
	class object;
}

namespace ewin::events{
	class callback;

	class object{
	public:
		typedef window::object window_type;

		enum class state_type : unsigned int{
			nil						= (0 << 0x0000),
			default_prevented		= (1 << 0x0000),
			propagation_stopped		= (1 << 0x0001),
			bubbled					= (1 << 0x0002),
		};

		explicit object(window_type *target);

		common::read_only_object_value_property<window_type, object> target;
		common::boolean_value_property<object> prevent_default;
		common::boolean_value_property<object> stop_propagation;
		common::boolean_value_property<object> bubble;

	protected:
		void bind_properties_();

		virtual void handle_property_(void *prop, void *arg, common::property_access access);

		virtual void prevent_default_();

		window_type *target_;
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

		common::read_only_object_value_property<common::types::msg, message> info;
		common::value_property<common::types::result, object> result;

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

	EWIN_MAKE_OPERATORS(object::state_type);
}

#endif /* !EWIN_EVENT_OBJECT_H */
