#pragma once

#ifndef EWIN_MENU_SEPARATOR_H
#define EWIN_MENU_SEPARATOR_H

#include "menu_object.h"

namespace ewin::menu{
	class separator : public object{
	public:
		separator();

		virtual ~separator();

	protected:
		virtual void create_(bool create, const create_info *info) override;

		virtual bool validate_child_remove_(object &value) override;

		virtual bool validate_child_add_(object &value, std::size_t index) override;

		virtual void parent_changed_(object *current, object *previous, std::size_t index, std::size_t previous_index) override;

		virtual void low_level_create_();

		virtual void low_level_create_(common::types::hmenu handle, common::types::uint index);

		bool created_;
	};
}

#endif /* !EWIN_MENU_SEPARATOR_H */
