#pragma once

#ifndef EWIN_MENU_POPUP_H
#define EWIN_MENU_POPUP_H

#include "menu_container.h"

namespace ewin::window{
	class object;
}

namespace ewin::menu{
	class popup;

	class popup_tracker{
	public:
		explicit popup_tracker(popup &owner);

		common::object_value_property<window::object, popup_tracker> target;
		common::state_value_property<common::types::uint, popup_tracker> flags;
		common::point_value_property<common::types::point, popup_tracker> position;
		common::write_only_boolean_value_property<popup_tracker> activate;

	private:
		popup &owner_;
		window::object *target_;
		common::types::uint flags_;
		common::types::point position_;
	};

	class popup : public container{
	public:
		popup();

		popup(const create_info &info);

		virtual ~popup();

		common::object_value_property<item, popup> owner;

		popup_tracker tracker;

	protected:
		friend class item;

		void bind_properties_();

		virtual void create_(bool create, const create_info *info) override;

		item *owner_;
	};
}

#endif /* !EWIN_MENU_POPUP_H */
