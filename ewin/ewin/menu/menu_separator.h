#pragma once

#ifndef EWIN_MENU_SEPARATOR_H
#define EWIN_MENU_SEPARATOR_H

#include "menu_object.h"

namespace ewin::menu{
	class separator : public object{
	public:
		separator();

		virtual ~separator();

		common::read_only_boolean_value_property<separator> owner_drawn;

	protected:
		virtual void create_(bool create, const create_info *info) override;

		virtual bool validate_child_remove_(object &value) override;

		virtual bool validate_child_add_(object &value, std::size_t index) override;

		virtual void parent_changed_(object *current, object *previous, std::size_t index, std::size_t previous_index) override;

		virtual void event_listener_count_changed_(events::menu_basic &e, std::size_t count) override;

		virtual bool is_owner_drawn_();

		virtual void low_level_create_();

		virtual void low_level_create_(common::types::hmenu handle, common::types::uint index);

		virtual void update_types_();

		virtual void update_(const common::types::menu_item_info &info);

		bool created_;
		common::types::uint types_;
	};
}

#endif /* !EWIN_MENU_SEPARATOR_H */
