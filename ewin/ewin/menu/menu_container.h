#pragma once

#ifndef EWIN_MENU_CONTAINER_H
#define EWIN_MENU_CONTAINER_H

#include "menu_item.h"

namespace ewin::menu{
	class container : public object{
	public:
		container();

		virtual ~container();

	protected:
		virtual common::types::hmenu handle_value_() override;

		virtual void create_(bool create, const create_info *info) override;

		virtual bool validate_parent_change_(object *value) override;

		virtual void low_level_create_(bool popup);

		common::types::hmenu handle_;
	};
}

#endif /* !EWIN_MENU_CONTAINER_H */
