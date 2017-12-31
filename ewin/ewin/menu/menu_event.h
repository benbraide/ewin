#pragma once

#ifndef EWIN_MENU_EVENT_H
#define EWIN_MENU_EVENT_H

#include "../events/basic_event.h"

namespace ewin::menu{
	class object;

	class menu_event{
	public:
		explicit menu_event(object &target);

		menu_event(const menu_event &) = delete;

		menu_event &operator =(const menu_event &) = delete;

		ewin::events::typed_basic<ewin::events::menu_message, object> create;
		ewin::events::typed_basic<ewin::events::menu_message, object> destroy;

		ewin::events::typed_basic<ewin::events::menu_message, object> highlight;
		ewin::events::typed_basic<ewin::events::menu_message, object> select;

		ewin::events::typed_basic<ewin::events::menu_message, object> init;

		ewin::events::typed_basic<ewin::events::menu_message, object> draw;
		ewin::events::typed_basic<ewin::events::menu_message, object> measure;

		ewin::events::typed_basic<ewin::events::menu_message, object> unknown_message;
	};
}

#endif /* !EWIN_MENU_EVENT_H */
