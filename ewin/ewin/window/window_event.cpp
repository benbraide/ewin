#include "window_object.h"

ewin::window::wnd_event::wnd_event(object &target)
	: create(target), destroy(target), close(target), mouse_activate(target), pre_activate(target), activate(target), cancel_mode(target),
	focus_change(target), enable(target), set_cursor(target), get_cursor(target), hit_test(target), position_change(target), size(target),
	move(target), style_change(target), background_erase(target), get_background_brush(target), get_background_color(target), non_client_paint(target),
	paint(target), print_client(target), mouse_leave(target), mouse_enter(target), mouse_move(target), mouse_hover(target), left_mouse_down(target),
	middle_mouse_down(target), right_mouse_down(target), left_mouse_up(target), middle_mouse_up(target), right_mouse_up(target), left_mouse_double_click(target),
	middle_mouse_double_click(target), right_mouse_double_click(target), mouse_wheel(target), mouse_drag(target), mouse_drag_end(target), context_menu(target),
	key_down(target), system_key_down(target), key_up(target), system_key_up(target), key_press(target), system_key_press(target), dead_key(target),
	dead_system_key(target), unknown_message(target){}
