#pragma once

#ifndef EWIN_MENU_RADIO_ITEM_H
#define EWIN_MENU_RADIO_ITEM_H

#include "menu_item.h"

namespace ewin::menu{
	class radio_item : public item{
	public:
		virtual ~radio_item();

	protected:
		virtual void on_select_(ewin::events::menu_message &e) override;
	};
}

#endif /* !EWIN_MENU_RADIO_ITEM_H */
