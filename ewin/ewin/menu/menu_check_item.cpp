#include "menu_check_item.h"

ewin::menu::check_item::~check_item(){
	destruct_();
}

void ewin::menu::check_item::on_select_(ewin::events::menu_message &e){
	set_state_(MFS_CHECKED, !get_state_(MFS_CHECKED));//Toggle
}
