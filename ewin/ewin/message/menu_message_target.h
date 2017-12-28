#pragma once

#ifndef EWIN_MENU_MESSAGE_TARGET_H
#define EWIN_MENU_MESSAGE_TARGET_H

#include "../common/transformation_property.h"

#include "../events/event_callback.h"
#include "../events/basic_event.h"

namespace ewin::events{
	class menu_object;
}

namespace ewin::message{
	class menu_target{
	public:
		typedef std::function<void(events::message &, bool)> dispatch_callback_type;
		typedef std::function<void(common::types::msg &)> bubble_callback_type;

		common::transformation_property<common::types::msg, common::types::result, target> dispatch_message;

	protected:
		virtual target *parent_() const = 0;
	};
}

#endif /* !EWIN_MENU_MESSAGE_TARGET_H */
