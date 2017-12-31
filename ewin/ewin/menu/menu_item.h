#pragma once

#ifndef EWIN_MENU_ITEM_H
#define EWIN_MENU_ITEM_H

#include "menu_object.h"

namespace ewin::menu{
	class popup;

	class item : public object{
	public:
		struct cache_info{
			common::types::word id;
			std::wstring label;
			std::wstring shortcut;
			common::types::uint states;
			common::types::uint types;
			common::types::hbitmap bitmap;
			common::types::hbitmap checked_bitmap;
			common::types::hbitmap unchecked_bitmap;
		};

		item();

		virtual ~item();

		common::numeric_value_property<common::types::word, item> id;
		common::object_value_property<popup, item> sub_menu;

		common::string_value_property<std::wstring, item> label;
		common::string_value_property<std::wstring, item> shortcut;

		common::state_value_property<common::types::uint, item> states;
		common::boolean_value_property<item> enabled;
		common::boolean_value_property<item> is_default;
		common::boolean_value_property<item> checked;
		common::read_only_boolean_value_property<item> owner_drawn;

		common::value_property<common::types::hbitmap, item> bitmap;
		common::value_property<common::types::hbitmap, item> checked_bitmap;
		common::value_property<common::types::hbitmap, item> unchecked_bitmap;

	protected:
		void bind_properties_();

		virtual void handle_property_(void *prop, void *arg, common::property_access access) override;

		virtual void destruct_() override;

		virtual void create_(bool create, const create_info *info) override;

		virtual bool validate_child_remove_(object &value) override;

		virtual bool validate_child_add_(object &value, std::size_t index) override;

		virtual void parent_changed_(object *current, object *previous, std::size_t index, std::size_t previous_index) override;

		virtual void event_listener_count_changed_(events::menu_basic &e, std::size_t count) override;

		virtual bool on_init_(ewin::events::menu_message &e) override;

		virtual bool is_owner_drawn_();

		virtual void low_level_create_();

		virtual void low_level_create_(common::types::hmenu handle, common::types::uint index);

		virtual void update_id_();

		virtual void update_label_();

		virtual void update_sub_menu_();

		virtual void update_states_();

		virtual void update_types_();

		virtual void update_bitmap_();

		virtual void update_check_marks_();

		virtual void update_(const common::types::menu_item_info &info);

		virtual void set_state_(common::types::uint state, bool add);

		virtual bool get_state_(common::types::uint state);

		virtual void set_sub_menu_(popup *value);

		virtual void split_label_();

		bool created_;
		popup *sub_menu_;
		cache_info cache_;
	};
}

#endif /* !EWIN_MENU_ITEM_H */
