#pragma once

#ifndef EWIN_MESSAGE_DISPATCHER_H
#define EWIN_MESSAGE_DISPATCHER_H

#include "../window/window_object.h"
#include "../events/event_callback.h"

#include "message_target.h"

namespace ewin::message{
	class dispatcher{
	public:
		typedef window::object window_type;

		virtual ~dispatcher() = default;

		virtual common::types::result dispatch(const common::types::msg &msg, window_type *target) = 0;
	};

	template <class event_type, class return_type, common::types::uint msg_code>
	class typed_dispatcher : public dispatcher{
	public:
		typedef events::typed_callback<window_type, return_type, event_type> callback_type;

		explicit typed_dispatcher(callback_type callback)
			: callback_(callback){}

		virtual ~typed_dispatcher() = default;

		virtual common::types::result dispatch(const common::types::msg &msg, window_type *target) override{
			event_type e(msg, callback_, target);
			return e.result;
		}

	protected:
		callback_type callback_;
	};
}

#endif /* !EWIN_MESSAGE_DISPATCHER_H */
