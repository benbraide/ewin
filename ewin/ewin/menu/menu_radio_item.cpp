#include "menu_radio_item.h"

ewin::menu::radio_item::~radio_item(){
	destruct_();
}

void ewin::menu::radio_item::on_select_(ewin::events::menu_message &e){
	if (get_state_(MFS_CHECKED))
		return;//No changes

	radio_item *previous_item = dynamic_cast<radio_item *>(static_cast<object *>(tree.previous_sibling));
	while (previous_item != nullptr){//Remove check from previous items in group
		previous_item->set_state_(MFS_CHECKED, false);
		previous_item = dynamic_cast<radio_item *>(static_cast<object *>(previous_item->tree.previous_sibling));
	}

	radio_item *next_item = dynamic_cast<radio_item *>(static_cast<object *>(tree.next_sibling));
	while (next_item != nullptr){//Remove check from next items in group
		next_item->set_state_(MFS_CHECKED, false);
		next_item = dynamic_cast<radio_item *>(static_cast<object *>(next_item->tree.next_sibling));
	}

	set_state_(MFS_CHECKED, true);
}
