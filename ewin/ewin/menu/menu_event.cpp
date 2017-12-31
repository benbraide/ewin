#include "menu_object.h"

ewin::menu::menu_event::menu_event(object &target)
	: create(target), destroy(target), highlight(target), select(target), check(target),
	init(target), draw(target), measure(target), unknown_message(target){}
