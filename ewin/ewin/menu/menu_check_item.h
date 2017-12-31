#pragma once

#ifndef EWIN_MENU_CHECK_ITEM_H
#define EWIN_MENU_CHECK_ITEM_H

#include "menu_item.h"

namespace ewin::menu{
	class check_item : public item{
	public:
		virtual ~check_item();

	protected:
		virtual void on_select_(ewin::events::menu_message &e) override;
	};
}

#endif /* !EWIN_MENU_CHECK_ITEM_H */
