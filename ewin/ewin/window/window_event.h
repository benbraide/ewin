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

		ewin::events::typed_basic<ewin::events::message, object> create;
		ewin::events::typed_basic<ewin::events::message, object> destroy;
		ewin::events::typed_basic<ewin::events::message, object> close;

		ewin::events::typed_basic<ewin::events::mouse_activate, object> mouse_activate;
		ewin::events::typed_basic<ewin::events::pre_activate, object> pre_activate;
		ewin::events::typed_basic<ewin::events::activate, object> activate;

		ewin::events::typed_basic<ewin::events::message, object> cancel_mode;
		ewin::events::typed_basic<ewin::events::focus_change, object> focus_change;
		ewin::events::typed_basic<ewin::events::enable, object> enable;

		ewin::events::typed_basic<ewin::events::cursor, object> set_cursor;
		ewin::events::typed_basic<ewin::events::cursor, object> get_cursor;
		ewin::events::typed_basic<ewin::events::hit_test, object> hit_test;

		ewin::events::typed_basic<ewin::events::position_change, object> position_change;
		ewin::events::typed_basic<ewin::events::size, object> size;
		ewin::events::typed_basic<ewin::events::move, object> move;
		ewin::events::typed_basic<ewin::events::style_change, object> style_change;

		ewin::events::typed_basic<ewin::events::draw, object> background_erase;
		ewin::events::typed_basic<ewin::events::message, object> get_background_brush;
		ewin::events::typed_basic<ewin::events::message, object> get_background_color;

		ewin::events::typed_basic<ewin::events::draw, object> non_client_paint;
		ewin::events::typed_basic<ewin::events::draw, object> paint;
		ewin::events::typed_basic<ewin::events::draw, object> print_client;

		ewin::events::typed_basic<ewin::events::mouse, object> mouse_leave;
		ewin::events::typed_basic<ewin::events::mouse, object> mouse_enter;

		ewin::events::typed_basic<ewin::events::mouse, object> mouse_move;
		ewin::events::typed_basic<ewin::events::mouse, object> mouse_hover;

		ewin::events::typed_basic<ewin::events::mouse, object> left_mouse_down;
		ewin::events::typed_basic<ewin::events::mouse, object> middle_mouse_down;
		ewin::events::typed_basic<ewin::events::mouse, object> right_mouse_down;

		ewin::events::typed_basic<ewin::events::mouse, object> left_mouse_up;
		ewin::events::typed_basic<ewin::events::mouse, object> middle_mouse_up;
		ewin::events::typed_basic<ewin::events::mouse, object> right_mouse_up;

		ewin::events::typed_basic<ewin::events::mouse, object> left_mouse_double_click;
		ewin::events::typed_basic<ewin::events::mouse, object> middle_mouse_double_click;
		ewin::events::typed_basic<ewin::events::mouse, object> right_mouse_double_click;

		ewin::events::typed_basic<ewin::events::mouse, object> mouse_wheel;
		ewin::events::typed_basic<ewin::events::mouse_drag, object> mouse_drag;
		ewin::events::typed_basic<ewin::events::mouse, object> mouse_drag_end;

		ewin::events::typed_basic<ewin::events::context_menu, object> context_menu;
		ewin::events::typed_basic<ewin::events::key_down, object> key_down;
		ewin::events::typed_basic<ewin::events::key_down, object> system_key_down;

		ewin::events::typed_basic<ewin::events::key, object> key_up;
		ewin::events::typed_basic<ewin::events::key, object> system_key_up;

		ewin::events::typed_basic<ewin::events::key_press, object> key_press;
		ewin::events::typed_basic<ewin::events::key_press, object> system_key_press;

		ewin::events::typed_basic<ewin::events::key_press, object> dead_key;
		ewin::events::typed_basic<ewin::events::key_press, object> dead_system_key;

		ewin::events::typed_basic<ewin::events::message, object> unknown_message;
	};
}

#endif /* !EWIN_WINDOW_EVENT_H */
