#pragma once

#ifndef EWIN_WINDOW_EVENT_H
#define EWIN_WINDOW_EVENT_H

#include "../events/basic_event.h"

namespace ewin::window{
	class object;

	class wnd_event{
	public:
		typedef std::shared_ptr<object> object_ptr_type;
		typedef std::list<object_ptr_type> object_list_type;

		typedef object_list_type::iterator object_list_iterator_type;

		explicit wnd_event(object &target);

		wnd_event(const wnd_event &) = delete;

		wnd_event &operator =(const wnd_event &) = delete;

		ewin::events::typed_basic<ewin::events::message, object, false> create;
		ewin::events::typed_basic<ewin::events::message, object, false> destroy;
		ewin::events::typed_basic<ewin::events::message, object, false> close;

		ewin::events::typed_basic<ewin::events::mouse_activate, object, false> mouse_activate;
		ewin::events::typed_basic<ewin::events::pre_activate, object, false> pre_activate;
		ewin::events::typed_basic<ewin::events::activate, object, false> activate;

		ewin::events::typed_basic<ewin::events::message, object, false> cancel_mode;
		ewin::events::typed_basic<ewin::events::focus_change, object, false> focus_change;
		ewin::events::typed_basic<ewin::events::enable, object, false> enable;

		ewin::events::typed_basic<ewin::events::set_cursor, object, false> set_cursor;
		ewin::events::typed_basic<ewin::events::hit_test, object, false> hit_test;

		ewin::events::typed_basic<ewin::events::position_change, object, false> position_change;
		ewin::events::typed_basic<ewin::events::size, object, false> size;
		ewin::events::typed_basic<ewin::events::move, object, false> move;
		ewin::events::typed_basic<ewin::events::style_change, object, false> style_change;

		ewin::events::typed_basic<ewin::events::message, object, false> unknown_message;
	};
}

#endif /* !EWIN_WINDOW_EVENT_H */
