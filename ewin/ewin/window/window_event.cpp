#include "window_object.h"

ewin::window::wnd_event::wnd_event(object &target)
	: create(target), destroy(target), close(target), mouse_activate(target), pre_activate(target), activate(target), cancel_mode(target),
	focus_change(target), enable(target), set_cursor(target), get_cursor(target), hit_test(target), position_change(target), size(target),
	move(target), style_change(target), unknown_message(target){}
